#pragma once

#include<boost/noncopyable.hpp>
#include<sys/types.h>
#include<sys/socket.h>
#include<fcntl.h>

namespace TcpServer
{

class Socket:public boost::noncopyable
    {
    public:
        Socket(int fd):fd_(fd)
        {}

        Socket()
        {
            fd_=socket(AF_INET,SOCK_STREAM,0);
        }

        int fd()
        {
            return fd_;
        }

        void shutdownWrite()
        {
            ::shutdown(fd_,SHUT_WR);
        }
        
        void nonBlock()
        {
            int flags=fcntl(fd_,F_GETFL,0);
            flags!=O_NONBLOCK;
            int ret=fcntl(fd_,F_SETFL,flags);
        }
        ~Socket()
        {
            ::close(fd_);
        }
    private:
        int fd_;
    };

}
