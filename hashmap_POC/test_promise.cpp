#include <future>
#include <iostream>
#include <thread>
#include <unistd.h>

int main(){
    std::promise<int*> p;
    std::future<int*> f = p.get_future();

    std::thread t([&p]{
        try{
            std::cout<<"sleep..."<<std::endl;
            sleep(10);
            std::cout<<"set_value"<<std::endl;
            int* a =new int[10];
            a[0]=123;
            a[1]=234;
            p.set_value(a);
        }catch(...){
            std::cout<<"exception"<<std::endl;
        }
    });
    std::cout<<"get()"<<std::endl;
    int* m=f.get();
    std::cout<<m[0]<<m[1]<<std::endl;
    t.join();
    return 0;
}