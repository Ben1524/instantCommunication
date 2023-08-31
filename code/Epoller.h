//
// Created by Stephen Curry on 5/8/2023.
//

#ifndef REACTOR_EPOLLER_H
#define REACTOR_EPOLLER_H
#define MAXEVENT 1024

#include "sys/epoll.h"
#include "iostream"
#include "unistd.h"
#include "functional"
#include "TcpTool.h"
#include "vector"
#include "map"
#include "memory"

class ThreadPool;

class Epoller{
public:
    Epoller();
    ~Epoller();


    using NewConnectionCallback = std::function<void()>;  //相当于定义了一种函数类型
    using HandleReadCallback = std::function<void(TcpTool*)>; // 处理读的回调用函数
    using HandleWriteCallback = std::function<void(TcpTool*)>; // 处理写的回调函数
    using CloseConnectionCallback = std::function<void(TcpTool*)>; // 关闭连接的回调函数



public:
    int addFd(int fd,TcpTool* tool,int event); //添加事件
    int modFd(int fd,TcpTool* tool,int event); //修改事件
    int delFd(int fd,TcpTool* tool,int event); //删除事件
    int wait(int time_ms); //等待事件

    void handleEvent(int listenfd,int events); //处理事件

    //事件回调函数
    void setNewConnection(const NewConnectionCallback &cb){
        _connentioncb = cb;
    }
    void setCloseConnection(const CloseConnectionCallback &cb){
        _closecb = cb;
    }
    void setWriteCb(const HandleWriteCallback &cb){
        _writecb = cb;
    }
    void setReadCb(const HandleReadCallback &cb){
        _readcb = cb;
    }

private:
    using eventList = std::vector<struct epoll_event>; //事件集合

    int m_epollfd; //epoll句柄,用于管理事件
    eventList  _events;  //事件集合

    NewConnectionCallback _connentioncb; //新连接的回调函数
    HandleReadCallback  _readcb; //读的回调函数
    HandleWriteCallback _writecb; //写的回调函数
    CloseConnectionCallback _closecb; //关闭连接的回调函数

    // 线程池
    std::unique_ptr<ThreadPool> threadpool;
    Cache* cache = Cache::getInstance();
};


#endif //REACTOR_EPOLLER_H
