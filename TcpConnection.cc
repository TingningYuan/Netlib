#include"TcpConnection.h"
#include<errno.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sstream>

namespace TcpServer
{

TcpConnection::TcpConnection(int fd,EventLoop* loop)
    :socket_(fd),socketIO_(fd),
    localAddr_(getLocalAddr(fd)),
    peerAddr_(getPeerAddr(fd)),
    isShutdownWrite_(false),loop_(loop)
    {}

TcpConnection::~TcpConnection()
{
    if(!isShutdownWrite_){
        shutdown();
    }
}

std::string TcpConnection::receive()
{
    char buf[65536]={0};
    socketIO_.readLine(buf,sizeof(buf));
    return std::string(buf);
}

void TcpConnection::send(const std::string& msg)
{
    socketIO_.writen(msg.c_str(),msg.size());
}

void TcpConnection::sendInLoop(const std::string& msg)
{
    if(loop_){
        loop_->runInLoop(std::bind(&TcpConnection::send,this,msg));
    }
}

void TcpConnection::shutdown()
{
    if(!isShutdownWrite_){
        isShutdownWrite_=true;
        socket_.shutdownWrite();
    }
}

std::string TcpConnection::toString() const
{
    std::ostringstream oss;
    oss<<localAddr_.ip()<<":"<<localAddr_.port()<<"-->"<<peerAddr_.ip()<<":"<<peerAddr_.port();
    return oss.str();
}

InetAddress TcpConnection::getLocalAddr(int fd)
{
    struct sockaddr_in addr;
    socklen_t len=sizeof(struct sockaddr);
    getsockname(socket_.fd(),(struct sockaddr*)&addr,&len);
    return InetAddress(addr);
}

InetAddress TcpConnection::getPeerAddr(int fd)
{
    struct sockaddr_in addr;
    socklen_t len=sizeof(struct sockaddr);
    getpeername(socket_.fd(),(struct sockaddr*)&addr,&len);
    return InetAddress(addr);
}

void TcpConnection::handleConnectionCallback()
{
    if(onConnection_){
        onConnection_(shared_from_this());
    }
}

void TcpConnection::handleMessageCallback()
{
    if(onMessage_){
        onMessage_(shared_from_this());
    }
}

void TcpConnection::handleCloseCallback()
{
    if(onClose_){
        onClose_(shared_from_this());
    }
}

bool TcpConnection::isClosed()
{
    int nready=-1;
    char buf[1024]={0};
    do{
        nready=::recv(socket_.fd(),buf,sizeof(buf),MSG_PEEK);
    }while(nready==-1&&errno==EINTR);
    
    return (nready==0);
}

void TcpConnection::setNonblock()
{
    socket_.nonBlock();
}


}
