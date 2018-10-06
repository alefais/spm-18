//
// Created by Alessandra Fais
// SPM project - MCSN - University of Pisa
// A.A. 2017/18
//

#ifndef SPM_PROJECT_OPERATIONS_H
#define SPM_PROJECT_OPERATIONS_H

#include <functional>
#include <string>

using namespace std;

typedef enum { INT_SUM, INT_MUL, DOUBLE_SUM, DOUBLE_MUL, STR_CONCAT } op_type;

/*
 * Identity values for operations.
 */
const int int_add_id = 0;
const double double_add_id = 0.0;
const int int_mul_id = 1;
const double double_mul_id = 1.0;
const string string_concat_id = "";

/*
 * Binary associative operations.
 */
plus<int> int_add;
plus<double> double_add;
multiplies<int> int_mul;
multiplies<double> double_mul;

string string_concat_op(const string& a, const string& b) {
    return a + b;
}
function<string (const string&, const string&)> string_concat = string_concat_op;

#endif //SPM_PROJECT_OPERATIONS_H
