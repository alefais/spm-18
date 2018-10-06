//
// Created by Alessandra Fais
// SPM project - MCSN - University of Pisa
// A.A. 2017/18
//

#ifndef SPM_PROJECT_SCAN_H
#define SPM_PROJECT_SCAN_H

#include "utils.h"

typedef enum { INCLUSIVE, EXCLUSIVE } scan_type;

template <class T> class Scan {
protected:
    std::vector<T> in;
    std::vector<T> out;
    size_t size;

    std::function<T(T,T)> op;
    T identity;

protected:
    Scan(std::vector<T>& v, std::function<T (T,T)> f, T id) {
        in = v;
        size = v.size();
        op = f;
        identity = id;
    }

public:
    /*
     * Set the input vector.
     */
    void set_in(std::vector<T>& v) {
        if (!is_power_of_two(v.size()))
            std::__throw_invalid_argument("Vector size is not a power of two!");
        in = v;
        size = v.size();
    }

    /*
     * Set the binary associative operation and its identity value.
     */
    void set_op(std::function<T(T,T)> f, T id) {
        op = f;
        identity = id;
    }

    /*
     * Get the input vector.
     */
    void get_in() {
        std::cout << "Input vector:\n";
        print(in);
    }

    /*
     * Get the output vector.
     */
    void get_out() {
        std::cout << "Output vector:\n";
        print(out);
    }
};

template <class T> class SequentialScan: public Scan<T> {
public:
    SequentialScan(std::vector<T>& v, std::function<T (T,T)> f, T id) : Scan<T>(v, f, id) {}

    void compute(scan_type t, int delay) {
        switch (t) {
            case EXCLUSIVE:
                compute_exclusive(delay);
                break;
            default:
                compute_inclusive(delay);
                break;
        }
    }

private:
    void compute_exclusive(int delay) {
        Scan<T>::out = seq_exclusive_scan<T>(Scan<T>::in, Scan<T>::size,
                                             Scan<T>::op, Scan<T>::identity, delay);
    }

    void compute_inclusive(int delay) {
        Scan<T>::out = seq_inclusive_scan<T>(Scan<T>::in, Scan<T>::size, Scan<T>::op, delay);
    }

};

template <class T> class ParallelScan: public Scan<T> {
public:
    size_t nw;
public:
    /*
     * When call constructor:
     * catch(std::invalid_argument& e) { std::cout << "Construction of scan failed" << std::endl; }
     */
    ParallelScan(std::vector<T>& v, std::function<T (T,T)> f, T id, size_t par_deg) : Scan<T>(v, f, id) {
        if (!is_power_of_two(v.size()))
            std::__throw_invalid_argument("Vector size is not a power of two!");
        nw = par_deg;
    }

    void compute(scan_type t, int delay) {
        switch(t) {
            case EXCLUSIVE:
                //compute_exclusive(delay);
                break;
            default:
                //compute_inclusive(delay);
                break;
        }
    }
};

#endif //SPM_PROJECT_SCAN_H

/*
 * Operations:
 * inclusive/exclusive, operator, identity, compute, set_input
 */