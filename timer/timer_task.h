#pragma once
#include <time.h>
#include <functional>
#include <memory>

#define MS_PER_SEC 1000
#define NS_PER_MS 1000000

namespace simple_cpp {

using Task = std::function<void()>;
using ll = long long;

class TimerUtils {
public:
    static ll getCurrentMs() {
        struct timespec request;
        clock_gettime(CLOCK_REALTIME, &request); // or CLOCK_MONOTONIC ?
        return request.tv_sec * MS_PER_SEC + request.tv_nsec / NS_PER_MS;
    }
private:
    TimerUtils();

};

class TimerTask {
public:
    TimerTask(Task task, ll expired, int period = 1): task_(task), expired_at(TimerUtils::getCurrentMs() + expired),
        expired_(expired), period_(period), active(true) {}
    // 经常性漏掉两个 const ...
    bool operator<(const TimerTask& rhs) const {
        return expired_at > rhs.expired_at;
    }
    void operator()() {
        task_(); 
        --period_;
        if (period_ > 0) {
            expired_at = TimerUtils::getCurrentMs() + expired_;
        } else active = false; // 任务执行次数已达标
    }
    ll getExpireTime() const {return expired_at;}
    int getPeriod() const {return period_;}
    bool isActive() const {return active;}
    TimerTask(const TimerTask& timer_task) {
        task_ = timer_task.task_;
        expired_at = timer_task.expired_at;
        expired_ = timer_task.expired_;
    }
    TimerTask& operator=(const TimerTask& timer_task) {
        if (this != &timer_task) {
            task_ = timer_task.task_;
            expired_at = timer_task.expired_at;
            expired_ = timer_task.expired_;
        }
        return *this;
    }

private:
    Task task_;
    ll expired_at;
    ll expired_;
    int period_;
    bool active; // timer 仍然有效
};

}