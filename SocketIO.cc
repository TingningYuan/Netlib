#include"SocketIO.h"

namespace TcpServer
{

SocketIO::SocketIO(int fd):fd_(fd)
    {}

int SocketIO::readn(char* buf,int len)
{
    int left=len;
    char* p=buf;
    while(left>0){
        int ret=::read(fd_,p,left);
        if(ret==-1&&errno==EINTR){
            continue;
        }else if(ret==-1){
            return len-left;
        }else if(ret==0){
            return len-left;
        }else{
            left-=ret;
            p+=ret;
        }
    }
    return len-left;
}

int SocketIO::readLine(char* buf,int max)
{
    int left=max-1;
    char* p=buf;
    int ret;
    int total=0;
    while(left>0){
        ret=recvPeek(p,left);
        //查找\n
        for(int idx=0;idx!=ret;++idx){
            if(p[idx]=='\n'){
                int size=idx+1;
                readn(p,size);
                total+=size;
                p+=size;
                *p='\0';
                return total;
            }
        }
        //如果没有发现'\n'
        readn(p,ret);
        left-=ret;
        p+=ret;
        total+=ret;
    }
    *p='\0';//最终没有发现'\n'
    return total;
}

int SocketIO::recvPeek(char* buf,int len)
{
    int ret;
    do{
        ret=::recv(fd_,buf,len,MSG_PEEK);
    }while(ret==-1&&errno==EINTR);
    return ret;
}

int SocketIO::writen(const char* buf,int len)
{
    int left=len;
    const char* p=buf;
    while(left>0){
        int ret=::write(fd_,p,left);
        if(ret==-1&&errno==EINTR){
            continue;
        }else if(ret==-1){
            return len-left;
        }else{
            left-=ret;
            p+=ret;
        }
    }
    return len-left;
}

}
