//
// Created by Stephen Curry on 5/23/2023.
//

#include "Cache.h"
#include <thread>
#include "Epoller.h"
#include "TcpServer.h"


Cache* Cache::m_cache = new Cache;

Cache *Cache::getInstance() {
    return m_cache;
}

void Cache::deleteInstance() {
    if(m_cache)
    {
        delete m_cache;
        m_cache = nullptr;
    }
}

Cache::~Cache() {
    pthread_rwlock_destroy(&rwlock);
}
Cache::Cache() {
    pthread_rwlock_init(&rwlock, nullptr);
    timerManagerPtr= std::make_unique<by::timerManager>(); // 初始化定时器
}




void Cache::printCache()
{
    std::cout << "在线用户:" << std::endl;
    for(auto it:UID_map)
    {
        std::cout << "uid:" << it.first << " fd:" << it.second.fd << " time:" << it.second.time << std::endl;
    }
    std::cout << "离线用户:" << std::endl;
    for(auto it = offlineUser.begin(); it != offlineUser.end(); it++)
    {
        std::cout << "fd:" << it->fd << " time:" << it->time << std::endl;
    }
}

void Cache::addUser(const std::string &uid, int fd, const std::string &time)
{
    pthread_rwlock_wrlock(&rwlock); // 加写锁(互斥锁)
    user u;
    u.fd = fd;
    strcpy(u.time,time.c_str());
    UID_map[uid] = u;
    // 添加一个定时器,用于检测用户是否超时
    timerManagerPtr->addTimer(MAXTIMEOUT, [this, uid](){
        // 超时后,将用户添加到超时退出预备队列
        exit.push_back(uid);
    });
    pthread_rwlock_unlock(&rwlock); // 解锁

}

int Cache::getUserFd(std::string uid)
{
    pthread_rwlock_rdlock(&rwlock); // 加读锁
    auto it = UID_map.find(uid);
    if(it == UID_map.end())
    {
        pthread_rwlock_unlock(&rwlock); // 解锁
        return -1;
    }
    pthread_rwlock_unlock(&rwlock); // 解锁
    return it->second.fd;
}

int Cache::delUser(std::string uid)
{
    pthread_rwlock_wrlock(&rwlock); // 加写锁
    auto it = UID_map.find(uid);
    if(it == UID_map.end())
    {
        pthread_rwlock_unlock(&rwlock); // 解锁
        return -1;
    }
    user u = it->second;
    offlineUser.push_back(u);
    UID_map.erase(it);
    pthread_rwlock_unlock(&rwlock); // 解锁
    return 0;
}

Json Cache::getOnlineUser()
{
    Json arr;
    pthread_rwlock_rdlock(&rwlock); // 加读锁
    for(auto it:UID_map)
    {
        Json obj;
        obj["userName"] = it.first;
        obj["loginTime"] = it.second.time;
        arr.append(obj);
    }
    pthread_rwlock_unlock(&rwlock); // 解锁
    return arr;
}
Json Cache::getOfflineUser()
{
    Json arr;
    pthread_rwlock_rdlock(&rwlock); // 加读锁
    for(auto it:offlineUser)
    {
        Json obj;
        obj["userName"] = it.time;
        obj["loginTime"] = it.uid;
        arr.append(obj);
    }
    pthread_rwlock_unlock(&rwlock); // 解锁
    return arr;
}

std::vector<int> Cache::getFdList()
{
    std::vector<int> fdList;
    pthread_rwlock_rdlock(&rwlock); // 加读锁
    for(auto it:UID_map)
    {
        fdList.push_back(it.second.fd);
    }
    pthread_rwlock_unlock(&rwlock); // 解锁
    return fdList;
}

void Cache::updateTime(const std::string & u)
{
    // 先检查是否在超时队列中
    pthread_rwlock_wrlock(&rwlock); // 加写锁
    auto it = std::find(exit.begin(), exit.end(), u);
    if(it != exit.end()) // 在超时队列中
    {
        exit.erase(it); // 从超时队列中删除
    }
    else // 不在超时队列中
    {
        auto it = UID_map.find(u);

    }

}

