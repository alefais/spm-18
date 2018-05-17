//
// Created by Alessandra Fais
// SPM course - MCSN - University of Pisa
// A.A. 2017/18
//

#ifndef SPM_HW2_CHANNEL_QUEUE_H
#define SPM_HW2_CHANNEL_QUEUE_H

#include <deque>
#include <mutex>
#include <condition_variable>

template <typename T>
class channel_queue {
private:
    std::mutex mux;
    std::condition_variable cond;
    std::deque<T> queue;
    bool ack = true;
    bool rdy = false;

public:
    bool getAck() {
        std::unique_lock<std::mutex> lock(mux);
        bool res = ack;
        return res;
    }

    void push(const T &value)  {
        {
            std::unique_lock<std::mutex> lock(mux);
            cond.wait(lock, [=] { return ack; });
            queue.push_front(value);
            rdy = true;
            ack = false;
        }
        cond.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(mux);
        cond.wait(lock, [=] { return rdy; });
        T rc(std::move(queue.back()));
        queue.pop_back();
        ack = true;
        rdy = false;
        cond.notify_one();
        return rc;
    }
};

#endif //SPM_HW2_CHANNEL_QUEUE_H
