//
// Created by Alessandra Fais
// SPM project - MCSN - University of Pisa
// A.A. 2017/18
//

#ifndef SPM_PROJECT_UTILS_H
#define SPM_PROJECT_UTILS_H

#include <chrono>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

void active_delay(int msecs);
bool is_power_of_two(unsigned long x);

/*
 * Exclusive sequential scan:
 * given a sequence [ x0, x1, x2, ... ] calculate output [ id, y0, y1, y2, ... ]
 * such that y[0] = id and y[i] = y[i - 1] + x[i - 1] for each i > 0.
 * Complexity is O(n) (n additions for a vector of n elements).
 */
template <typename T> std::vector<T> seq_exclusive_scan(
        const std::vector<T>& in, size_t size,
        std::function<T (T,T)>& op, T id,
        int delay) {

    auto start = std::chrono::high_resolution_clock::now();

    std::vector<T> out(size);
    out.at(0) = id;
    std::cout << "Input vector size is " << in.size() << ".\n";
    for (unsigned long i = 1; i < size; ++i) {
        active_delay(delay);
        out.at(i) = op(out.at(i - 1), in.at(i - 1));
    }

    auto now = std::chrono::high_resolution_clock::now();
    auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();

    std::cerr << "Seq Exclusive Scan time " << msec << " msecs.\n";

    return out;
}

/*
 * Inclusive sequential scan:
 * given a sequence [ x0, x1, x2, ... ] calculate output [ y0, y1, y2, ... ]
 * such that y[0] = x[0] and y[i] = y[i - 1] + x[i] for each i > 0.
 * Complexity is O(n) (n additions for a vector of n elements).
 */
template <typename T> std::vector<T> seq_inclusive_scan(
        const std::vector<T>& in, size_t size,
        std::function<T (T,T)>& op,
        int delay) {

    auto start = std::chrono::high_resolution_clock::now();

    std::vector<T> out(size);
    out.at(0) = in.at(0);
    for (unsigned long i = 1; i < size; ++i) {
        active_delay(delay);
        out.at(i) = op(out.at(i - 1), in.at(i));
    }

    auto now = std::chrono::high_resolution_clock::now();
    auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();

    std::cerr << "Seq Inclusive Scan time " << msec << " msecs.\n";

    return out;
}

template <typename T> void print(const std::vector <T>& v) {
    std::stringstream ss;
    ss << "{ ";
    for (auto el : v) {
        ss << el << " ";
    }
    ss << "}\n";
    std::cout << ss.str();
}


#endif //SPM_PROJECT_UTILS_H
