#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <thread>
#include <cassert>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>

class ThreadPool {
public:
    explicit ThreadPool(size_t threadCount = 8) : pool_(std::make_shared<Pool>()) {
        assert(threadCount > 0);
        // 创建threadCount个线程，每个线程执行Lambda表达式：循环检查任务队列，处理任务或等待新任务
        for(size_t i = 0; i < threadCount; i++) {
            std::thread([pool = pool_] {
                std::unique_lock<std::mutex> locker(pool->mtx);     // 上锁，线程全局生命周期
                while(true) {
                    if(!pool->tasks.empty()) {
                        auto task = std::move(pool->tasks.front());
                        pool->tasks.pop();
                        locker.unlock();        // 解锁，允许其他线程操作队列
                        task();                 // 执行任务
                        locker.lock();          // 重新上锁以检查队列
                    }
                    else if(pool->isClosed) break;
                    else pool->cond.wait(locker);   // 阻塞，等待任务队列唤醒
                }
            }).detach();
        }
    }

    ThreadPool() = default;
    ThreadPool(ThreadPool&&) = default;

    ~ThreadPool() {
        if(static_cast<bool>(pool_)) {
            {   // 作用域，控制锁的生命周期
                std::lock_guard<std::mutex> locker(pool_->mtx);
                pool_->isClosed = true;     // 关闭线程池
            }
            pool_->cond.notify_all();       // 唤醒所有等待线程
        }
    }

    // 任务队列添加任务
    template<class F>
    void AddTask(F&& task) {
        {
            std::lock_guard<std::mutex> locker(pool_->mtx);
            pool_->tasks.emplace(std::forward<F>(task));
        }
        pool_->cond.notify_all();
    }

private:
    struct Pool {
        std::mutex mtx;                             // 互斥锁
        std::condition_variable cond;               // 条件变量
        std::queue<std::function<void()>> tasks;    // 任务队列
        bool isClosed;                              // 线程池关闭标志
    };

    std::shared_ptr<Pool> pool_;
};

#endif  // THREADPOOL_H