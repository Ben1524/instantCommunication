//
// Created by a1767 on 2023/8/11.
//

#include "ThreadPool.h"
#include <memory>

Task::Task(const Task &task):
function(task.function),priority(task.priority),arg(task.arg)
{}
Task& Task::operator=(const Task &task)
{
    if(this==&task)
    {
        return *this;
    }
    this->function=task.function;
    this->priority=task.priority;
    this->arg=task.arg;
    return *this;
}
bool Task::operator<(const Task& task) const
{
    return this->priority>task.priority;
}


ThreadPool::ThreadPool(int startNum,int minNum,int maxNum) : maxNum(maxNum),minNum(minNum)
{
    isStart = true;
    for (int i = 0; i < startNum; ++i)
    {
        std::thread* thread = new std::thread(worker, this);
        threadList.push_back(thread);
        currentNum++;
    }
    std::unique_ptr<std::thread> man_ptr= std::make_unique<std::thread>(manager,this);
    man_ptr->detach();
}

void ThreadPool::addTask(Task task)
{
    {
        std::unique_lock<std::mutex> t(threadPoolMutex);
        while (taskQueue.size() >= maxTaskNum) // 任务队列满了
        {
            notFull.wait(t); // 等待任务队列不满
        }
    }
    {
        std::lock_guard<std::mutex> lock(threadPoolMutex);
        taskQueue.push(task);
    }
    notEmpty.notify_one();
}
void ThreadPool::addTask(callBackFunction func, TcpTool *arg)
{
    {
        std::unique_lock<std::mutex> t(threadPoolMutex);
        while (taskQueue.size() >= maxTaskNum) // 任务队列满了
        {
            notFull.wait(t); // 等待任务队列不满
        }
    }
    {
        std::lock_guard<std::mutex> lock(threadPoolMutex);
        Task task(func, arg);
        taskQueue.push(task);
    }
    notEmpty.notify_one();
}
void ThreadPool::worker(ThreadPool * pool)
{
    while(pool!= nullptr&&pool->isStart)
    {
        Task temp;
        {
            std::unique_lock<std::mutex> lock(pool->threadPoolMutex);
            // 条件变量的第二个参数是一个lambda表达式，返回值是bool类型,如果返回值是false,那么就会阻塞
            // 条件变量会阻塞当前线程，直到另外一个线程调用notify_one()或者notify_all()成员函数
            while(pool->taskQueue.empty()== true&&pool->isStart) // 每调用一次wait()，就会先解锁，然后阻塞当前线程
                // 当其他线程调用notify_one()或者notify_all()成员函数后，wait()函数就会返回，然后wait()函数重新锁住互斥量
                // 然后wait()函数返回后，就可以继续执行下面的代码,使用while是为了防止虚假唤醒(即使没有其他线程调用notify_one()或者notify_all()成员函数，wait()函数也有可能返回)
                pool->notEmpty.wait(lock); //参数是一个std::unique_lock
            if(pool->exitNum>0)
            {
                pool->exitNum--; // 要销毁线程减一
                if(pool->currentNum>pool->minNum)
                {
                    pool->currentNum--;
                    lock.unlock();// 解锁
                    // 当前线程退出
                    threadExit(pool,std::this_thread::get_id());
                    return; // 退出线程
                }
            }
            else
            {
                temp = pool->taskQueue.top();
                pool->taskQueue.pop();
                pool->notFull.notify_one(); // 通知任务队列不满可以添加任务
            }
            lock.unlock(); // 解锁
        }
        pool->busyMutex.lock();
        pool->busyNum++; // 忙的线程数量加一
        pool->busyMutex.unlock();
        temp.function(temp.arg); // 执行任务

        pool->busyMutex.lock();
        pool->busyNum--; // 忙的线程数量减一
        pool->busyMutex.unlock();
    }
    if(pool->isStart== false) // 线程池关闭
    {
        pool->busyMutex.lock();
        pool->busyNum--; // 忙的线程数量减一
        pool->busyMutex.unlock();
        threadExit(pool,std::this_thread::get_id()); // 退出线程
        return;
    }
}



void ThreadPool::threadExit(ThreadPool *pool, std::thread::id id)
{
    auto item=pool->threadList.begin();
    for(;item != pool -> threadList.end()&&(*item)->get_id()!=id; item++);
    (*item)->detach(); // 分离线程,不然会抛出异常
}

void ThreadPool::manager(ThreadPool *pool)
{
    while(pool->isStart== true) //线程池启动
    {
        std::this_thread::sleep_for(std::chrono::seconds(3)); // 休眠3秒
        pool->busyMutex.lock();
        int busyNum=pool->busyNum; // 获取忙的线程数量
        int currentNum=pool->currentNum;
        pool->busyMutex.unlock();
        int leisureNum=currentNum-busyNum; // 空闲的线程数量
        int taskNum=pool->taskQueue.size(); // 任务数量
        if(taskNum>leisureNum&&currentNum<pool->maxNum) // 任务数量大于空闲的线程数量,且当前线程数量小于最大线程数量
        {
            for(int i=0;i<pool->num;i++)
            {
                std::thread* thread=new std::thread(worker,pool);
                pool->threadList.push_back(thread);
                pool->currentNum++;
                if(pool->currentNum==pool->maxNum)
                {
                    break;
                }
            }
        }
        else if(leisureNum-taskNum>pool->minNum) // 空闲的线程数量大于任务数量,且空闲的线程数量大于最小线程数量
        {
            for(int i=0;i<pool->num;i++)
            {
                pool->exitNum++; // 要销毁的线程数量加一
                pool->notEmpty.notify_one(); // 唤醒一个线程，相当于执行了一个销毁线程的任务
            }
        }
    }
    if(pool->isStart== false) // 线程池关闭
    {
        return; // 退出线程
    }
}
ThreadPool::~ThreadPool()
{
    if(isStart== true)
    {
        isStart= false; // 关闭线程池
        exitNum= currentNum; // 要销毁的线程数量等于当前线程数量
        minNum=0; // 最小线程数量设置为0
        notEmpty.notify_all(); // 唤醒所有线程
    }
    std::cout<<"~ThreadPool()"<<std::endl;
}

ThreadPool::ThreadPool()
{}

