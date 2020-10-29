#pragma once

#include<boost/noncopyable.hpp>
#include<pthread.h>
#include<string>
#include<functional>
using namespace std;

namespace TcpServer
{

namespace current_thread
{
extern __thread const char* name;
}

using ThreadCallback=std::function<void()>;

class Thread:public boost::noncopyable
{
public:
    Thread(ThreadCallback&& cb,const string& name=string());
    ~Thread();

    void start();
    void join();

    bool isRunning() const
    {
        return isRunning_;
    }
    string getName() const
    {
        return name_;
    }
private:
    static void* threadFunc(void* arg);
private:
    pthread_t pthid_;
    string name_;
    bool isRunning_;
    ThreadCallback cb_;
};

}
