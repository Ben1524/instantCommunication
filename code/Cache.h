//
// Created by Stephen Curry on 5/23/2023.
// 缓存在线的用户,可以转发消息
//

#ifndef REACTOR_CACHE_H
#define REACTOR_CACHE_H
#include "map"
#include "iostream"
#include "Log.h"
#include <functional>
#include <pthread.h>
#include <list>
#include "../Json.h"
#include "../timer.h"


using namespace byjson;

#define MAXLEAVETIME 5*60*1000 //最大离线时间

class TcpServer;
class TcpTool;

#define MAXTIMEOUT 5*60*1000 //最大超时时间
#define MAXINSPECT 5*1000 //最大巡检时间 ,用于检查超时队列
 
struct user
{ // 保存离线用户的fd
    int fd; //套接字
    char uid[20]; //用户id
    char time[20]; //登录时间
};

class timerManager;

class Cache {
public:

    using CloseConnectionCallback = std::function<void(TcpTool*)>; // 关闭连接的回调函数

    void setCloseConnectionCallback(const CloseConnectionCallback& cb){
        closeConnectionCallback = cb;
    }

    static void deleteInstance();

    static Cache* getInstance(void);

    void printCache();
    void addUser(const std::string& uid, int fd, const std::string& time); // 添加用户

    Json getOnlineUser(); // 获取在线用户
    Json getOfflineUser(); // 获取离线用户

    int  delUser(std::string);

    int getUserFd(std::string);

    void updateTime(const std::string&); // 更新用户时间

    std::vector<int> getFdList(); // 获取所有在线用户的fd
private:

    Cache();
    ~Cache();
    Cache(const Cache& cache) = delete;
    Cache& operator=(const Cache& cache) = delete;

private:
    CloseConnectionCallback closeConnectionCallback;
    pthread_rwlock_t rwlock{}; // 读写锁
    static Cache* m_cache;
    std::list<user> offlineUser; // 保存离线用户的信息,用于转发消息
    std::map<std::string,user>UID_map; // 保存在线用户的fd,用于转发消息

    // 超时退出预备队列 , 用于一次处理多个超时退出的用户
    std::vector<std::string> exit;

    std::unique_ptr<by::timerManager> timerManagerPtr;
};


#endif //REACTOR_CACHE_H
