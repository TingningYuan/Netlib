#pragma once

#include<boost/noncopyable.hpp>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<errno.h>
#include<stdio.h>

namespace TcpServer
{

class SocketIO:public boost::noncopyable
    {
    public:
        explicit SocketIO(int fd);
        int readn(char* buf,int len);
        int readLine(char* buf,int max);
        int writen(const char* buf,int len);

    private:
        int recvPeek(char* buf,int len);
    private:
        int fd_;
    };
}
