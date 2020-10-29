#include"TcpConnection.h"
#include"Acceptor.h"
#include"EventLoop.h"
#include"Thread.h"

#include<unistd.h>
#include<stdio.h>
#include<assert.h>
#include<sys/eventfd.h>
#include<iostream>

using std::cout;
using std::endl;

namespace TcpServer
{

EventLoop::EventLoop(Acceptor& acceptor)
    :epollfd_(createEpollFd()),
    eventfd_(createEventFd()),
    acceptor_(acceptor),
    isLooping_(false),
    epoll_eventList_(1024)
    {
        addEpollReadFd(acceptor_.fd());
        addEpollReadFd(eventfd_);
        cout<<"epoll_eventList_:"<<epoll_eventList_.size()<<endl;
        cout<<"acceptor_.fd:"<<acceptor_.fd()<<endl;
    }

EventLoop::~EventLoop()
{
    ::close(epollfd_);
}

//IO线程执行loop
void EventLoop::loop()
{
    if(!isLooping_){
        isLooping_=true;
        while(isLooping_){
            waitEpollFd();
        }
    }
}

void EventLoop::unloop()
{
    if(isLooping_){
        isLooping_=false;
    }
}

void EventLoop::runInLoop(Functor&& cb)
{
    {
        MutexLockGuard lock(mutex_);
        pendingFunctors_.push_back(std::move(cb));
    }
    wakeup();
}

void EventLoop::waitEpollFd()
{
    int nready=0;
    do{
        nready=::epoll_wait(epollfd_,&*epoll_eventList_.begin(),epoll_eventList_.size(),5000);
        
    }while(nready==-1&&errno==EINTR);
    
    if(nready==-1){
        return;
    }else if(nready==0){
        perror("epoll_wait");
    }else{
        if(nready==epoll_eventList_.size()){
            epoll_eventList_.resize(2*nready);
        }
        for(int idx=0;idx!=nready;++idx){
            int fd=epoll_eventList_[idx].data.fd;
            if(fd==acceptor_.fd()){
                if(epoll_eventList_[idx].events & EPOLLIN){
                    //针对新连接的处理
                    handleNewConnection();
                }
            }else if(fd==eventfd_){
                //计算线程通知IO线程处理后续事宜
                handleRead();
                doPendingFunctors();
            }else if(epoll_eventList_[idx].events & EPOLLIN){
                //消息到达时的处理
                handleMessage(fd);
            }
        }
    }
}

void EventLoop::handleNewConnection()
{
    int peerfd=acceptor_.accept();
    addEpollReadFd(peerfd);
    TcpConnectionPtr conn(new TcpConnection(peerfd,this));

    conn->setConnectionCallback(onConnection_);
    conn->setMessageCallback(onMessage_);
    conn->setCloseCallback(onClose_);
    connMap_.emplace(make_pair(peerfd,conn));
    conn->setNonblock();
    conn->handleConnectionCallback();
}

void EventLoop::handleMessage(int fd)
{
    auto iter=connMap_.find(fd);
    assert(iter!=connMap_.end());

    //判断连接是否断开
    if(iter->second.isClosed()){
        cout<<fd<<":handle close"<<endl;
        iter->second.handleCloseCallback();
        delEpollReadFd(fd);
        connMap_.erase(iter);
    }else{
        cout<<fd<<":handle message"<<endl;
        iter->second.handleMessageCallback();
    }
}

int EventLoop::createEpollFd()
{
    int fd=::epoll_create1(0);
    assert(fd!=-1);
    return fd;
}

void EventLoop::addEpollReadFd(int fd)
{
    struct epoll_event value;
    value.data.fd=fd;
    value.events=EPOLLIN;
    int ret=::epoll_ctl(epollfd_,EPOLL_CTL_ADD,fd,&value);
    assert(ret==0);
}

void EventLoop::delEpollReadFd(int fd)
{
    struct epoll_event value;
    value.data.fd=fd;
    int ret=::epoll_ctl(epollfd_,EPOLL_CTL_DEL,fd,&value);
    assert(ret==0);
}

int EventLoop::createEventFd()
{
    int fd=::eventfd(0,0);
    assert(fd!=-1);
    return fd;
}

void EventLoop::handleRead()
{
    uint64_t howmany=-1;
    int ret=::read(eventfd_,&howmany,sizeof(howmany));
    assert(read!=sizeof(howmany));
}

void EventLoop::wakeup()
{
    uint64_t one=1;
    int ret=::write(eventfd_,&one,sizeof(one));
    assert(ret!=sizeof(one));
}

void EventLoop::doPendingFunctors()
{
    vector<Functor> tmp;
    {
        MutexLockGuard lock(mutex_);
        tmp.swap(pendingFunctors_);
    }
    for(auto& functor:tmp){
        functor();
    }
}

}
