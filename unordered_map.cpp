#include "hashmap.h"

shared_mutex g_mutex;

void testInsert(unordered_map<fingerPrint,indexNode,myHash> *myHashMap,uint64 total, uint32_t* pdata){
    for(uint64 i=0;i<total;i++){
        uint64 fp=pdata[i];
        unique_lock<shared_mutex> lock(g_mutex);
        myHashMap->insert({fp,indexNode(12345678)});
    }
}

void testUnorderedMap(uint32 NodeBit, uint32 threadNum){
    cout<<"using:"<<__FUNCTION__<<endl;

    uint64 max=1UL<<NodeBit;
    auto timeStamp1=chrono::high_resolution_clock::now();

    unordered_map<fingerPrint,indexNode,myHash> myHashMap;

    auto timeStamp2=chrono::high_resolution_clock::now();

    vector<thread> threads(threadNum);
    for(uint32 i=0;i<threadNum;i++){
        threads[i]=thread(testInsert,&myHashMap,max/threadNum,random_data.getPData(i));
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
    myHashMap.insert({1234,indexNode(12345678)});
    auto it = myHashMap.find(1234);
    assert(it!=myHashMap.end() && it->second.dcid==12345678);

    //not accurate
    cout<<"myHashMap size:"<<sizeof(myHashMap)<<endl;
    cout<<"Mem:"<<myHashMap.size()<<"*"<<sizeof(indexNode)<<" not accurate!"<<endl;
    
}