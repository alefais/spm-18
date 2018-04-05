//
// Created by Alessandra Fais
// SPM course - MCSN - University of Pisa
// A.A. 2017/18
//

#include <iostream>
#include <chrono>

#include "computations.h"

const long ITER = 10000000;
const long BALANCED = 5000000;
const long UNBALANCED = 3000000;

int main(int argc, char** argv) {

    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " STREAM_LEN VECTOR_LEN" << std::endl;
        return 1;
    }

    int m = atoi(argv[1]);
    int n = atoi(argv[2]);

    std::cout << "### Parallel computation: 4 threads (balanced stages)" << std::endl;
    auto start1 = std::chrono::high_resolution_clock::now();
    par_comp_n(m, n, BALANCED, BALANCED);
    auto stop1 = std::chrono::high_resolution_clock::now();
    auto time1 = std::chrono::duration_cast<std::chrono::microseconds>(stop1 - start1).count();
    std::cout << "### Tpar(n) = " << time1 << " usecs to traverse the pipeline (balanced stages).\n" << std::endl;

    std::cout << "### Parallel computation: 1 thread (balanced stages)" << std::endl;
    auto start2 = std::chrono::high_resolution_clock::now();
    par_comp_one(m, n, BALANCED, BALANCED);
    auto stop2 = std::chrono::high_resolution_clock::now();
    auto time2 = std::chrono::duration_cast<std::chrono::microseconds>(stop2 - start2).count();
    std::cout << "### Tpar(1) = " << time2 << " usecs to traverse the pipeline (balanced stages).\n" << std::endl;

    std::cout << "### Sequential computation" << std::endl;
    auto start3 = std::chrono::high_resolution_clock::now();
    seq_comp(m, n, ITER);
    auto stop3 = std::chrono::high_resolution_clock::now();
    auto time3 = std::chrono::duration_cast<std::chrono::microseconds>(stop3 - start3).count();
    std::cout << "### Tseq = " << time3 << " usecs to finish the computation.\n" << std::endl;

    std::cout << "### Parallel computation: 4 threads (unbalanced stages)" << std::endl;
    auto start4 = std::chrono::high_resolution_clock::now();
    par_comp_n(m, n, UNBALANCED, ITER - UNBALANCED);
    auto stop4 = std::chrono::high_resolution_clock::now();
    auto time4 = std::chrono::duration_cast<std::chrono::microseconds>(stop4 - start4).count();
    std::cout << "### Tpar(n) = " << time4 << " usecs to traverse the pipeline (unbalanced stages).\n" << std::endl;

    std::cout << "### Parallel computation: 1 thread (unbalanced stages)" << std::endl;
    auto start5 = std::chrono::high_resolution_clock::now();
    par_comp_one(m, n, UNBALANCED, ITER - UNBALANCED);
    auto stop5 = std::chrono::high_resolution_clock::now();
    auto time5 = std::chrono::duration_cast<std::chrono::microseconds>(stop5 - start5).count();
    std::cout << "### Tpar(1) = " << time5 << " usecs to traverse the pipeline (unbalanced stages).\n" << std::endl;

    double scalability1 = static_cast<double>(time2) / time1;    // S(n) = Tpar(1) / Tpar(n)
    double scalability2 = static_cast<double>(time5) / time4;
    double speedup1 = static_cast<double>(time3) / time1;        // Sp(n) = Tseq / Tpar(n)
    double speedup2 = static_cast<double>(time3) / time4;
    double efficiency1 = speedup1 / 4;                           // E(n) = Tid(n) / Tpar(n)
    double efficiency2 = speedup2 / 4;
    std::cout << "\n--------------------------------------------------\n"
              << "Computation of a stream of " << m
              << " vectors of " << n
              << " items."
              << "\n--------------------------------------------------\n";
    std::cout << "-----------------------------------------\n"
              << "Final results (balanced version):"
              << "\n-----------------------------------------\n"
              << "Tpar(n) = " << time1 << " usecs\n"
              << "Tpar(1) = " << time2 << " usecs\n"
              << "Tseq = " << time3 << " usecs\n"
              << "S(n) = " << scalability1
              << "\nE(n) = " << efficiency1
              << "\nSp(n) = " << speedup1
              << std::endl;
    std::cout << "\n-----------------------------------------\n"
              << "Final results (unbalanced version):"
              << "\n-----------------------------------------\n"
              << "Tpar(n) = " << time4 << " usecs\n"
              << "Tpar(1) = " << time5 << " usecs\n"
              << "Tseq = " << time3 << " usecs\n"
              << "S(n) = " << scalability2
              << "\nE(n) = " << efficiency2
              << "\nSp(n) = " << speedup2
              << std::endl << std::endl;
    return 0;
}

