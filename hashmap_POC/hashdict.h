#ifndef HASHDICT_H
#define HASHDICT_H

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
#include <queue>
#include <unordered_set>
#include <condition_variable>
#include "logging.h"
#include "messageQueue.h"
#include "sstream"
#include <new>
//using namespace std;

#define hashsize(n) ((uint64_t)1<<(n))
#define hashmask(n) (hashsize(n)-1)

#define QUEUE_CAPACITY 1
#define MAX_REQUEST_THREAD 8

enum io_request_type{
    SET=0,
    GET=1,
    DEL=2,
    TERMINATE=4
};

enum io_response_type{
    SUCCESSFUL=0,
    FAILED=1
};

template <typename K, typename V, typename HASH=std::hash<K>>
class hashdict {
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

    struct req_item {
        K key_;
        V val_;
        req_item(){}

        req_item(K key, V val):key_(key),
        val_(val)
        {}
    };

    struct io_response{
        uint32_t id_;
        req_item item_;
        io_response_type type_;
        io_response(){
            type_=FAILED;
        }
        io_response(uint32_t id, req_item item, io_response_type type):id_(id),
        item_(item),
        type_(type){}
    };

    struct io_request{
        uint32_t id_;
        req_item item_;
        uint32_t notify_index_;
        io_request_type type_;

        io_request(io_request_type type){
            type_=type;
        }

        io_request(uint32_t id, req_item item, uint32_t notify_index, io_request_type type):id_(id),
        item_(item),
        notify_index_(notify_index),
        type_(type){}
        
    };
    
private:
    uint32_t m_hashPower;
    uint32_t m_maxListLength;
    uint64_t m_currentNum;
    //io_queue io_queue_;
    MessageQue<io_request> io_queue_;
    volatile bool terminated_;
    std::vector<std::thread> t_;
    //uint32_t m_bucketMutexNum;
    //mutable vector<shared_mutex> m_bucketMutexes;
    node **m_cache;

    Notify<io_response> *pnotify_;
    
    uint32_t getBucketIndex(const K key) const {
        return HASH{}(key)&hashmask(m_hashPower);
    }
    /*
    uint32 getBucketMutexIndex(const K key) const{
        assert(m_bucketMutexes.size()!=0);
        return HASH{}(key)%m_bucketMutexes.size();
    }*/
    int expend(){
        return 0;
    }
    int put(const K key, const V val);
    int get(const K key, V& value);
    void handleItem(io_request* item);
    void threadfunc();
public:
    hashdict(uint32_t hashPower=2);
    ~hashdict(){
        stopService();
    };

    void startService(){
        terminated_=false;
    }
    void stopService(){
        terminated_=true;
        for(size_t i=0;i<t_.size();i++){
            io_request ioItem(TERMINATE);
            requestWithoutReply(ioItem);
        }
        for(size_t i=0;i<t_.size();i++){
            t_[i].join();
            std::cout<<__FUNCTION__<<"thread "<<i<<"is stopped"<<std::endl;
            LOG(INFO,"%s:thread:%s is stopped", __FUNCTION__, i);
        }
        LOG(INFO,"%s:end", __FUNCTION__);
    }
    
    uint32_t createNotify(std::string name){
        LOG(INFO,"%s:%s", __FUNCTION__, name.c_str());
        //new (&pnotify_[0]) Notify(name);
        return 0;
    }
    void destoryNotify(uint32_t notify_index){
        LOG(INFO,"%s:notify index::%u", __FUNCTION__, notify_index);
    }
    io_response_type request(io_request& ioItem){
        io_queue_.send(ioItem);
        io_response* res=pnotify_[ioItem.notify_index_].get();
        return res->type_;
    }
    void requestWithoutReply(io_request& ioItem){
        io_queue_.send(ioItem);
    }
};
#endif