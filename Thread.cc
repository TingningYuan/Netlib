#include"Thread.h"

namespace TcpServer
{

namespace current_thread
{
__thread const char* name="TcpServer thread";
}

struct ThreadData
{
    ThreadData(const string& name,const ThreadCallback& cb):name_(name),cb_(cb)
    {}

    void runInThread()
    {
        current_thread::name=((name_==string())?"TcpServer":name_.c_str());
        if(cb_){
            cb_();
        }
    }
    string name_;
    ThreadCallback cb_;
};

Thread::Thread(ThreadCallback&& cb,const string& name)
    :pthid_(0),name_(name),isRunning_(false),cb_(std::move(cb))
{}

Thread::~Thread()
{
    if(isRunning_){
        pthread_detach(pthid_);
    }
}

void Thread::start()
{
    ThreadData* pdata=new ThreadData(name_,cb_);
    pthread_create(&pthid_,nullptr,threadFunc,pdata);
    isRunning_=true;
}

void Thread::join()
{
    if(isRunning_){
        pthread_join(pthid_,nullptr);
        isRunning_=false;
    }
}

void* Thread::threadFunc(void* arg)
{
    ThreadData* pdata=static_cast<ThreadData*>(arg);
    if(pdata){
        pdata->runInThread();
    }
    delete pdata;
    return nullptr;
}
}
