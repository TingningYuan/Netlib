#include"TcpServer.h"
#include"ThreadPool.h"
#include"Thread.h"
#include<unistd.h>
#include<stdio.h>
#include<string>
#include<iostream>

using std::cout;
using std::endl;
using namespace TcpServer;
Threadpool* gThreadpool=nullptr;

class MyTask
{
public:
    MyTask(const string& message,const TcpConnectionPtr& conn)
        :message_(message),conn_(conn)
    {}

    void process()
    {
        cout<<current_thread::name<<" thread is going to do the task"<<endl;
        conn_->sendInLoop(message_);
    }
private:
    string message_;
    const TcpConnectionPtr& conn_;
};

void onConnection(const TcpConnectionPtr& conn)
{
    cout<<conn->toString()<<" has connected"<<endl;
}

void onMessage(const TcpConnectionPtr& conn)
{
    string msg=conn->receive();
    cout<<">>server: "<<msg<<endl;
    MyTask task(msg,conn);
    gThreadpool->addTask(bind(&MyTask::process,task));

}

void onClose(const TcpConnectionPtr& conn)
{
    cout<<conn->toString()<<" has closed"<<endl;
}

int main()
{
    Threadpool threadpool(4,10);
    gThreadpool=&threadpool;
    threadpool.start();
    TcpServer server("192.168.40.135",80000);
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);
    server.setCloseCallback(onClose);
    server.start();
    return 0;
}
