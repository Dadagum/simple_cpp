#include <iostream>
#include "heap_timer.h"
using namespace std;

int add(int a, int b) {
    cout << "a = " << a << " ,b = " << b << endl;  
    return a + b;
}

int main() {
    simple_cpp::HeapTimer timer;
    auto f1 = timer.addTask(2000, add, 2, 2);
    auto f2 = timer.addTask(1000, add, 1, 1);
    auto f3 = timer.addTask(3000, add, 3, 3);
    timer.start(); // 可以选择另开线程执行
    cout << f1.get() << " " << f2.get() << " " << f3.get() << endl;
}