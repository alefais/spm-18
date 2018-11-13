//
// Created by Alessandra Fais
// SPM course - MCSN - University of Pisa
// A.A. 2017/18
//

#include <iostream>
#include <vector>

#include "utils.h"

// Primality test
bool is_prime(unsigned long long number) {
    if (number < 4) return (number > 1);

    if (number % 2 == 0 || number % 3 == 0) return false;

    for (unsigned long long i = 5; (i * i) <= number; i += 6) {
        if (number % i == 0 || number % (i + 2) == 0)
            return false;
    }

    return true;
}

void print_results(ull n1, ull n2, const std::vector<ull>& res, const size_t n) {
    std::cout << "Found " << n << " primes in [" << n1 << ", " << n2 << "]" << std::endl;
    std::cout << "Primes: [ ";
    for (size_t i = 0; i < n; ++i) {
        std::cout << res[i] << " ";
    }
    std::cout << "]\n";
}