//
// Created by Alessandra Fais
// SPM course - MCSN - University of Pisa
// A.A. 2017/18
//

#include <iostream>

#include "computations.h"

using namespace std;
using ull = unsigned long long;

int main(int argc, char* const argv[]) {

    if (argc < 4) {
        cerr << "Usage: "
             << argv[0]
             << " <input_file> <output_file> <nw>"
             << std::endl;
        return 1;
    }

    ull nw = atoi(argv[3]);

    sequential(argv[1], argv[2]);

    return 0;
}

