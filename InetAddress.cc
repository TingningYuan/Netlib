#include"InetAddress.h"

#include<string.h>

namespace TcpServer
{
InetAddress::InetAddress(unsigned short port)
{
    memset(&addr_,0,sizeof(struct sockaddr_in));
    addr_.sin_family=AF_INET;
    addr_.sin_port=htons(port);
    addr_.sin_addr.s_addr=INADDR_ANY;
}

InetAddress::InetAddress(const std::string& ip,unsigned short port)
{
    memset(&addr_,0,sizeof(struct sockaddr_in));
    addr_.sin_family=AF_INET;
    addr_.sin_port=htons(port);
    addr_.sin_addr.s_addr=inet_addr(ip.c_str());
}

InetAddress::InetAddress(const struct sockaddr_in& addr):addr_(addr)
{}

std::string InetAddress::ip() const
{
    return std::string(inet_ntoa(addr_.sin_addr));
}

unsigned short InetAddress::port() const
{
    return ntohs(addr_.sin_port);
}


}
