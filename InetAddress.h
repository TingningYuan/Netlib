#pragma once

#include<arpa/inet.h>
#include<string>
#include<boost/noncopyable.hpp>

namespace TcpServer
{

class InetAddress:public boost::noncopyable
    {
    public:
        explicit InetAddress(unsigned short port);
        InetAddress(const std::string& ip,unsigned short port);
        InetAddress(const struct sockaddr_in& addr);

        std::string ip() const;
        unsigned short port() const;
        struct sockaddr_in* getInetAddressPtr()
        {
            return &addr_;
        }
    private:
        struct sockaddr_in addr_;
    };
}
