#include "randomData.h"

RandomData random_data;

bool RandomData::init(uint32_t loopCount,uint64_t count){
        
        random_device rd;
        mt19937_64 gen(rd());
        uniform_int_distribution<uint64_t> dis(0, UINT64_MAX);
        
        auto timeStamp1=chrono::high_resolution_clock::now();

        m_loopCount=loopCount;
        m_count=count;
        uint64_t total=m_count/m_loopCount;
        cout<<"starting to generate "<<m_loopCount<<"*"<<total<<endl;

        for(uint32_t i=0;i<m_loopCount;i++){
            uint32_t* pdata=(uint32_t*)calloc(total,sizeof(uint32_t));
            if(!pdata){
                cerr<<__FUNCTION__<<":OOM"<<endl;
                return false;
            }

            for(uint64_t j=0;j<total;j++){
                pdata[j]=dis(gen);
            }

            m_data.emplace_back(pdata);
        }
        auto timeStamp2=chrono::high_resolution_clock::now();
        chrono::duration<double> d=timeStamp2-timeStamp1;
        cout<<"data has been initilized, duration:"<<d.count()<<endl;
        return true;
    }
    uint32_t* RandomData::getPData(uint32_t index){
        if(index>=m_loopCount){
            cerr<<"the index["<<index<<"] is over than capacity["<<m_loopCount<<"]"<<endl;
            return nullptr;
        }
        return m_data[index];
    }