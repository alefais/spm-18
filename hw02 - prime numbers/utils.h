//
// Created by Alessandra Fais
// SPM course - MCSN - University of Pisa
// A.A. 2017/18
//

#ifndef SPM_HW2_UTILS_H
#define SPM_HW2_UTILS_H

#include "collection.h"

bool isPrime(int task);
std::vector<int>* sieveOfEratosthenes(int task);
int interval_primes_seq(int task, result_collection& collection2);
int interval_primes_seq_opt(int task);
int usage(char* s);
void waste(long iter, double x);

#endif //SPM_HW2_UTILS_H
