#pragma once

#include"Socket.h"
#include"SocketIO.h"
#include"InetAddress.h"
#include<boost/noncopyable.hpp>
#include<string>
#include<memory>
#include<functional>

namespace TcpServer
{

class EventLoop;
class TcpConnection;
using TcpConnectionPtr=std::shared_ptr<TcpConnection>;
using TcpConnectionCallback=std::function<void(const TcpConnectionPtr& conn)>;


class TcpConnection
    :public boost::noncopyable,
    public std::enable_shared_from_this<TcpConnection>
    {
    public:
        TcpConnection(int fd,EventLoop* loop);
        ~TcpConnection();

        std::string receive();
        void send(const std::string& msg);
        void sendInLoop(const std::string& msg);

        std::string toString() const;
        void shutdown();

        void setConnectionCallback(const TcpConnectionCallback& cb)
        {
            onConnection_=cb;
        }
        void setMessageCallback(const TcpConnectionCallback& cb)
        {
            onMessage_=cb;
        }
        void setCloseCallback(const TcpConnectionCallback& cb)
        {
            onClose_=cb;
        }

        void handleConnectionCallback();
        void handleMessageCallback();
        void handleCloseCallback();

        bool isClosed();
        void setNonblock();
    private:
        InetAddress getLocalAddr(int fd);
        InetAddress getPeerAddr(int fd);
    private:
        Socket socket_;
        Socket peerSocket_;
        SocketIO socketIO_;
        InetAddress localAddr_;
        InetAddress peerAddr_;
        bool isShutdownWrite_;
        EventLoop* loop_;
        TcpConnectionCallback onConnection_;
        TcpConnectionCallback onMessage_;
        TcpConnectionCallback onClose_;
    };

}
