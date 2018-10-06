//
// Created by Alessandra Fais
// SPM project - MCSN - University of Pisa
// A.A. 2017/18
//

#include "utils.h"

void active_delay(int msecs) {
    auto start = std::chrono::high_resolution_clock::now();
    bool end = false;
    while (!end) {
        auto now = std::chrono::high_resolution_clock::now();
        auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
        end = (msec > msecs);
    }
}

bool is_power_of_two(unsigned long x) {
    return (x != 0) && ((x & (x - 1)) == 0);
}