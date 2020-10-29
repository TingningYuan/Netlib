#pragma once
#include"MutexLock.h"
#include<sys/epoll.h>
#include<map>
#include<vector>
#include<memory>
#include<functional>
#include<boost/noncopyable.hpp>

using namespace std;

namespace TcpServer
{
class Acceptor;
class TcpConnection;
using TcpConnectionPtr=shared_ptr<TcpConnection>;
using TcpConnectionCallback=function<void(const TcpConnectionPtr& conn)>;
using Functor=function<void()>;

class EventLoop:public boost::noncopyable
    {
    public:
        explicit EventLoop(Acceptor& acceptor);
        ~EventLoop();

        void loop();
        void unloop();
        void runInLoop(Functor&& cb);

        void setConnectionCallback(TcpConnectionCallback&& cb)
        {
            onConnection_=std::move(cb);
        }
        void setMessageCallback(TcpConnectionCallback&& cb)
        {
            onMessage_=std::move(cb);
        }
        void setCloseCallback(TcpConnectionCallback&& cb)
        {
            onClose_=std::move(cb);
        }

    private:
        void waitEpollFd();
        void handleNewConnection();
        void handleMessage(int fd);

        int createEpollFd();
        void addEpollReadFd(int fd);
        void delEpollReadFd(int fd);
        bool isConnectionClosed();
        int createEventFd();

        void handleRead();
        void wakeup();
        void doPendingFunctors();
    private:
        int epollfd_;
        int eventfd_;
        Acceptor& acceptor_;
        bool isLooping_;

        map<int,TcpConnection> connMap_;
        vector<struct epoll_event> epoll_eventList_;
        vector<Functor> pendingFunctors_;
        MutexLock mutex_;
        
        TcpConnectionCallback onConnection_;
        TcpConnectionCallback onMessage_;
        TcpConnectionCallback onClose_;

    };
}
