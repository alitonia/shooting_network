
#ifndef NETWORK_CLIENT_SHARED_QUEUE_CPP
#define NETWORK_CLIENT_SHARED_QUEUE_CPP

#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class SharedQueue {
public:
    SharedQueue();

    ~SharedQueue();

    T &front();

    void pop_front();

    void push_back(const T &item);

    void push_back(T &&item);

    int size();

    bool empty();

    void push(const T &item);

    T pop();

private:
    std::deque<T> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

template<typename T>
SharedQueue<T>::SharedQueue() {}

template<typename T>
SharedQueue<T>::~SharedQueue() {}

template<typename T>
T &SharedQueue<T>::front() {
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.empty()) {
        cond_.wait(mlock);
    }
    return queue_.front();
}

template<typename T>
void SharedQueue<T>::pop_front() {
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.empty()) {
        cond_.wait(mlock);
    }
    queue_.pop_front();
}

template<typename T>
void SharedQueue<T>::push_back(const T &item) {
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push_back(item);
    mlock.unlock();     // unlock before notification to minimize mutex con
    cond_.notify_one(); // notify one waiting thread

}

template<typename T>
void SharedQueue<T>::push_back(T &&item) {
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push_back(std::move(item));
    mlock.unlock();     // unlock before notification to minimize mutex con
    cond_.notify_one(); // notify one waiting thread

}

template<typename T>
int SharedQueue<T>::size() {
    std::unique_lock<std::mutex> mlock(mutex_);
    int size = queue_.size();
    mlock.unlock();
    return size;
}

template<typename T>
bool SharedQueue<T>::empty() {
    return queue_.empty();
}

template<typename T>
void SharedQueue<T>::push(const T &item) {
    SharedQueue<T>::push_back(item);
}


template<typename T>
T SharedQueue<T>::pop() {
    auto v = SharedQueue<T>::front();
    SharedQueue<T>::pop_front();
    return v;
}

#endif //NETWORK_CLIENT_SHARED_QUEUE_CPP
