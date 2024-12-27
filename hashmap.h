#ifndef HASHMAP_H
#define HASHMAP_H

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
using namespace std;
typedef unsigned int uint32;
typedef unsigned long long uint64;
typedef uint64 fingerPrint;

class RandomData{
private:
    vector<uint32_t*> m_data;
    uint64 m_loopCount;
    uint64 m_count;
public:
    RandomData(){
        m_count=0;
    }
    ~RandomData(){
        for(uint32_t i=0;i<m_loopCount;i++){
            free(m_data[i]);
            m_data[i]=nullptr;
        }
    }
    bool init(uint32_t loopCount,uint64 count);
    uint32_t* getPData(uint32 index);
};

extern RandomData random_data;

void testUnorderedMap(uint32 NodeBit, uint32 threadNum);

void testCustomHashMapWithMultiThreads(uint32 capBit, uint32 NodeBit, uint32 threadNum, uint32 mutexNum);

void testCustomHashMap1WithMultiThreads(uint32 capBit, uint32 NodeBit, uint32 threadNum, uint32 mutexNum);

void testCustomHashMapGroupWithMultiThreads(uint32 capBit, uint32 NodeBit, uint32 threadNum, uint32 partNum, uint32 mutexNum);

struct myHash{
    uint32 operator()(const fingerPrint &fp) const {
        return (uint32)(fp&0xFFFFFFFF);
    }
};

struct indexNode{
    uint64 dcid;
    indexNode(uint64 dcid_):dcid(dcid_){}
};

#endif