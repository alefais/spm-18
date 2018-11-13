//
// Created by Alessandra Fais
// SPM course - MCSN - University of Pisa
// A.A. 2017/18
//

#ifndef SPM_HW04_MODULES_H
#define SPM_HW04_MODULES_H

using ull = unsigned long long;

int sequential(ull n1, ull n2);
int master_worker(ull n1, ull n2, const size_t nw);
int master_worker_intervals(ull n1, ull n2, const size_t nw);
int parallel_for(ull n1, ull n2, long chunk, const size_t nw);

#endif //SPM_HW04_MODULES_H
