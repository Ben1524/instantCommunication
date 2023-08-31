//
// Created by Stephen Curry on 5/8/2023.
//

#include "Epoller.h"
#include "ThreadPool.h"

Epoller::Epoller() : _events(MAXEVENT) {
    m_epollfd = epoll_create(1); //创建epoll句柄,1表示监听的fd数量，这个参数在Linux2.6.8之后被忽略，只要大于0就可以
    assert(m_epollfd >= 0);
    threadpool= std::make_unique<ThreadPool>(3,5,100);
}

Epoller::~Epoller() {
    close(this->m_epollfd); //关闭epoll句柄
}

int Epoller::addFd(int fd, TcpTool *tool, int event) {
    struct epoll_event events;
    events.data.ptr = tool; //将tool指针传递给events.data.ptr,这样在epoll_wait中就可以通过events.data.ptr获取到tool指针
    events.events = event;

    int ret = epoll_ctl(this->m_epollfd, EPOLL_CTL_ADD, fd, &events); //将fd添加到epoll句柄中
    return ret;
}

int Epoller::modFd(int fd, TcpTool *tool, int event) {
    struct epoll_event events;
    events.data.ptr = tool;
    events.events = event;

    // 将对应的fd的事件修改为event
    int ret = epoll_ctl(this->m_epollfd, EPOLL_CTL_MOD, fd, &events);
    return ret;
}

int Epoller::delFd(int fd, TcpTool *tool, int event) {
    struct epoll_event events;
    events.events = event;
    events.data.ptr = tool;


    int ret = epoll_ctl(this->m_epollfd, EPOLL_CTL_DEL, fd, &events);
    return ret;
}

int Epoller::wait(int time_ms) {

    int ret = epoll_wait(this->m_epollfd, &*_events.begin(), static_cast<int>(_events.size()), time_ms);
    if (ret <= 0) {

    } else {

    }
    return ret;
}

void Epoller::handleEvent(int listenfd, int eventnum) {
    assert(eventnum > 0);
    for (int i = 0; i < eventnum; i++) {
        TcpTool *tool = (TcpTool *) (_events[i].data.ptr);
        int fd = tool->getFd(); //获取fd

        if (fd == listenfd) { //如果是监听的fd
            std::cout << "listen event create" << std::endl; //这里是测试代码
            _connentioncb(); //调用新连接的回调函数
        } else if (_events[i].events & EPOLLIN) {  // 可读

            // std::bind绑定tool指针到_readcb函数中，这样在_readcb函数中就可以通过tool指针获取到tool对象
            threadpool->addTask(std::bind(_readcb, tool),tool); //将读的回调函数添加到线程池中去执行
        } else if (_events[i].events & EPOLLOUT) {  //可写 这里是测试代码 一般不会出现这种情况
            _writecb(tool); //调用写的回调函数
        }
    }
}
