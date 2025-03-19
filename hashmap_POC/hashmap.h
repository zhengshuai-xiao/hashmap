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

#include "hashdict.h"

using namespace std;
typedef unsigned int uint32;
typedef unsigned long long uint64;
typedef uint64 fingerPrint;


void testUnorderedMap(uint32 NodeBit, uint32 threadNum);

void testCustomHashMapWithMultiThreads(uint32 capBit, uint32 NodeBit, uint32 threadNum, uint32 mutexNum);

void testCustomHashMap1WithMultiThreads(uint32 capBit, uint32 NodeBit, uint32 threadNum, uint32 mutexNum);

void testCustomHashMapGroupWithMultiThreads(uint32 capBit, uint32 NodeBit, uint32 threadNum, uint32 partNum, uint32 mutexNum);

void testCustomHashDictWithMultiThreads(uint32_t capBit, uint32_t NodeBit, uint32_t threadNum, uint32_t partNum,uint32_t mutexNum);

void testIndexCacheWithMultiThreads(uint32_t capBit, uint32_t NodeBit, uint32_t threadNum, uint32_t partNum,uint32_t mutexNum);


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