#pragma once
#include "timer_task.h"
#include <sys/epoll.h>
#include <queue>
#include <future>

namespace simple_cpp {

class HeapTimer {
public:
    HeapTimer(){}
    template<typename F, typename... Args>
    auto addTask(int expire_ms, F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type> {
        using ret_type = typename std::result_of<F(Args...)>::type;
        auto pk = std::make_shared<std::packaged_task<ret_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        Task task = [pk]() {(*pk)();};
        TimerTask timer_task(task, expire_ms);
        min_heap.emplace(timer_task);
        return pk->get_future();
    }
    template<typename F, typename... Args>
    void addPeriodTask(int period, int expire_ms, F &&f, Args &&...args) {
        Task task = std::function<void()>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        TimerTask timer_task(task, expire_ms, period);
        min_heap.emplace(timer_task);
    }
    /* 若想"异步"执行，可以另开线程调用 start() */
    void start() {
        int epfd = epoll_create(1);
        while (!min_heap.empty()) {
            epoll_wait(epfd, NULL, 1, getMinExpired());
            TimerTask timer_task = min_heap.top(); 
            min_heap.pop();
            timer_task();
            if (timer_task.isActive()) min_heap.emplace(timer_task);
        }
    }
    // 得到现在到下一次触发执行任务的时间间隔
    ll getMinExpired() const {
        ll now = TimerUtils::getCurrentMs();
        ll expire = min_heap.top().getExpireTime() - now; // 保证 min_heap 是否空
        return std::max(0ll, expire); // 返回 0 让 epoll_wait() 立刻返回
    }
    HeapTimer(HeapTimer&) = delete;
    HeapTimer& operator=(HeapTimer&) = delete;

private:
    std::priority_queue<TimerTask> min_heap;

};

}