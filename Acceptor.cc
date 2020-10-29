#include"Acceptor.h"

namespace  TcpServer
{

Acceptor::Acceptor(unsigned short port):addr_(port),listenSocket_()
    {}

Acceptor::Acceptor(const std::string& ip,unsigned short port):addr_(ip,port),listenSocket_()
    {}

void Acceptor::ready()
{
    setReuseAddr(true);
    setReusePort(true);
    bind();
    listen();
}

//地址复用
void Acceptor::setReuseAddr(bool on)
{
    int one=on;
    setsockopt(listenSocket_.fd(),SOL_SOCKET,SO_REUSEADDR,&one,sizeof(one));
}

//端口复用
void Acceptor::setReusePort(bool on)
{
    int one=on;
    setsockopt(listenSocket_.fd(),SOL_SOCKET,SO_REUSEPORT,&one,sizeof(one));
}

void Acceptor::bind()
{
    int ret=::bind(listenSocket_.fd(),(struct sockaddr*)addr_.getInetAddressPtr(),sizeof(struct sockaddr));
}

void Acceptor::listen()
{
    int ret=::listen(listenSocket_.fd(),100);
}

int Acceptor::accept()
{
    int peerfd=::accept(listenSocket_.fd(),NULL,NULL);
    return peerfd;
}


}
