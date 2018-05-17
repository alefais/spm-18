//
// Created by Alessandra Fais
// SPM course - MCSN - University of Pisa
// A.A. 2017/18
//

#include <vector>
#include <iostream>
#include <thread>
#include <algorithm>
#include <random>
#include <sstream>

#include "collection.h"
#include "utils.h"

collection::collection(int init_size, int init_value) :
    coll(init_size, init_value),
    n_tasks(init_size) {}

task_collection::task_collection(int init_size) :
    collection(init_size, 0),
    pick(0) {
    std::random_device random_device;
    std::mt19937 num_generator(random_device());
    std::uniform_int_distribution<int> dist(1, MAX_TASK);
    for (int& item : coll) {
        item = dist(num_generator);
    }
}

int task_collection::pop(long iter) {
    int task;
    waste(iter, 1);
    if (pick < n_tasks) {
        task = coll.at(pick);
        pick++;
        return task;
    }
    return -1;
}

int task_collection::get_distributed() {
    return pick;
}

void task_collection::printCollection(std::string& name) {
    std::stringstream s;
    s << name << "[ ";
    for (const int& item : coll) {
        s << item << " ";
    }
    s << "]";
    std::cout << s.str() << std::endl;
}

result_collection::result_collection(int init_size, int init_value) :
    collection(init_size, init_value),
    res(init_size, init_value),
    done_tasks(0) {
}

void result_collection::push(const int& prime_num, int task) {
    std::unique_lock<std::mutex> lock(mux);
    coll.at(done_tasks)= task;        // store the task
    res.at(done_tasks) = prime_num;    // store the count of primes for the specific task
    done_tasks++;
}

int result_collection::get_value(int task) {
    if (task >= 1 && task <= MAX_TASK) {
        ptrdiff_t pos = std::distance(coll.begin(), std::find(coll.begin(), coll.end(), std::ref(task)));
        if (pos < n_tasks)
            return res.at(pos);
    }
    return -1;
}

/**
 * Checks if mytask has already been computed by someone else (the value stored in the
 * collection at index mytask has to be different from -1); if not search for the biggest
 * task smaller than mytask that has already been computed.
 * @param mytask
 * @return ID of the biggest task smaller than mytask that has already been computed, -1
 * if no task has already been computed
 */
int result_collection::get_biggest_computed(int mytask) {
    int task_comp = 1;
    if (mytask >= 1 && mytask <= MAX_TASK) {
        if (get_value(mytask) != -1) {
            task_comp = mytask;
        } else {
            ptrdiff_t pos = std::distance(coll.begin(), find(coll.begin(), coll.end(), std::ref(mytask)));
            if (pos < n_tasks) {
                int max = 0;
                for (long int i = pos - 1; i > 0; i--) {
                    int cur = coll.at(i);
                    if (cur < mytask && cur > max)
                        max = cur;
                }
                if (max != 0)
                    task_comp = max;
            }
        }
    }
    return task_comp;
}

int result_collection::get_done() {
    return done_tasks;
}

void result_collection::printResults(std::string& name) {
    std::stringstream s;
    s << name << "[ ";
    for (int i = 0; i < n_tasks; i++) {
        s << "(" << coll.at(i) << ", " << res.at(i) << ") ";
    }
    s << "]";
    std::cout << s.str() << std::endl;
}
