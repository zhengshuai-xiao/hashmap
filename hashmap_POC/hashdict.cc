#include "hashdict.h"
#include "randomData.h"

#define BUCKET_SIZE 16
#define HANDLE_THREAD_NUM 1

struct myHash{
    uint32_t operator()(const uint64_t &fp) const {
        return (uint32_t)(fp&0xFFFFFFFF);
    }
};

//{
    template <typename K, typename V, typename HASH>
    hashdict<K,V,HASH>::hashdict(uint32_t hashPower):m_hashPower(hashPower),
    m_maxListLength(BUCKET_SIZE),
    m_currentNum(0),
    io_queue_("handling thread queue",QUEUE_CAPACITY),
    terminated_(false)
    {
        m_cache = (node**)calloc(hashsize(m_hashPower), sizeof(node*));
        if(m_cache==nullptr){
            LOG(ERR,"%s:hashdict created failed", __FUNCTION__);
            return;
        }
        
        pnotify_=new Notify<io_response>[2] {std::string("notify test1"),std::string("notify test2")};
        //for(uint32_t i=0;i<MAX_REQUEST_THREAD;i++){
        //    new (&pnotify_[i]) Notify<io_response>(std::string("notify test1"));
        //}

        if(pnotify_==nullptr){
            LOG(ERR,"%s:notify created failed", __FUNCTION__);
            return;
        }
        
        LOG(INFO,"%s:created hashdict,capacity:%llu, total size:%llu", __FUNCTION__, hashsize(m_hashPower),sizeof(node*)*hashsize(m_hashPower));
        for(int i=0;i<HANDLE_THREAD_NUM;i++){
            t_.emplace_back(thread(&hashdict<K,V,HASH>::threadfunc, this));
        }
        
    }
    template <typename K, typename V, typename HASH>
    int hashdict<K,V,HASH>::put(const K key, const V val){
        uint32_t index = getBucketIndex(key);
        {
            //unique_lock<shared_mutex> lock(m_bucketMutexes[getBucketMutexIndex(key)]);
            node** bucket=&(m_cache[index]);
            node* curNode=*bucket;
            uint32_t count=0;
            while(curNode){
                count++;
                if(curNode->key_==key){
                    curNode->val_=val;
                    return 1;
                }
                curNode=curNode->next_;
            }
            if(count>m_maxListLength) {
                LOG(INFO,"%s:Warning: the count[%u] is greater than %d", __FUNCTION__, count, BUCKET_SIZE);
            }

            //did not find same node, insert the new.
            if(m_currentNum+1 >= hashsize(m_hashPower)*m_maxListLength*0.8){
                //TODO: extend
                LOG(INFO,"%s::too many nodes, need to extend", __FUNCTION__);
                return -1;
            }
            if(*bucket==nullptr){
                *bucket=new node(key,val,nullptr);
            }
            else{
                curNode=*bucket;
                node* n=curNode->next_;
                curNode->next_=new node(key,val,n);
            }
            m_currentNum++;
            
        }
        return 0;
    }

    template <typename K, typename V, typename HASH>
    int hashdict<K,V,HASH>::get(const K key, V& value) {
        uint32_t index = getBucketIndex(key);
        //shared_lock<shared_mutex> lock(m_bucketMutexes[getBucketMutexIndex(key)]);
        
        node* bucket = m_cache[index];
        node* curNode=bucket;
        while(curNode){
            if(curNode->key_==key){
                value=curNode->val_;
                return 0;
            }
            curNode=curNode->next_;
        }
        
        LOG(ERR,"%s:did not find the key", __FUNCTION__);
        return -1;
    }
    template <typename K, typename V, typename HASH>
    void hashdict<K,V,HASH>::handleItem(io_request* item){
        int ret=0;
        switch(item->type_){
        case SET:
            ret = put(item->item_.key_,item->item_.val_);
            pnotify_[item->notify_index_].set_value(io_response(item->id_,item->item_,ret>=0?SUCCESSFUL:FAILED));
            break;
        case GET:
            ret = get(item->item_.key_,item->item_.val_);
            pnotify_[item->notify_index_].set_value(io_response(item->id_,item->item_,ret>=0?SUCCESSFUL:FAILED));
            break;
        case DEL:
            LOG(ERR,"%s:DEL: not implemented", __FUNCTION__);
            break;
        case TERMINATE:
            LOG(INFO,"%s:TERMINATE", __FUNCTION__);
            break;
        default:
            LOG(ERR,"%s:unknown operation:%d", __FUNCTION__,item->type_);
            break;
        }
    }

    template <typename K, typename V, typename HASH>
    void hashdict<K,V,HASH>::threadfunc(){
        uint64_t count=0;
        LOG(INFO,"%s:start", __FUNCTION__);
        while(!terminated_){

            io_request* req=io_queue_.receive();

            handleItem(req);
            count++;
            
        }

        LOG(INFO,"%s:end, handled total events count:%u", __FUNCTION__,count);
    }

    void testInsert(hashdict<uint64_t,uint64_t,myHash> *myHashDict, uint64_t total, uint32_t index,uint32_t* pdata){
        stringstream name;
        name<<"testInsert_"<<index;
        uint32_t notify_index = myHashDict->createNotify(name.str());
        uint32_t id=0;
        for(uint64_t i=0;i<total;i++){
            uint64_t fp=pdata[i];
            hashdict<uint64_t,uint64_t,myHash>::io_request ioRes(id++,hashdict<uint64_t,uint64_t,myHash>::req_item(fp,fp),notify_index,SET);
            if(myHashDict->request(ioRes)!=SUCCESSFUL){
                LOG(ERR,"%s:get response failed id:%u", __FUNCTION__,id);
            }
        }
        myHashDict->destoryNotify(notify_index);
    }
//}

void testCustomHashDictWithMultiThreads(uint32_t capBit, uint32_t NodeBit, uint32_t threadNum, uint32_t partNum,uint32_t mutexNum)
{
    LOG(INFO,"%s:is using", __FUNCTION__);

    uint64_t max=1UL<<NodeBit;
    auto timeStamp1=std::chrono::high_resolution_clock::now();
    hashdict<uint64_t,uint64_t,myHash> myHashDict(capBit);
    sleep(1);

    auto timeStamp2=std::chrono::high_resolution_clock::now();
#if 1
    std::vector<std::thread> threads(threadNum);
    for(uint32_t i=0;i<threadNum;i++){
        threads[i]=std::thread{&testInsert,&myHashDict,max/threadNum, i, random_data.getPData(i)};
    }
    for(auto& t:threads){
        t.join();
    }
#endif
    //myHashMap.testInsert(0,threadNum,max);
    auto timeStamp3=std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> d1=timeStamp2-timeStamp1;
    std::chrono::duration<double> d2=timeStamp3-timeStamp2;
    std::chrono::duration<double> d_total=timeStamp3-timeStamp1;

    LOG(INFO,"%s:duration1:%f", __FUNCTION__, d1.count());
    LOG(INFO,"%s:duration2:%f", __FUNCTION__, d2.count());
    LOG(INFO,"%s:total duration:%f", __FUNCTION__, d_total.count());
    //validate the map
    /*
    myHashDict.put(1234,12345678);
    indexNode node = (myHashDict.get(1234));
    assert(node.dcid==12345678);
    
    myHashDict.getMem();*/
}

//./CustomHashMap  -me 4 -cb 9 -nb 10 -t 1 -p 1 -mu 1 -s 10