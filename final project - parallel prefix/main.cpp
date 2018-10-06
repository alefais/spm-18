//
// Created by Alessandra Fais
// SPM project - MCSN - University of Pisa
// A.A. 2017/18
//

#include <cstring>
#include <iostream>

#include "operations.h"
#include "test.h"

int main(int argc, char* argv[]) {
    if(argc < 6) {
        std::cout << "Parallel Scan Framework usage: \n"
                  << "pick an operation between: int+ int* double+ double* str_concat (default: int+)\n"
                  << "delays are in milliseconds\n\n"
                  << "Please invoke the program with the following parameters:\n"
                  << argv[0]
                  << " <vector_size> <op> <delay_ms_calc> <delay_ms_dist> <par_deg>"
                  << std::endl;
        return 0;
    }

    // Vector size.
    unsigned long size = atol(argv[1]);

    // Associative operation.
    op_type op = INT_SUM;
    if (strcmp(argv[2], "int+") == 0)
        op = INT_SUM;
    else if (strcmp(argv[2], "int*") == 0)
        op = INT_MUL;
    else if (strcmp(argv[2], "double+") == 0)
        op = DOUBLE_SUM;
    else if (strcmp(argv[2], "double*") == 0)
        op = DOUBLE_MUL;
    else if (strcmp(argv[2], "str_concat") == 0)
        op = STR_CONCAT;

    // Calculation and communication delays and times.
    int delay_calc = atoi(argv[3]);
    int delay_comm = atoi(argv[4]);
    int t_calc = 1 + delay_calc; // approx_op_time + delay_calc
    int t_gather = 0;

    // Parallelism degree.
    int nw = atoi(argv[5]);

    // Time required for (size - 1) sequential operations.
    std::cout << "Expected sequential completion time:  "
              << (size - 1) * t_calc << " msecs approx." << std::endl;

    // Sequential test.
    test_seq(op, size, delay_calc);

    // Completion time given by the scatter distribution of the vector,
    // the sequential computation of each worker on its portion of the vector,
    // the gather (and final reduce) time.
    std::cout << "Expected parallel completion time:  "
              << ( (nw * delay_comm) + ((size/nw - 1) * t_calc) + (t_gather + delay_comm) )
              << " msecs approx." << std::endl;

    return 0;
}