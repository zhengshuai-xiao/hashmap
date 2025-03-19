#pragma once
#include <iostream>
#include <vector>
#include <list>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <cassert>
#include <stdexcept>
#include <unistd.h>
#include <chrono>
#include <unordered_map>
#include <atomic>
#include <random>
#include <memory>
#include "../POC/logging.h"
#include <semaphore>

namespace cache{
    template <typename K, typename V, typename HASH=std::hash<K>>
    class IndexCache {
    public:
        struct node{
            K key_;
            V val_;
            node* next_;

            node(K key, V val,node* next):key_(key),
            val_(val),
            next_(next)
            {}
        };
        typedef node** cache;
        
    private:
        uint32_t hashPower_;
        uint32_t maxListLength_;
        uint64_t currentNum_;
        volatile bool terminated_;
        std::shared_mutex bucketMutexe_;

        std::thread threadExpand_;
        std::binary_semaphore smphExpand;
        /* this is the flag indicates if cache is in expanding */
        bool expanding_;
        /*which bucket index are we expanding, from left to right*/
        uint32_t expandBucket_;
        /*these two cache are using to store K/V, the second is using in expanding mode
        So during expanding, these two are all using, data migration will be processing
        from the first to the second, expandBucket_ record where we are expanding*/
        cache cache_[2];
        
        uint32_t getBucketIndex(const K key) const;
        
        /*uint32_t getBucketMutexIndex(const K key) const{
            assert(bucketMutexes_.size()!=0);
            return HASH{}(key)%bucketMutexes_.size();
        }*/
        bool findAndInsert(const K key, const V& val);
        int find(const K key, V& value);
        int expend();
        void maintainExpandThread();
    public:
        explicit IndexCache(uint32_t hashPower=2);
        ~IndexCache(){
            stopService();
        };

        void startService(){
            terminated_=false;
        }
        void stopService(){
            terminated_=true;
            smphExpand.release();
            threadExpand_.join();
            LOG(INFO,"%s:end", __FUNCTION__);
        }
        bool put(const K key, const V& val){
            return findAndInsert(key,val);
        }
        int get(const K key, V& value){
            return find(key,value);
        }
    };
}