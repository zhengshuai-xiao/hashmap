#include "hashmap.h"

template <typename K, typename V, typename HASH=hash<K>>
class CustomHashMap {
private:
    static const uint32 BUCKET_SIZE=10;
    static const uint32 MUTEX_NUM=128;
    uint32 m_capacity;
    atomic_uint64_t m_currentNum;
    vector<list<pair<K,V>>> m_buckets;
    mutable vector<shared_mutex> m_bucketMutexes;

    uint64 getMax(){
        return m_capacity*BUCKET_SIZE;
    }

    uint32 getBucketIndex(const K& key) const {
        return HASH{}(key)%m_buckets.size();
    }
    uint32 getBucketMutexIndex(const K& key) const{
        return HASH{}(key)%m_bucketMutexes.size();
    }
    int expend(){
        return 0;
    }

public:
    CustomHashMap(uint32 capacity, uint32 mutexNum=MUTEX_NUM):
    m_capacity(capacity),
    m_currentNum(0),
    m_buckets(m_capacity),
    m_bucketMutexes(mutexNum){
        cout<<"create CustomHashMap,capacity:"<<m_capacity<<endl;
    }
    
    int put(const K& key, const V& val){
        uint32 index = getBucketIndex(key);
        {
            unique_lock<shared_mutex> lock(m_bucketMutexes[getBucketMutexIndex(key)]);
            list<pair<K,V>>& bucket=m_buckets[index];

            for(auto& pair_:bucket){
                if(pair_.first==key){
                    pair_.second=val;
                    return 0;
                }
            }

            if(m_currentNum+1 >= m_capacity*BUCKET_SIZE*0.8){
                //TODO: extend
                cout<<"too many nodes, need to extend"<<endl;
                return -1;
            }

            bucket.emplace_back(key,val);
            m_currentNum++;
        }
        return 0;
    }

    V get(const K& key) const {
        uint32 index = getBucketIndex(key);
        shared_lock<shared_mutex> lock(m_bucketMutexes[getBucketMutexIndex(key)]);
        auto& bucket = m_buckets[index];

        for(auto& pair_:bucket){
            if(pair_.first==key){
                return pair_.second;
            }
        }
        cerr<<"did not find the key"<<endl;
        throw std::runtime_error("Key not found");
    }
    void getMem(){
        cout<<"m_currentNum:"<<m_currentNum<<endl;
        uint32 bucketMem=m_buckets.size()*sizeof(m_buckets[0]);
        uint32 nodeMem = m_currentNum*(sizeof(V)+sizeof(K));
        cout<<"bucketMem:"<<bucketMem<<endl;
        cout<<"nodeMem:"<<nodeMem<<endl;
        //not accurate
        cout<<"total:"<<(bucketMem+nodeMem)/(1024*1024)<<"MiB"<<" not accurate!"<<endl;
        
    }

    void testInsert(uint64 total, uint32_t* pdata){
        for(uint64 i=0;i<total;i++){
            uint64 fp=pdata[i];
            put(fp,indexNode(fp));
        }
    }

};

void testCustomHashMap(uint32 capBit, uint32 NodeBit){
    cout<<"using:"<<__FUNCTION__<<endl;

    uint64 max=1UL<<NodeBit;
    auto timeStamp1=chrono::high_resolution_clock::now();

    CustomHashMap<fingerPrint,indexNode,myHash> myHashMap(1<<capBit);
    
    auto timeStamp2=chrono::high_resolution_clock::now();

    myHashMap.testInsert(max,random_data.getPData(0));

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

void testCustomHashMapWithMultiThreads(uint32 capBit, uint32 NodeBit, uint32 threadNum, uint32 mutexNum){
    cout<<"using:"<<__FUNCTION__<<endl;

    uint64 max=1UL<<NodeBit;
    auto timeStamp1=chrono::high_resolution_clock::now();
    CustomHashMap<fingerPrint,indexNode,myHash> myHashMap(1<<capBit, mutexNum);

    auto timeStamp2=chrono::high_resolution_clock::now();

    vector<thread> threads(threadNum);
    for(uint32 i=0;i<threadNum;i++){
        threads[i]=thread{&CustomHashMap<fingerPrint,indexNode,myHash>::testInsert,&myHashMap,max/threadNum,random_data.getPData(i)};
    }
    for(auto& t:threads){
        t.join();
    }

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