//
// Created by Alessandra Fais
// SPM project - MCSN - University of Pisa
// A.A. 2017/18
//

#ifndef SPM_PROJECT_TEST_H
#define SPM_PROJECT_TEST_H

#include "scan.h"

std::vector<int> test_int_vector_gen(int size, int delay) {
    std::vector<int> v(size);
    for (int i = 0; i < size; ++i) {
        active_delay(delay);
        v.at(i) = i + 1;
    }
    return v;
}

std::vector<double> test_double_vector_gen(int size, int delay) {
    std::vector<double> v(size);
    double pi = 3.14159;
    for (int i = 0; i < size; ++i) {
        active_delay(delay);
        v.at(i) = (i + 1) * pi;
    }
    return v;
}

void test_int_seq_sum(int dim, int delay) {
    // Generate vector [ 1, 2, 3, ...]
    // Set operation = int addition
    std::cout << "Int sequential addition" << std::endl;

    //std::vector<int> in = {3, 1, 7, 0, 4, 1, 6, 3};
    //std::vector<int> in = {3, 5, 2, 7, 28, 4, 3, 0, 8, 1};
    std::vector<int> in = test_int_vector_gen(dim, delay);
    SequentialScan<int> seq_obj(in, int_add, int_add_id);
    seq_obj.get_in();

    seq_obj.compute(EXCLUSIVE, delay);
    seq_obj.get_out();

    seq_obj.compute(INCLUSIVE, delay);
    seq_obj.get_out();
}

void test_int_seq_mul(int dim, int delay) {
    // Generate vector [ 1, 2, 3, ...]
    // Set operation = integer multiplication
    std::cout << "Int sequential multiplication" << std::endl;

    std::vector<int> in = test_int_vector_gen(dim, delay);
    SequentialScan<int> seq_obj(in, int_mul, int_mul_id);
    seq_obj.get_in();

    seq_obj.compute(EXCLUSIVE, delay);
    seq_obj.get_out();

    seq_obj.compute(INCLUSIVE, delay);
    seq_obj.get_out();
}

void test_double_seq_sum(int dim, int delay) {
    // Generate vector [ 3.14159 6.28318 9.42477 12.5664 ...]
    // Set operation = double addition
    std::cout << "Double sequential addition" << std::endl;

    std::vector<double> in = test_double_vector_gen(dim, delay);
    SequentialScan<double> seq_obj(in, double_add, double_add_id);
    seq_obj.get_in();

    seq_obj.compute(EXCLUSIVE, delay);
    seq_obj.get_out();

    seq_obj.compute(INCLUSIVE, delay);
    seq_obj.get_out();
}

void test_double_seq_mul(int dim, int delay) {
    // Generate vector [ 3.14159 6.28318 9.42477 12.5664 ...]
    // Set operation = double multiplication
    std::cout << "Double sequential multiplication" << std::endl;

    std::vector<double> in = test_double_vector_gen(dim, delay);
    SequentialScan<double> seq_obj(in, double_mul, double_mul_id);
    seq_obj.get_in();

    seq_obj.compute(EXCLUSIVE, delay);
    seq_obj.get_out();

    seq_obj.compute(INCLUSIVE, delay);
    seq_obj.get_out();
}

void test_string_concat(int dim, int delay) {
    std::cout << "Sequential string concatenation" << std::endl;

    std::vector<std::string> in = { "zero", "uno", "due", "tre", "quattro", "cinque", "sei", "sette" };
    SequentialScan<std::string> seq_obj(in, string_concat, string_concat_id);
    seq_obj.get_in();

    seq_obj.compute(EXCLUSIVE, delay);
    seq_obj.get_out();

    seq_obj.compute(INCLUSIVE, delay);
    seq_obj.get_out();
}

void test_seq(op_type op, int dim, int delay) {
    switch (op) {
        case INT_SUM:
            test_int_seq_sum(dim, delay);
            break;
        case INT_MUL:
            test_int_seq_mul(dim, delay);
            break;
        case DOUBLE_SUM:
            test_double_seq_sum(dim, delay);
            break;
        case DOUBLE_MUL:
            test_double_seq_mul(dim, delay);
            break;
        case STR_CONCAT:
            test_string_concat(dim, delay);
        default:
            break;
    }
}

#endif //SPM_PROJECT_TEST_H
