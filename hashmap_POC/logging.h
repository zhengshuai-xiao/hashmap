#pragma once
#include <stdio.h>
#include <iostream>
#include <vector>
#include <mutex>
#include <fstream>
#include <sstream>
#include <format>

#define LOGFILE "/tmp/hashlog.log"

#define LOG(type, format, ...) Logger::getInstance().log(type, format, __VA_ARGS__)

extern bool debug_flag;
enum log_type{
    ERR=0,
    WARN=1,
    INFO=2,
    TRACE=3
};

static std::string logTypeStr[4]={"ERR","WARN","INFO","TRACE"};

class Logger{
public:
    static Logger& getInstance(){
        static Logger Logger;
        return Logger;
    }
    Logger(){
        filename_=LOGFILE;
        file_.open(LOGFILE);
        fLimit_=10;
    }
    Logger(std::string filename,uint32_t fLimit=10):filename_(filename),
    msgCache_(fLimit),
    fLimit_(fLimit){
    }
    ~Logger(){
        if(msgCache_.size()>0){
            flush();
        }
        msgCache_.clear();
        if(file_.is_open()) file_.close();
    }
    template<typename... Args>
    void log(log_type type , const std::string& format,Args... args){
        if(!debug_flag && type==TRACE) return;
        char str[512]={0};
        std::lock_guard<std::mutex> lock(mt_);
        sprintf(str, format.c_str(), args...);
        msgCache_.push_back(logTypeStr[type]+","+str);
        if(msgCache_.size()>=fLimit_){
            flush();
        }
    }

    void flush(){
        for(const auto &msg : msgCache_){
            file_<<msg<<std::endl;
        }
        msgCache_.clear();
    }
private:
    std::string filename_;
    std::ofstream file_;
    std::vector<std::string> msgCache_;
    std::mutex mt_;
    uint32_t fLimit_;
};
