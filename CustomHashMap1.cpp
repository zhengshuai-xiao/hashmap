#include "hashmap.h"

int BUCKET_SIZE=16;

template <typename K, typename V, typename HASH=hash<K>>
class CustomHashMap1 {
private:
    static const int MUTEX_NUM=64;
    uint32 m_capacity;
    atomic_uint64_t m_currentNum;
    mutable vector<shared_mutex> m_bucketMutexes;
    struct node{
        K key;
        V val;
        node* next;

        node(K key_, V val_,node* n):key(key_),
        val(val_),
        next(n)
        {}
    };
    node** m_buckets;

    uint64 getMax(){
        return m_capacity*BUCKET_SIZE;
    }

    uint32 getBucketIndex(const K& key) const {
        return HASH{}(key)%m_capacity;
    }
    uint32 getBucketMutexIndex(const K& key) const{
        return HASH{}(key)%m_bucketMutexes.size();
    }
    int expend(){
        return 0;
    }
public:

    CustomHashMap1(uint32 capacity, int mutex_num=MUTEX_NUM):m_capacity(capacity),
    m_currentNum(0),
    m_bucketMutexes(mutex_num){
        m_buckets = (node**)calloc(m_capacity, sizeof(node*));
        if(m_buckets==nullptr){
            cerr<<"CustomHashMap created failed"<<endl;
            return;
        }
        cout<<"created CustomHashMap,capacity:"<<m_capacity<<", total size:"<<sizeof(node*)*m_capacity<<endl;
    }
    ~CustomHashMap1(){
        for(uint32 i=0;i<m_capacity;i++){
            node* curNode = m_buckets[i];
            while(curNode){
                node* n=curNode->next;
                delete curNode;
                curNode=n;
            }
        }
        free(m_buckets);
        m_buckets=nullptr;
    }
    int put(const K& key, const V& val){
        uint32 index = getBucketIndex(key);
        {
            unique_lock<shared_mutex> lock(m_bucketMutexes[getBucketMutexIndex(key)]);
            node** bucket=&(m_buckets[index]);
            node* curNode=*bucket;
            int count=0;
            while(curNode){
                count++;
                if(curNode->key==key){
                    curNode->val=val;
                    return 1;
                }
                curNode=curNode->next;
            }
            if(count>BUCKET_SIZE) {
                cout<<"Warning: the count["<<count<< "] is greater than "<<BUCKET_SIZE<<endl;
            }

            //did not find same node, insert the new.
            if(m_currentNum+1 >= m_capacity*BUCKET_SIZE*0.8){
                //TODO: extend
                cout<<"too many nodes, need to extend"<<endl;
                return -1;
            }
            if(*bucket==nullptr){
                *bucket=new node(key,val,nullptr);
            }
            else{
                curNode=*bucket;
                node* n=curNode->next;
                curNode->next=new node(key,val,n);
            }
            m_currentNum++;
            
        }
        return 0;
    }

    V get(const K& key) const {
        uint32 index = getBucketIndex(key);
        shared_lock<shared_mutex> lock(m_bucketMutexes[getBucketMutexIndex(key)]);
        
        node* bucket = m_buckets[index];
        node* curNode=bucket;
        while(curNode){
            if(curNode->key==key){
                return curNode->val;
            }
            curNode=curNode->next;
        }
        
        cerr<<"did not find the key"<<endl;
        return V(0);
    }

    void getMem(){
        cout<<"m_currentNum:"<<m_currentNum<<"*"<<sizeof(node)<<endl;
        uint32 bucketMem=sizeof(node*)*m_capacity;
        uint32 nodeMem = m_currentNum*sizeof(node);
        cout<<"bucketMem:"<<bucketMem<<endl;
        cout<<"nodeMem:"<<nodeMem<<endl;
        cout<<"total:"<<(bucketMem+nodeMem)/(1024*1024)<<"MiB"<<endl;
    }
    uint32 getNodeSize(){
        return sizeof(node);
    }
    uint32 getCount(){
        return m_currentNum;
    }

    void testInsert(uint64 total, uint32_t* pdata){

        for(uint64 i=0;i<total;i++){
            uint64 fp=pdata[i];
            put(fp,indexNode(fp));
        }
    }
};

template <typename K, typename V, typename HASH=hash<K>>
class CustomHashMapGroup{
private:
    static const uint32 PARTITION_NUM=64;
    uint32 m_capacity;
    atomic_uint64_t m_currentNum;
    uint32 m_partitionNum;

    typedef shared_ptr<CustomHashMap1<K,V,HASH>> CustomHashMap1_Ptr;
    vector<CustomHashMap1_Ptr> myHashMapGroup;

    uint64 getMax(){
        return m_capacity*BUCKET_SIZE;
    }

    uint32 getPartitionIndex(const K& key) const {
        uint32 part = HASH{}(key);
        return (part>>24)%m_partitionNum;
    }

public:
    CustomHashMapGroup(uint32 capacity, uint32 partition_num=PARTITION_NUM, uint32 mutexNum=32):m_capacity(capacity),
    m_currentNum(0),
    m_partitionNum(partition_num),
    myHashMapGroup(m_partitionNum){
        for(uint32 i=0;i<m_partitionNum;i++){
            myHashMapGroup[i]=CustomHashMap1_Ptr(new CustomHashMap1<K,V,HASH>(m_capacity/m_partitionNum,mutexNum));
        }
    }

    int put(const K& key, const V& val){
        m_currentNum++;
        uint32 partition=getPartitionIndex(key);
        //cout<<"put"<<key<<"partition:"<<partition<<endl;
        return myHashMapGroup[partition]->put(key,val);
    }
    V get(const K& key){
        uint32 partition=getPartitionIndex(key);
        return myHashMapGroup[partition]->get(key);
    }

    void getMem(){
#if 0        
        for(uint32 i=0;i<m_partitionNum;i++){
            cout<<"Part:"<<i<<" count:"<<myHashMapGroup[i]->getCount()<<endl;
        }
#endif  

        uint32 nodeSize=myHashMapGroup[0]->getNodeSize();
        cout<<"m_currentNum:"<<m_currentNum<<endl;
        uint32 bucketMem=sizeof(void*)*m_capacity;
        uint32 nodeMem = m_currentNum*nodeSize;
        cout<<"bucketMem:"<<bucketMem<<endl;
        cout<<"nodeMem:"<<nodeMem<<endl;
        cout<<"total:"<<(bucketMem+nodeMem)/(1024*1024)<<"MiB"<<endl;
    }

    void testInsert(uint64 total, uint32_t* pdata){
        for(uint64 i=0;i<total;i++){
            uint64 fp=pdata[i];
            put(fp,indexNode(fp));
        }
    }

};

void testCustomHashMap1WithMultiThreads(uint32 capBit, uint32 NodeBit, uint32 threadNum, uint32 mutexNum){
    cout<<"using:"<<__FUNCTION__<<endl;

    uint64 max=1UL<<NodeBit;
    auto timeStamp1=chrono::high_resolution_clock::now();
    CustomHashMap1<fingerPrint,indexNode,myHash> myHashMap(1<<capBit,mutexNum);

    auto timeStamp2=chrono::high_resolution_clock::now();
#if 1
    vector<thread> threads(threadNum);
    for(uint32 i=0;i<threadNum;i++){
        threads[i]=thread{&CustomHashMap1<fingerPrint,indexNode,myHash>::testInsert,&myHashMap,max/threadNum, random_data.getPData(i)};
    }
    for(auto& t:threads){
        t.join();
    }
#endif
    //myHashMap.testInsert(0,threadNum,max);
    auto timeStamp3=chrono::high_resolution_clock::now();
    chrono::duration<double> d1=timeStamp2-timeStamp1;
    chrono::duration<double> d2=timeStamp3-timeStamp2;
    chrono::duration<double> d_total=timeStamp3-timeStamp1;
    cout<<"duration1:"<<d1.count()<<endl;
    cout<<"duration2:"<<d2.count()<<endl;
    cout<<"total duration:"<<d_total.count()<<endl;
    //validate the map
    myHashMap.put(1234,12345678);
    indexNode node = (myHashMap.get(1234));
    assert(node.dcid==12345678);

    myHashMap.getMem();
}

void testCustomHashMapGroupWithMultiThreads(uint32 capBit, uint32 NodeBit, uint32 threadNum, uint32 partNum,uint32 mutexNum){
    cout<<"using:"<<__FUNCTION__<<endl;

    uint64 max=1UL<<NodeBit;
    auto timeStamp1=chrono::high_resolution_clock::now();
    CustomHashMapGroup<fingerPrint,indexNode,myHash> myHashMapGroup(1<<capBit,partNum, mutexNum);

    auto timeStamp2=chrono::high_resolution_clock::now();
#if 1
    vector<thread> threads(threadNum);
    for(uint32 i=0;i<threadNum;i++){
        threads[i]=thread{&CustomHashMapGroup<fingerPrint,indexNode,myHash>::testInsert,&myHashMapGroup,max/threadNum, random_data.getPData(i)};
    }
    for(auto& t:threads){
        t.join();
    }
#endif
    //myHashMap.testInsert(0,threadNum,max);
    auto timeStamp3=chrono::high_resolution_clock::now();
    chrono::duration<double> d1=timeStamp2-timeStamp1;
    chrono::duration<double> d2=timeStamp3-timeStamp2;
    chrono::duration<double> d_total=timeStamp3-timeStamp1;
    cout<<"duration1:"<<d1.count()<<endl;
    cout<<"duration2:"<<d2.count()<<endl;
    cout<<"total duration:"<<d_total.count()<<endl;
    //validate the map
    myHashMapGroup.put(1234,12345678);
    indexNode node = (myHashMapGroup.get(1234));
    assert(node.dcid==12345678);

    myHashMapGroup.getMem();
}