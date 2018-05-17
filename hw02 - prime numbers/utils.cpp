//
// Created by Alessandra Fais
// SPM course - MCSN - University of Pisa
// A.A. 2017/18
//

#include <iostream>
#include <vector>
#include <cmath>
#include <sstream>

#include "utils.h"

bool isPrime(int number) {
    if (number < 2)
        return false;
    if (number == 2)
        return true;
    if (number % 2 == 0)
        return false;
    for (int i = 3; i <= sqrt(number); i += 2) {
        if (number % i == 0)
            return false;
    }
    return true;
}

int interval_primes_seq(int task, result_collection& collection2) {
    std::stringstream s;
    int prime_count = 0;
    if (task > 1) {
        // Search for already computed tasks in collection2
        int last_task = collection2.get_biggest_computed(task);
        s << "current task: " << task << " last computed task: " << last_task << "\n";
        if (last_task == task) // task already computed by another worker
            return collection2.get_value(task);
        else {
            for (int x = last_task + 1; x <= task; x++) {
                if (isPrime(x)) {
                    prime_count++;
                }
            }
            if (last_task > 1) prime_count += collection2.get_value(last_task);
        }
        //std::cerr << s.str();
    }
    return prime_count;
}

std::vector<int>* sieveOfEratosthenes(int task) {
    std::vector<int>* prime = new std::vector<int>(task + 1, 1); // Initialize to 1 (true: assume all the numbers prime)
    prime->at(0) = 0; // 0 and 1 are not prime (set to 0: false)
    prime->at(1) = 0;

    // Set to 0 (false) all the non prime numbers
    int squareRoot = std::sqrt(task);
    for (int i = 2; i <= squareRoot; i++) {
        if (prime->at(i) == 1) {
            // Start j by the next multiple of i (that is: 2 * i), and increase it by i each time
            // until j is less than or equal to sqrt(task)
            for (int j = 2 * i; j <= task; j += i)
                prime->at(j) = 0;
        }
    }
    return prime;
}

int interval_primes_seq_opt(int task) {
    int prime_count = 0;
    std::vector<int>* primes = sieveOfEratosthenes(task);
    for (int i = 0; i <= task; i++) {
        if (primes->at(i) == 1) {
            prime_count++;
        }
    }
    return prime_count;
}

int usage(char* s) {
    std::cerr << "Usage: " << s << " TASK_NUM [optional] -nw PAR_DEGREE -d DELAY(#iterations)" << std::endl;
    return 1;
}

void waste(long iter, double x) {
    for (long i = 0; i < iter; ++i)
        x = sin(x);
    if (iter != 0)
        std::cerr << "wasting ... " << x << std::endl;
}