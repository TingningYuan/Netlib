#pragma once

#include"MutexLock.h"
#include"Condition.h"
#include"Task.h"

#include<queue>

using namespace std;

namespace TcpServer
{

using ElemType=Task;

class TaskQueue
{
public:
    TaskQueue(size_t queSize);

    bool empty() const;
    bool full() const;
    size_t size() const;
    void push(ElemType elem);
    ElemType pop();

    void wakeup();

private:
    size_t queSize_;
    queue<ElemType> que_;
    MutexLock mutex_;
    Condition notFull_;
    Condition notEmpty_;
    bool flag_;
};


}
