#include "connection_pool.h"
#include <iostream>
#include <vector>
#include <thread>
#include <unistd.h>
using namespace std;

class Conn {
public:
    Conn() {cout << "Conn()" << endl;}
    ~Conn() {cout << "~Conn" << endl;}
};

Conn* build() {
    return new Conn();
}

void destroy(Conn* conn) {
    delete conn;
}

int main() {
    int num = 5;
    std::unique_ptr<Conn, std::function<void(Conn*)>> ptr;
    {
        auto cp = simple_cpp::ConnectionPool<Conn>::build(1, 1000, build, destroy);
        vector<thread> vt;
        for (int i = 0; i < num; ++i) {
            thread th{[cp](){ auto c = cp->getConnection();}}; // 模拟多线程访问
            vt.emplace_back(std::move(th));
        }
        ptr = std::move(cp->getConnection());
        for (int i = 0; i < vt.size(); ++i) {
            if (vt[i].joinable()) {
                vt[i].join();
            }
        }
    }
    cout << "outer block" << endl;
}