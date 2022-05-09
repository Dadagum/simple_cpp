#pragma once
#include <time.h>
#include <functional>
#include <memory>

#define MS_PER_SEC 1000
#define NS_PER_MS 1000000

namespace simple_cpp {

using Task = std::function<void()>;
using ll = long long;

class TimerTask {
public:
    TimerTask(Task task, ll expired): task_(task), expired_(expired) {}
    // 经常性漏掉两个 const ...
    bool operator<(const TimerTask& rhs) const {
        return expired_ > rhs.expired_;
    }
    void operator()() {task_();}
    ll getExpire() const {return expired_;}
    TimerTask(const TimerTask& timer_task) {
        task_ = timer_task.task_;
        expired_ = timer_task.expired_;
    }
    TimerTask& operator=(const TimerTask& timer_task) {
        if (this != &timer_task) {
            task_ = timer_task.task_;
            expired_ = timer_task.expired_;
        }
        return *this;
    }

private:
    Task task_;
    ll expired_;
};

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

}