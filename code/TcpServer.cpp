//
// Created by Stephen Curry on 5/9/2023.
//

#include "TcpServer.h"
#include "Log.h"
#include "command.h"

void TcpServer::_acceptConnection (){
    struct sockaddr_in clitaddr;
    socklen_t len;
    int acceptFd = ::accept(listenFd, (struct sockaddr *) &clitaddr, &len);
    if (acceptFd == -1)
    {
        LOG_WARN_CONSLE("accept error!")
        std::cout << "accept error!" << std::endl;
        std::cout << strerror(errno) << std::endl;
        return;
    }
    std::cout << "acceptfd = " << acceptFd << std::endl;
    LOG_DEBUG_CONSLE("acceptfd = {}", acceptFd);
    TcpTool *tool = new TcpTool(acceptFd);  // 创建一个新的TcpTool对象
    _epoll->addFd(acceptFd, tool, (EPOLLIN | EPOLLET)); // 将新的TcpTool对象添加到epoll中
}

void TcpServer::_closeConnection (TcpTool *tool){
    int fd = tool->getFd();
    std::cout<<"close fd = "<<fd<<std::endl;
    LOG_DEBUG_CONSLE("closefd = {}",fd);
    // 删除epoll中的fd
    _epoll->delFd(fd,tool,0);

    // 删除Cache中的fd
    m_cache->delUser(tool->getUid());

    delete tool;
    tool = nullptr;
}

void TcpServer::_handleWrite (TcpTool * tool){
    tool->sendn("",0,tool->getFd()); //发送完毕
}

// 处理客户端的读写
void TcpServer::_handleRead  (TcpTool * tool){

    int fd = tool->getFd();
    // todo 更新时间
    //m_cache->updateTime(tool->Uid);
    std::cout<<"_handleRead fd = "<<fd<<std::endl; //打印fd
    int nRead = tool->read(); //读取数据同时处理数据

    if(nRead == 0 || (nRead <0 && (errno != EAGAIN))) // 读取错误或者客户端关闭连接
    { //客户端关闭连接
        std::string close_uid{CLOSE};
        close_uid +=tool->getUid();
        _closeConnection(tool);
        //将下线的消息发送给每个在线的客户
        std::vector<int> online_fd = m_cache->getFdList();
        for(auto &fd : online_fd)
        {
            tool->sendn(close_uid.c_str(),close_uid.size(),fd);
        }
        return ;
    }

    if(nRead < 0 && errno == EAGAIN)  //读取完毕。EAAGIN是非阻塞读取完成的标志
    {
        _epoll->modFd(fd,tool,(EPOLLIN)); //修改为可读
        return ;
    }
}


void TcpServer::run(){
    // get返回的是智能指针的原始指针
    LOG_INFO_CONSLE("服务器运行中");
    _epoll->addFd(listenFd,_listenTool.get(),(EPOLLIN | EPOLLET));
    // this在这里使得回调函数是由当前TcpServer类来调用的
    // std::bind 绑定成员函数要指定类的对象，因为成员函数是依赖于对象的
    _epoll->setNewConnection(std::bind(&TcpServer::_acceptConnection,this));
    // std::bind(&TcpServer::_acceptConnection,this)  返回一个可调用对象
    _epoll->setWriteCb(std::bind(&TcpServer::_handleWrite,this,std::placeholders::_1));
    // std::forward<decltype(PH1)>(PH1) 保持参数的类型 不会丢失右值的特性, 保持参数的完美转发
    _epoll->setReadCb([this](auto && PH1) { _handleRead(std::forward<decltype(PH1)>(PH1)); });

//开始事件循环
    while(1){
        int eventsNum = _epoll->wait(-1);
        LOG_DEBUG_CONSLE("eventNUM = {}",eventsNum)
        if(eventsNum > 0){
            _epoll->handleEvent(listenFd,eventsNum); // 事件处理
        }
    }
}

int TcpServer::createListenFd(int port)
{
    //防止port
    int _listenFd = ::socket(AF_INET,SOCK_STREAM | SOCK_NONBLOCK,0);
    int opt=1;
    setsockopt(_listenFd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    if(_listenFd==-1)
    {
        LOG_ERROR_CONSLE("listen socket create error")
        exit(-1);
    }
    struct sockaddr_in  _servAddr;
    port = ((port <= 1024) || (port >=65536)) ? 10000:port;
    ::memset(&_servAddr,0,sizeof(_servAddr));
    _servAddr.sin_family = AF_INET;
    _servAddr.sin_port = ::htons((unsigned short)port);
    _servAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
    if((::bind(_listenFd,(struct sockaddr*) &_servAddr,sizeof(_servAddr))) == -1){
        LOG_ERROR_CONSLE("bind error")
        return -1;
    }

    if((::listen(_listenFd,5)) == -1){
        LOG_ERROR_CONSLE("listen error")
        return -1;
    }

    return _listenFd;
}

