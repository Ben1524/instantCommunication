//
// Created by a1767 on 2023/8/24.
//

#include "timer.h"

#include <cassert>
#include <chrono>

by::timelineTimer::timelineTimer():
repeatCount(-1),m_startTime(0),m_endTime(0),m_interval(0)
{
    m_startTime = getCurrentTime();
}
char * by::timelineTimer::getCurrentTimeStr()
{
    // 将时间转换为字符串,格式为:月-日 时:分
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char str[20];
    sprintf(str,"%d-%d %d:%d",1+ltm->tm_mon,ltm->tm_mday,ltm->tm_hour,ltm->tm_min);
    return str;
}

by::timelineTimer::timelineTimer(int repeatCount)
:repeatCount(repeatCount),m_startTime(0),m_endTime(0),m_interval(0)
{
    m_startTime = getCurrentTime();
}

void by::timelineTimer::setRepeatCount(int count)
{
    assert(count>=-1);
    repeatCount = count;
}

by::timelineTimer::~timelineTimer()
{

}

unsigned long long by::timelineTimer::getCurrentTime()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void by::timelineTimer::trigger()
{
    if(m_callback== nullptr||repeatCount==0) // 没有设置回调函数,m_callback为空
    {
        return;
    }
    m_callback();
    m_startTime+=m_interval; // 更新下一次的开始时间
    if(repeatCount>0)
    {
        repeatCount--;
    }
}



void by::timerManager::update()
{
    if(m_timers.empty()&&task_queue.empty()) // 如果定时器和队列为空,则直接返回
    {
        return;
    }
    if(!task_queue.empty())
    {
        std::lock_guard<std::mutex> lock(m_mutex_queue);
        while(!task_queue.empty())
        {
            m_timers.insert(task_queue.front());
            task_queue.pop();
        }
    }
    unsigned long long currentTime = timelineTimer::getCurrentTime();
    for(auto it = m_timers.begin();it!=m_timers.end();)
    {
        if(it->first<=currentTime) // 当前时间大于等于定时器的开始时间,定时器触发
        {
            it->second.trigger(); //trigger会自动更新下一次的开始时间

            auto t=it->second; // 保存定时器
            it=m_timers.erase(it); // 删除定时器,返回下一个定时器的迭代器,因为删除后迭代器失效
            if(t.repeatCount!=0) // 如果定时器还有剩余次数,则重新添加定时器
            {
                auto new_it=m_timers.insert(std::make_pair(t.m_startTime,t));//返回值是一个pair,first是指向插入元素的迭代器,second是一个bool值,表示是否插入成功
                if(it==m_timers.end()||new_it->first<it->first)
                {// 如果插入的定时器的开始时间小于下一个定时器的开始时间或者map为空,则更新下一个要遍历的迭代器
                    it=new_it;
                }
            }
        }
        else // 当前时间小于定时器的开始时间,则退出循环,map中的定时器是按照开始时间排序的,所以后面的定时器也不会触发
        {
            break;
        }
    }
}

void by::timerManager::start()
{
    m_isRunning.store(true);
    while(m_isRunning.load())
    {
        update();
    }
}

void by::timerManager::stop()
{
    m_isRunning.store(false);
}

