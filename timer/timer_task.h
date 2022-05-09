#pragma once
#include <functional>
#include <memory>

namespace simple_cpp {

class TimerTask {
public:
    using Task = std::function<void()>;
    using ll = long long;
    TimerTask(std::unique_ptr<Task> tp, ll expired): tp_(std::move(tp)),
    expired_(expired) {}
    TimerTask(TimerTask&) = delete;
    TimerTask& operator=(TimerTask&) = delete;

private:
    std::unique_ptr<Task> tp_;
    ll expired_;
};

}