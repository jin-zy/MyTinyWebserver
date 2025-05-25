#ifndef BLOCKQUEUE_H
#define BLOCKQUEUE_H

#include <deque>
#include <mutex>
#include <condition_variable>
#include <cassert>
#include <sys/time.h>

/**
 * 线程安全的有界阻塞队列，支持多线程生产者/消费者模型
 */
template<class T>
class BlockDeque {
public:
    explicit BlockDeque(size_t MaxCapacity = 1000);
    ~BlockDeque();

    void Close();
    void flush();

    void clear();

    bool empty();
    bool full();
    size_t size();
    size_t capacity();

    T front();
    T back();

    void push_back(const T &item);
    void push_front(const T &item);
    bool pop(T &item);
    bool pop(T &item, int timeout);

private:
    std::deque<T> deq_;                     // 双端队列容器
    size_t capacity_;                       // 队列容量
    bool isClose_;                          // 队列关闭标志
    std::mutex mtx_;                        // 互斥锁
    std::condition_variable condConsumer_;  // 消费者条件变量
    std::condition_variable condProducer_;  // 生产者条件变量

};

/** 构造函数，初始化容量和关闭标志 */
template<class T>
BlockDeque<T>::BlockDeque(size_t MaxCapacity) :capacity_(MaxCapacity) {
    assert(MaxCapacity > 0);
    isClose_ = false;
}

/** 析构函数，自动关闭队列 */
template<class T>
BlockDeque<T>::~BlockDeque() {
    Close();
};

/** 关闭队列，清空数据并唤醒所有等待线程 */
template<class T>
void BlockDeque<T>::Close() {
    {   
        std::lock_guard<std::mutex> locker(mtx_);
        deq_.clear();
        isClose_ = true;
    }
    condProducer_.notify_all();
    condConsumer_.notify_all();
};

/** 唤醒一个等待的消费者线程 */
template<class T>
void BlockDeque<T>::flush() {
    condConsumer_.notify_one();
};

/** 清空队列 */
template<class T>
void BlockDeque<T>::clear() {
    std::lock_guard<std::mutex> locker(mtx_);
    deq_.clear();
}

/** 判断队列是否为空 */
template<class T>
bool BlockDeque<T>::empty() {
    std::lock_guard<std::mutex> locker(mtx_);
    return deq_.empty();
}

/** 判断队列是否已满 */
template<class T>
bool BlockDeque<T>::full(){
    std::lock_guard<std::mutex> locker(mtx_);
    return deq_.size() >= capacity_;
}

/** 获取队列当前元素个数 */
template<class T>
size_t BlockDeque<T>::size() {
    std::lock_guard<std::mutex> locker(mtx_);
    return deq_.size();
}

/** 获取队列的最大容量 */
template<class T>
size_t BlockDeque<T>::capacity() {
    std::lock_guard<std::mutex> locker(mtx_);
    return capacity_;
}

/** 获取队首元素 */
template<class T>
T BlockDeque<T>::front() {
    std::lock_guard<std::mutex> locker(mtx_);
    return deq_.front();
}

/** 获取队尾元素 */
template<class T>
T BlockDeque<T>::back() {
    std::lock_guard<std::mutex> locker(mtx_);
    return deq_.back();
}

/** 从队尾插入元素 */
template<class T>
void BlockDeque<T>::push_back(const T &item) {
    std::unique_lock<std::mutex> locker(mtx_);
    // 队列已满，生产者线程阻塞等待
    while(deq_.size() >= capacity_) {
        condProducer_.wait(locker);
    }
    deq_.push_back(item);
    // 唤醒一个等待的消费者线程
    condConsumer_.notify_one();
}

/** 从队首插入元素 */
template<class T>
void BlockDeque<T>::push_front(const T &item) {
    std::unique_lock<std::mutex> locker(mtx_);
    while(deq_.size() >= capacity_) {
        condProducer_.wait(locker);
    }
    deq_.push_front(item);
    condConsumer_.notify_one();
}

/** 从队首弹出元素 */
template<class T>
bool BlockDeque<T>::pop(T &item) {
    std::unique_lock<std::mutex> locker(mtx_);
    // 队列为空，消费者线程阻塞等待
    while(deq_.empty()){
        condConsumer_.wait(locker);
        // 队列已关闭，返回false
        if(isClose_){
            return false;
        }
    }
    // 从队首获取元素
    item = deq_.front();
    // 移除队首元素
    deq_.pop_front();
    // 唤醒一个等待的生产者线程
    condProducer_.notify_one();
    return true;
}

/** 从队首弹出元素，支持超时机制 */
template<class T>
bool BlockDeque<T>::pop(T &item, int timeout) {
    std::unique_lock<std::mutex> locker(mtx_);
    while(deq_.empty()){
        // 等待指定时间，如果超时返回false
        if(condConsumer_.wait_for(locker, std::chrono::seconds(timeout)) 
                == std::cv_status::timeout){
            return false;
        }
        // 队列已关闭，返回false
        if(isClose_){
            return false;
        }
    }
    item = deq_.front();
    deq_.pop_front();
    condProducer_.notify_one();
    return true;
}

#endif  // BLOCKQUEUE_H