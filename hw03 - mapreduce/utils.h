//
// Created by Alessandra Fais on 23/09/2018.
//

#ifndef SPM_HW03_UTILS_H
#define SPM_HW03_UTILS_H

#include <string>

using namespace std;
using ull = unsigned long long;

enum print_flag { partial_results, final_results };

bool pairs_sort(const pair<string, int>* p1, const pair<string, int>* p2);
void print_results(char* of, vector< pair<string, int>* >& v, ull tc, print_flag f);

#endif //SPM_HW03_UTILS_H
