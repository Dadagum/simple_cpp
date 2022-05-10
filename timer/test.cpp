#include <iostream>
#include <string>
#include "heap_timer.h"
using namespace std;

int add(int a, int b) {
    cout << "a = " << a << " ,b = " << b << endl;  
    return a + b;
}

void print(string str) {
    cout << str << endl;
}

int main() {
    simple_cpp::HeapTimer timer;
    // 一次性任务
    auto f1 = timer.addTask(2000, add, 2, 2);
    auto f2 = timer.addTask(1000, add, 1, 1);
    auto f3 = timer.addTask(3000, add, 3, 3);
    timer.start(); // 可以选择另开线程执行
    cout << "res: " << f1.get() << " " << f2.get() << " " << f3.get() << endl;
    // 阶段性任务
    timer.addPeriodTask(3, 1000, print, "hello");
    timer.start();
    return 0;
}