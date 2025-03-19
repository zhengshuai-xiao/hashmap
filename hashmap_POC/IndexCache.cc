#include "IndexCache.h"
#include "randomData.h"

#define BUCKET_SIZE 16
#define HANDLE_THREAD_NUM 1

#define hashsize(n) ((uint64_t)1<<(n))
#define hashmask(n) (hashsize(n)-1)

struct myHash{
    uint32_t operator()(const uint64_t &fp) const {
        return (uint32_t)(fp&0xFFFFFFFF);
    }
};
void CheckFree(void** point){
    if(*point!=nullptr){
        free(*point);
        *point=nullptr;
    }
    else{
        LOG(WARN,"%s::the point is a nullptr", __FUNCTION__);
    }
}

namespace cache{
    template <typename K, typename V, typename HASH>
    IndexCache<K,V,HASH>::IndexCache(uint32_t hashPower):hashPower_(hashPower),
    maxListLength_(BUCKET_SIZE),
    currentNum_(0),
    terminated_(false),
    //bucketMutexes_(256),
    smphExpand(0),
    expanding_(false),
    expandBucket_(0)
    {
        cache_[0] = (node**)calloc(hashsize(hashPower_), sizeof(node*));
        if(cache_[0]==nullptr){
            LOG(ERR,"%s:hashdict created failed", __FUNCTION__);
            return;
        }
        cache_[1]=nullptr;
        threadExpand_ = std::thread{&IndexCache<K,V,HASH>::maintainExpandThread, this};
        
        LOG(INFO,"%s:created IndexCache,capacity:%llu, total size:%llu", __FUNCTION__, hashsize(hashPower_),sizeof(node*)*hashsize(hashPower_));   
    }
    template <typename K, typename V, typename HASH>
    uint32_t IndexCache<K,V,HASH>::getBucketIndex(const K key) const {
        return HASH{}(key)&hashmask(hashPower_);
    }

    template <typename K, typename V, typename HASH>
    bool IndexCache<K,V,HASH>::findAndInsert(const K key, const V& val){
        
        uint32_t index;
        {
            std::unique_lock<std::shared_mutex> lock(bucketMutexe_);
            cache pcache;
            if(expanding_ && (index = HASH{}(key)&hashmask(hashPower_-1))<expandBucket_)
            {
                index=getBucketIndex(key);
                pcache=cache_[1];
            }
            else{
                if(!expanding_) index=getBucketIndex(key);
                pcache=cache_[0];
            }
            node** bucket=&(pcache[index]);
            node* curNode=*bucket;
            uint32_t count=0;
            while(curNode){
                count++;
                if(curNode->key_==key){
                    curNode->val_=val;
                    return true;
                }
                curNode=curNode->next_;
            }
            if(count>maxListLength_) {
                LOG(INFO,"%s:Warning: the count[%u] is greater than %d", __FUNCTION__, count, BUCKET_SIZE);
            }

            //did not find same node, insert the new.
            if(!expanding_ && (currentNum_+1 >= hashsize(hashPower_)*maxListLength_*0.8)){
                //TODO: extend
                LOG(INFO,"%s::too many nodes, need to extend", __FUNCTION__);
                expend();
            }
            if(*bucket==nullptr){
                *bucket=new node(key,val,nullptr);
            }
            else{
                curNode=*bucket;
                node* n=curNode->next_;
                curNode->next_=new node(key,val,n);
            }
            currentNum_++;
            
        }
        return true;
    }

    template <typename K, typename V, typename HASH>
    int IndexCache<K,V,HASH>::find(const K key, V& value) {
        uint32_t index;
        {
            std::shared_lock<std::shared_mutex> lock(bucketMutexe_);
            cache pcache;
            if(expanding_ && (index = HASH{}(key)&hashmask(hashPower_-1))<expandBucket_)
            {
                index=getBucketIndex(key);
                pcache=cache_[1];
            }
            else{
                if(!expanding_) index=getBucketIndex(key);
                pcache=cache_[0];
            }
            
            node* bucket = pcache[index];
            node* curNode=bucket;
            while(curNode){
                if(curNode->key_==key){
                    value=curNode->val_;
                    return 0;
                }
                curNode=curNode->next_;
            }
        }
        
        LOG(TRACE,"%s:did not find the key", __FUNCTION__);
        return -1;
    }

    template <typename K, typename V, typename HASH>
    int IndexCache<K,V,HASH>::expend(){
        //unique_lock<shared_mutex> lock(bucketMutexe_);

        cache_[1] = (node**)calloc(hashsize(hashPower_+1), sizeof(node*));
        if(cache_[1]==nullptr){
            LOG(ERR,"%s:expending IndexCache failed", __FUNCTION__);
            return -1;
        }

        hashPower_++;
        expanding_=true;
        expandBucket_=0;
        
        LOG(INFO,"%s::expended IndexCache is starting", __FUNCTION__);
        //wake expand thread
        smphExpand.release();
        LOG(TRACE,"%s:wake Expand Thread", __FUNCTION__);
        return 0;
    }

    template <typename K, typename V, typename HASH>
    void IndexCache<K,V,HASH>::maintainExpandThread(){
        LOG(INFO,"%s:start", __FUNCTION__);
        while(!terminated_){
            if(expanding_){
                for(uint32_t index=0;index<hashsize(hashPower_-1) && !terminated_;index++){
                    std::unique_lock<std::shared_mutex> lock(bucketMutexe_);
                    node* pslot = cache_[0][index];
                    cache_[0][index]=nullptr;
                    int count=0;
                    while(pslot){
                        count++;
                        K k=pslot->key_;
                        node* n=pslot->next_;
                        uint32_t newIndex=getBucketIndex(pslot->key_);
                        pslot->next_=cache_[1][newIndex];
                        cache_[1][newIndex]=pslot;
                        pslot=n;
                    }
                    expandBucket_=index;
                }
                expanding_=false;
                CheckFree((void**)&(cache_[0]));
                cache_[0]=cache_[1];
                cache_[1]=nullptr;
                LOG(INFO,"%s::IndexCache is expanded successfully", __FUNCTION__);
            }

            LOG(TRACE,"%s:ExpandThread is waitting to be awakened", __FUNCTION__);
            //smphExpand.try_acquire_for(std::chrono::milliseconds(1s));
            smphExpand.acquire();
        }

        LOG(INFO,"%s:end", __FUNCTION__);
    }
}

void testInsert(cache::IndexCache<uint64_t,uint64_t,myHash> *indexCache, uint64_t total, uint32_t* pdata){
    for(uint64_t i=0;i<total;i++){
        uint64_t fp=pdata[i];
        indexCache->put(fp,fp);
    }
}
void testIndexCacheWithMultiThreads(uint32_t capBit, uint32_t NodeBit, uint32_t threadNum, uint32_t partNum,uint32_t mutexNum){
    LOG(INFO,"%s:is using", __FUNCTION__);

    uint64_t max=1UL<<NodeBit;
    auto timeStamp1=std::chrono::high_resolution_clock::now();
    cache::IndexCache<uint64_t,uint64_t,myHash> myIndexCache(capBit);
    //sleep(1);

    auto timeStamp2=std::chrono::high_resolution_clock::now();
#if 1
    std::vector<std::thread> threads(threadNum);
    for(uint32_t i=0;i<threadNum;i++){
        threads[i]=std::thread{testInsert,&myIndexCache,max/threadNum, random_data.getPData(i)};
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
}