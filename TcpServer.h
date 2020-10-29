#pragma once

#include"Acceptor.h"
#include<boost/noncopyable.hpp>

namespace TcpServer
{

class TcpServer:public boost::noncopyable
    {
    public:

    private:
        Acceptor acceptor_;
        EventLoop loop_;
    };

}
