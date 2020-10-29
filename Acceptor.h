#pragma once

#include"Socket.h"
#include"InetAddress.h"
#include<boost/noncopyable.hpp>

namespace TcpServer
{

class Acceptor:public boost::noncopyable
    {
    public:
        Acceptor(const std::string& ip,unsigned short port);
        Acceptor(unsigned short port);
        void ready();
        int accept();
        int fd();
        void setReuseAddr(bool on);
        void setReusePort(bool on);
        void bind();
        void listen();
        
    private:
        InetAddress addr_;
        Socket listenSocket_;
    };
}
