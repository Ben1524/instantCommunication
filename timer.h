//
// Created by a1767 on 2023/8/24.
//

#ifndef BYTIMER_TIMELINETIMER_H
#define BYTIMER_TIMELINETIMER_H

#include <functional>
#include <map>
#include <mutex>
#include <queue>
#include <atomic>
namespace by
{

    class timelineTimer // 时间轴定时器
    {
    public:
        friend class timerManager;

        using TimerCallback = std::function<void()>;
        timelineTimer();
        explicit timelineTimer(int repeatCount);
        ~timelineTimer();

        template<class Func,typename...Arg>
        void setCallbackFunction(int interval,Func&&f,Arg&&...arg); // 设置回调函数
        void setRepeatCount(int count);
        void trigger();
        static unsigned long long getCurrentTime();
        static char* getCurrentTimeStr();

    private:
        int repeatCount; // 重复次数
        unsigned long long m_startTime,m_endTime;
        TimerCallback m_callback;
        int m_interval; // ms
    };

    template<class Func, typename... Arg>
    void timelineTimer::setCallbackFunction(int interval, Func &&f, Arg &&... arg)
    {
        m_interval = interval;
        m_callback = std::bind(std::forward<Func>(f),std::forward<Arg>(arg)...);
    }


    class timerManager
    {
    public:
        template<class F,typename...Args>
        void addTimer(int interval,F&&f,Args&&...args);
        template<class F,typename...Args>
        void addTimer(int interval,int re,F&&f,Args&&...args);
        void update();
        void start();
        void stop();
        timerManager()= default;
        ~timerManager()= default;
        timerManager(const timerManager&)= delete;
        timerManager& operator=(const timerManager&)= delete;

    private:
        std::atomic<bool> m_isRunning;
        std::mutex m_mutex_queue; // 保护task_queue
        std::queue<std::pair<unsigned long long,timelineTimer>> task_queue; // 用于存储已经触发的时间轴定时器
        std::multimap<unsigned long long,timelineTimer> m_timers; // 用于存储时间轴定时器,时间轴定时器按照开始时间排序
    };

    template<class F, typename... Args>
    void timerManager::addTimer(int interval, int re, F &&f, Args &&... args)
    {
        std::lock_guard<std::mutex> lock(m_mutex_queue); // 保护task_queue
        timelineTimer timer;
        timer.setRepeatCount(re);
        timer.setCallbackFunction(interval,std::forward<F>(f),std::forward<Args>(args)...);
        task_queue.push(std::make_pair(timelineTimer::getCurrentTime(),timer)); // 将时间轴定时器放入task_queue
    }

    template<class F, typename... Args>
    void timerManager::addTimer(int interval, F &&f, Args &&... args)
    {
        addTimer(interval,-1,std::forward<F>(f),std::forward<Args>(args)...);
    }
}


#endif //BYTIMER_TIMELINETIMER_H
