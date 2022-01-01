//
// Created by bo on 2021/12/21.
//

#ifndef AVPLAYER_THREADSAFEQUEUE_H
#define AVPLAYER_THREADSAFEQUEUE_H

#include <string>
#include <queue>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

template<class T>
class ThreadSafeQueue {
public:
    ThreadSafeQueue(uint8_t maxSize) : current_size(0), max_size(maxSize) {
    }

    size_t size() { return m_container.size(); }


    void enqueue(T item) {
        std::unique_lock<std::mutex> lock(m_mutex);
        //只要队列没放满，就可以继续放数据
        m_cond_not_full.wait(lock, [this]{return current_size < max_size;});

        m_container.push(item);
        ++current_size;
        lock.unlock();
        m_cond_has_item.notify_one();
    }

    void dequeue(T &item) {
        std::unique_lock<std::mutex> lock(m_mutex);
        //只要队列不为空，就可以取数据
        m_cond_has_item.template wait(lock, [this]{return !m_container.empty();});

        item = m_container.front();
        m_container.pop();
        --current_size;
        lock.unlock();
        m_cond_not_full.notify_one();
    }

    void getFront(T &item) {
        std::unique_lock<std::mutex> lock(m_mutex);
        //只要队列不为空，就可以取数据
        m_cond_has_item.template wait(lock, [this]{return !m_container.empty();});

        item = m_container.front();
        lock.unlock();
    }

    size_t current_size;
private:
    uint8_t max_size;
    std::queue<T> m_container;
    std::mutex m_mutex;
    std::condition_variable m_cond_has_item;
    std::condition_variable m_cond_not_full;
};

#endif //AVPLAYER_THREADSAFEQUEUE_H
