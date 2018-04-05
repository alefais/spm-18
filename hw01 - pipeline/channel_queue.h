//
// Created by Alessandra Fais
// SPM course - MCSN - University of Pisa
// A.A. 2017/18
//

#ifndef SPM_HW1_CHANNEL_QUEUE_H
#define SPM_HW1_CHANNEL_QUEUE_H

#include <deque>
#include <mutex>
#include <condition_variable>

template <typename T>
class channel_queue {
private:
    std::mutex              mux;
    std::condition_variable cond;
    std::deque<T>           queue;

public:
    void push(const T &value)  {
        {
            std::unique_lock<std::mutex> lock(mux);
            queue.push_front(value);
        }
        cond.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(mux);
        cond.wait(lock, [=] { return !queue.empty(); });
        T rc(std::move(queue.back()));
        queue.pop_back();
        return rc;
    }
};

#endif //SPM_HW1_CHANNEL_QUEUE_H
