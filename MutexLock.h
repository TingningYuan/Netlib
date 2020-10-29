#pragma once

#include<boost/noncopyable.hpp>
#include<pthread.h>
#include<assert.h>

namespace TcpServer
{

class MutexLock:public boost::noncopyable
{
public:
    MutexLock():islocking_(false)
    {
        pthread_mutex_init(&mutex_,NULL);
    }
    ~MutexLock()
    {
        assert(islocking_==false);
        pthread_mutex_destroy(&mutex_);
    }
    
    void lock()
    {
        pthread_mutex_lock(&mutex_);
        islocking_=true;
    }

    void unlock()
    {
        islocking_=false;
        pthread_mutex_unlock(&mutex_);
    }

    bool isLocking() const
    {
        return islocking_;
    }

    pthread_mutex_t* getPthreadMutex()
    {
        return &mutex_;
    }
private:
    pthread_mutex_t mutex_;
    bool islocking_;
};

class MutexLockGuard:public MutexLock,public boost::noncopyable
{
public:
    explicit MutexLockGuard(MutexLock& mutex):mutex_(mutex)
    {mutex_.lock();}
    ~MutexLockGuard()
    {mutex_.unlock();}
private:
    MutexLock& mutex_;
};

}
