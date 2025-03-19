#pragma once

#include <iostream>
#include <mutex>
#include <memory>
#include <queue>
#include <condition_variable>
#include "logging.h"
#include <chrono>
#include "SPSCQueue.h"
#include <atomic>
#include <semaphore>

template<typename T>
class Notify{
public:
    //Notify();
    Notify(std::string name):name_(name){std::cout<<"Notify construct:"<<name<<std::endl;}
    Notify(const Notify& n)=delete;
    void set_value(T && v){
        LOG(TRACE,"%s:set_value:%u", name_.c_str(), v.id_);

        new (&value_) T(std::move(v)); 
        content_.store(&value_);
        content_.notify_one();
    }
    T* get(){
        content_.wait(nullptr);
        T* res = content_.load();

        LOG(TRACE,"%s:get:%u", name_.c_str(), res->id_);
        content_.store(nullptr);
        return res;
    }
private:
    std::string name_;
    T value_;
    std::atomic<T*> content_;
};

#if 0
template<typename MSG>
class MPSCQueue{
private:
    enum state_type{
        null=1,
        hasContent=2
    };
public:
    explicit MPSCQueue(std::string name, size_t capacity):name_(name),
    state_(null),
    capacity_(capacity)
    {
        if (capacity_ < 1) {
            capacity_ = 1;
        }
        if (capacity_ > SIZE_MAX - 2 * kPadding) {
            capacity_ = SIZE_MAX - 2 * kPadding;
        }
        size_t count = queueCapacity + 2 * this->kPadding;
        slots_ = new MSG[count];
        if(slots_==nullptr){
            LOG(ERR,"%s:get response failed id:%u", __FUNCTION__,id);
        }
    } 
     
    void emplace(Args &&...args) noexcept {
        std::is_nothrow_constructible<MSG, Args &&...>::value) {
            static_assert(std::is_constructible<MSG, Args &&...>::value,"MSG must be constructible with Args&&...");
            
            auto const writeIdx = writeIdx_.load(std::memory_order_relaxed);
            new (&slots_[writeIdx + kPadding]) T(std::forward<Args>(args)...);
            writeIdx_.store(nextWriteIdx, std::memory_order_release);
        }
    }
    MSG* front(){
        
    }
    size_t size(){
        std::unique_lock<std::mutex> lock(req_mutex_);
        return que_req_.size();
    }
    std::string getName(){
        return name_;
    }
private:
    #ifdef __cpp_lib_hardware_interference_size
    static constexpr size_t kCacheLineSize =
        std::hardware_destructive_interference_size;
    #else
    static constexpr size_t kCacheLineSize = 64;
    #endif

    // Padding to avoid false sharing between slots_ and adjacent allocations
    static constexpr size_t kPadding = (kCacheLineSize - 1) / sizeof(MSG) + 1;
private:
    std::string name_;
    state_type state_;
    size_t capacity_;
    MSG* slots_;

    alignas(kCacheLineSize) std::atomic<size_t> writeIdx_ = {0};
    alignas(kCacheLineSize) std::atomic<size_t> readIdx_ = {0};

};
#endif

template<typename REQUEST>
class MessageQue{
public:
    explicit MessageQue(std::string name, uint32_t capacity):name_(name),
    que_req_(capacity){
    } 
    // non-copyable and non-movable
    MessageQue(const MessageQue &) = delete;
    MessageQue &operator=(const MessageQue &) = delete;
     
    void send(const REQUEST& req){
        que_req_.emplace(req);
        LOG(TRACE,"%s:send:%u", name_.c_str(), req.id_);
    }
    REQUEST* receive(){
        LOG(TRACE,"%s:wait for receive", name_.c_str());
        while(!que_req_.front())
            ;

        REQUEST* req=que_req_.front();
        que_req_.pop();
        
        LOG(TRACE,"%s:receive:%u", name_.c_str(),req->id_);
        return req;
    }
    std::string getName(){
        return name_;
    }
private:
    std::string name_;
    rigtorp::SPSCQueue<REQUEST> que_req_;
};