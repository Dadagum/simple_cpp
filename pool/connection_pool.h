#pragma once
#include <memory>
#include <functional>
#include <mutex>
#include <queue>
#include <iostream>

/**
 * 线程安全连接池实现 
 */

namespace simple_cpp {

template<typename Conn>
class ConnectionPool : public std::enable_shared_from_this<ConnectionPool<Conn>> {
public:

    /* 工厂方法：得到 ConnectionPool 对象 */ 
    static std::shared_ptr<ConnectionPool> build(int active, long long expired, 
        std::function<Conn*()> factory, std::function<void(Conn*)> deletor) {
        ConnectionPool* pool = new ConnectionPool(active, expired, factory, deletor);
        return std::shared_ptr<ConnectionPool>(pool);
    }

    ~ConnectionPool() {
        std::cout << "~ConnectionPool" << std::endl;
        while (!conn_queue.empty()) {
            Conn* conn = conn_queue.front();
            conn_queue.pop();
            deletor_(conn);
        }
    }

    // /* 获取一个连接 */
    std::unique_ptr<Conn, std::function<void(Conn*)>> getConnection() {
        std::lock_guard<std::mutex> lock(mtx);
        // 判断是否有活跃连接可以直接取用
        Conn* ptr;
        if (!conn_queue.empty()) {
            ptr = conn_queue.front();
            conn_queue.pop();
        } else ptr = factory_(); 
        std::cout << "getConnection going" << std::endl;
        return std::unique_ptr<Conn, std::function<void(Conn*)>>(ptr, std::bind(takeBackConn, 
            std::weak_ptr<ConnectionPool>(this->shared_from_this()), deletor_, std::placeholders::_1));
    }

    ConnectionPool(ConnectionPool&) = delete;
    ConnectionPool& operator=(ConnectionPool&) = delete;

private:
    ConnectionPool(int active, long long expired, std::function<Conn*()> factory, std::function<void(Conn*)> deletor):
        active_(active), expired_(expired), deletor_(deletor), factory_(factory) {
            init();
    }

    static void takeBackConn(std::weak_ptr<ConnectionPool> weak_pool, std::function<void(Conn*)> deletor, Conn* conn) {
        std::shared_ptr<ConnectionPool> pool(weak_pool.lock());
        if (pool) {
            std::lock_guard<std::mutex> lock(pool->mtx);
            pool->conn_queue.push(conn);
        } else {
            deletor(conn);
        }
        std::cout << "takeBackConn going" << std::endl;
    }

    void init() {
        // 初始化连接池
        for (int i = 0; i < active_; ++i) {
            Conn* conn_ptr = factory_(); // 创建连接
            conn_queue.emplace(conn_ptr);
        }
        cur_active = active_;
    }

    int active_; // 连接池最少活跃连接数量
    int cur_active; // 当前连接池的连接数量
    int expired_; // (暂未实现)：没经过 expired_ 检查是否有多余的空闲连接，有则将其关闭
    std::function<void(Conn*)> deletor_; 
    std::function<Conn*()> factory_;
    std::mutex mtx;
    std::queue<Conn*> conn_queue;
};

}