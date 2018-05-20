//
// Created by Alessandra Fais
// SPM course - MCSN - University of Pisa
// A.A. 2017/18
//

#include <iostream>

#include "modules.h"

int main(int argc, char* argv[]) {

    if (argc < 5) {
        std::cerr << "Usage:"
                  << argv[0]
                  << " <number1> <number2> <nw> <chunk size par_for>"
                  << std::endl;
        return 1;
    }
    ull n1 = std::stoll(argv[1]);
    ull n2 = std::stoll(argv[2]);
    const size_t nw = std::stol(argv[3]);
    long chunk = std::stol(argv[4]);

    // sequential version
    if (sequential(n1, n2) == -1) return 1;

    // parallel version: master-worker schema with task grain 1 number in [n1, n2]
    if (master_worker(n1, n2, nw) == -1) return 1;

    // parallel version: master-worker schema with task grain 1 sub-interval of [n1, n2]
    if (master_worker_intervals(n1, n2, nw) == -1) return 1;

    // parallel version: parallel for
    if (parallel_for(n1, n2, chunk, nw) == -1) return 1;

    return 0;
}