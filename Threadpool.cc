#include"Threadpool.h"
#include"Thread.h"
#include<unistd.h>
#include<iostream>

namespace TcpServer
{

Threadpool::Threadpool(size_t threadNum,size_t queSize)
    :threadNum_(threadNum),queSize_(queSize),threadList_(),taskQue_(queSize_),isExit_(false)
{
    threadList_.reserve(threadNum_);
}

Threadpool::~Threadpool()
{
    if(!isExit_){
        stop();
    }
    cout<<"~Threadpool()"<<endl;
}

void Threadpool::start()
{
    for(size_t idx=0;idx!=threadNum_;++idx){
        unique_ptr<Thread> threadptr(new Thread(bind(&Threadpool::threadFunc,this),to_string(idx)));
        threadList_.push_back(std::move(threadptr));
    }
    for(auto& thread:threadList_){
        thread->start();
    }
}

void Threadpool::stop()
{
    if(!isExit_){
        while(!taskQue_.empty()){
            ::usleep(1);
        }
        isExit_=true;
        wakeup();
        for(auto& thread:threadList_){
            thread->join();
        }
    }
}

void Threadpool::addTask(Task&& task)
{
    taskQue_.push(task);
}

void Threadpool::wakeup()
{
    taskQue_.wakeup();
}

Task Threadpool::getTask()
{
    return taskQue_.pop();
}

void Threadpool::threadFunc()
{
    while(!isExit_){
        Task task=getTask();
        if(task){
            task();
        }
    }
}
}
