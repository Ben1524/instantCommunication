//
// Created by Luo Ruiheng on 5/8/2023.
// Reactor
//

#ifndef REACTOR_TCPTOOL_H
#define REACTOR_TCPTOOL_H
#include "iostream"
#include "assert.h"
#include "unistd.h"
#include <cstring>
#include <vector>
#include "sys/socket.h"
#include "TcpSql.h"
#include "string"
#include "functional"
#include <netinet/in.h>
#include "sys/socket.h"
#include "Cache.h"
#include "Log.h"

#define BUFFMAXSIZE 1024

//==================handle==================================
//处理读写的class   具体的handle实例
class TcpTool {
public:
    explicit TcpTool(int fd):m_fd(fd)
    {
        assert(m_fd>=0);
    }
    ~TcpTool(){
        close(m_fd);
    }

public:
    int getFd();
    std::string getUid();
    int read();
    int readn(char *buffer, const size_t n);
    int sendn( const char *buffer, const size_t n,int fd);




    //@brief: 指定单个分隔符（单个字符或子串）分割字符串
    //@param: src 原字符串；delimiter 分隔符，单个字符或子串
    std::vector<std::string> splitStr(const std::string& src, const std::string& delimiter);

    virtual void dowork(const char* buf);   // 处理读到的数据,后续子类重写
    void enroll(const char* buf); // 注册
    void login(const char*); // 登录
    void transfer(const char*); // 转发消息,用于直接发送的所有消息
    void singleChat(const char*); // 私聊
    void send_user();   // 发送在线用户

protected:
    void sendNewUserForInfo(int fd); // 为新用户发送各种初始化信息

    int m_fd;
    std::string Uid; // 用户id
    std::vector<std::string>vec_work;
    TcpSql* tcpsql = TcpSql::getInstance(); // 数据库操作

    Cache* cache = Cache::getInstance(); // 保存在线用户的fd,用于转发消息


};



#endif //REACTOR_TCPTOOL_H
