#pragma once

#include"Task.h"
#include"TaskQueue.h"
#include<vector>
#include<memory>

namespace TcpServer
{

class Thread;
class Threadpool
{
    friend class WokerThread;
public:
    Threadpool(size_t threadNum,size_t queSize);
    ~Threadpool();

    void start();
    void stop();
    void wakeup();
    void addTask(Task&& task);
private:
    Task getTask();
    void threadFunc();

    size_t threadNum_;
    size_t queSize_;
    vector<unique_ptr<Thread>> threadList_;
    TaskQueue taskQue_;
    bool isExit_;
};


}
