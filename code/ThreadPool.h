//
// Created by a1767 on 2023/8/11.
//

#ifndef MY_THREAD_POOL_THREADPOOL_H
#define MY_THREAD_POOL_THREADPOOL_H

#include <functional>
#include <list>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>
#include <iostream>

class TcpTool;
using callBackFunction=std::function<void(TcpTool*)>;

struct Task
{
    explicit Task(callBackFunction func, TcpTool* arg):function(func), arg(arg){}
    Task()=default;

    Task(const Task &task);

    callBackFunction function;
    TcpTool* arg;
    int priority=0;
    bool operator<(const Task& task) const ;
    Task& operator=(const Task& task);
};

class ThreadPool
{
public:
    ThreadPool(int startNum,int minNum=100,int maxNum=1000);

    ThreadPool();

    ~ThreadPool();
    void addTask(Task task);
    void addTask(callBackFunction func, TcpTool* arg);
private:
    static void manager(ThreadPool*); // 管理者线程的回调函数
    static void worker(ThreadPool*); // 工作线程的回调函数
    static void threadExit(ThreadPool* pool,std::thread::id id); // 线程退出函数

    std::priority_queue<Task> taskQueue; // 任务队列

    bool isStart= false; // 线程池是否启动
    int currentNum=0; // 当前线程数量
    int maxNum=0; // 最大线程数量
    int minNum=0;
    int busyNum=0; // 忙的线程数量
    int exitNum=0; // 要销毁的线程数量

    int maxTaskNum=20; // 最大任务数量
    int num=3; // 每次创建和销毁的线程数量

    std::mutex threadPoolMutex; // 线程池互斥锁
    std::mutex busyMutex; // 忙的线程数量互斥锁

    std::condition_variable notFull; // 任务队列是不是满了
    std::condition_variable notEmpty; // 任务队列是不是空了
    std::list<std::thread*> threadList; // 线程列表


};


#endif //MY_THREAD_POOL_THREADPOOL_H
