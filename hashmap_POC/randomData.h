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
using namespace std;

class RandomData{
private:
    vector<uint32_t*> m_data;
    uint64_t m_loopCount;
    uint64_t m_count;
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
    bool init(uint32_t loopCount,uint64_t count);
    uint32_t* getPData(uint32_t index);
};

extern RandomData random_data;