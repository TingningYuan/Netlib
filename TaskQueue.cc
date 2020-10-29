#include"TaskQueue.h"

namespace TcpServer
{

TaskQueue::TaskQueue(size_t queSize)
    :queSize_(queSize),
    mutex_(),
    notFull_(mutex_),
    notEmpty_(mutex_),
    flag_(true)
    {}

bool TaskQueue::empty() const
{
    return (que_.size()==0);
}

bool TaskQueue::full() const
{
    return (que_.size()==queSize_);
}

size_t TaskQueue::size() const
{
    return que_.size();
}

//运行于生产者线程
void TaskQueue::push(ElemType elem)
{
    MutexLockGuard lock(mutex_);
    while(full()){
        notFull_.wait();
    }
    que_.push(elem);
    notEmpty_.notify();
}

//运行在消费者线程
ElemType TaskQueue::pop()
{
    MutexLockGuard lock(mutex_);
    while(flag_&&empty()){
        notEmpty_.wait();
    }
    if(flag_){
        ElemType elem=que_.front();
        que_.pop();
        notFull_.notify();
        return elem;
    }else{
        return nullptr;
    }
    return nullptr;
}

void TaskQueue::wakeup()
{
    flag_=false;
    notEmpty_.notifyAll();
}

}
