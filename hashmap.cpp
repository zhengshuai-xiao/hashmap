#include "hashmap.h"

    bool RandomData::init(uint32_t loopCount,uint64 count){
        
        random_device rd;
        mt19937_64 gen(rd());
        uniform_int_distribution<uint64> dis(0, UINT64_MAX);
        
        auto timeStamp1=chrono::high_resolution_clock::now();

        m_loopCount=loopCount;
        m_count=count;
        uint64 total=m_count/m_loopCount;
        cout<<"starting to generate "<<m_loopCount<<"*"<<total<<endl;

        for(uint32_t i=0;i<m_loopCount;i++){
            uint32_t* pdata=(uint32_t*)calloc(total,sizeof(uint32_t));
            if(!pdata){
                cerr<<__FUNCTION__<<":OOM"<<endl;
                return false;
            }

            for(uint64 j=0;j<total;j++){
                pdata[j]=dis(gen);
            }

            m_data.emplace_back(pdata);
        }
        auto timeStamp2=chrono::high_resolution_clock::now();
        chrono::duration<double> d=timeStamp2-timeStamp1;
        cout<<"data has been initilized, duration:"<<d.count()<<endl;
        return true;
    }
    uint32_t* RandomData::getPData(uint32 index){
        if(index>=m_loopCount){
            cerr<<"the index["<<index<<"] is over than capacity["<<m_loopCount<<"]"<<endl;
            return nullptr;
        }
        return m_data[index];
    }

RandomData random_data;

void usage(){
    cout<<"-Usage:"<<endl;
    cout<<"-me method: 0:unordered_map; \n\
    1: std::vector + std::list + shared_mutex pool; \n\
    2: array + list + shared_mutex pool\n\
    3: partition + array + list + shared_mutex pool"<<endl;
    cout<<"-t thread: uses how many threads to insert data"<<endl;
    cout<<"-cb capBit: 1~32"<<endl;
    cout<<"-nb NodeBit: 1~32"<<endl;
    cout<<"-p part: default:64"<<endl;
    cout<<"-mu mutex num: default:64"<<endl;
    cout<<"-s sleep default:100s"<<endl;
}

int main(int argc, char* argv[]){
    uint32 method=0;//0:unordered_map; 1:customHashMap
    uint32 capBit=16;
    uint32 NodeBit=18;
    uint32 threadNum=1;
    uint32 partitionNum=1;
    uint32 mutexNum=1;
    uint32 sleepTime=0;
    cout<< argc<<endl;
    if(argc==5){
        method = atoi(argv[1]);
        capBit=atoi(argv[2]);
        NodeBit=atoi(argv[3]);
        threadNum=atoi(argv[4]);
    }
    
    for(int i=1;i<argc;i++){
        string arg=argv[i];
        if(arg=="-me"){
            method = atoi(argv[++i]);
        }else if(arg=="-t"){
            threadNum=atoi(argv[++i]);
        }else if(arg=="-cb"){
            capBit=atoi(argv[++i]);
        }else if(arg=="-nb"){
            NodeBit=atoi(argv[++i]);
        }else if(arg=="-p"){
            partitionNum=atoi(argv[++i]);
        }else if(arg=="-mu"){
            mutexNum=atoi(argv[++i]);
        }else if(arg=="-s"){
            sleepTime=atoi(argv[++i]);
        }else{
            cerr<<"unknown option"<<endl;
            usage();
            return -1;
        }
    }
    cout<<"method="<<method<<endl;
    cout<<"capBit="<<capBit<<endl;
    cout<<"NodeBit="<<NodeBit<<endl;
    cout<<"threadNum="<<threadNum<<endl;
    cout<<"partitionNum="<<partitionNum<<endl;
    cout<<"mutexNum="<<mutexNum<<endl;
    
    if(!random_data.init(threadNum,1UL<<NodeBit)){
        return -1;
    }

    switch(method){
    case 0:
        testUnorderedMap(NodeBit,threadNum);
        break;
    case 1:
        testCustomHashMapWithMultiThreads(capBit,NodeBit,threadNum, mutexNum);
        break;
    case 2:
        testCustomHashMap1WithMultiThreads(capBit,NodeBit,threadNum, mutexNum);
        break;
    case 3:
        testCustomHashMapGroupWithMultiThreads(capBit,NodeBit,threadNum, partitionNum, mutexNum);
        break;
    default:
        cerr<<"invalid method:"<<method<<endl;
        break;
    }

    sleep(sleepTime);
    return 0;
}