//
// Created by Alessandra Fais
// SPM project - MCSN - University of Pisa
// A.A. 2017/18
//

#include <algorithm>
#include <random>
#include <string>

#include "scan.h"

const int add_op_id = 0;
const int mul_op_id = 1;
const std::string str_conc_id = "";

const int comp_delay = 1; // milliseconds

int add (int a, int b);
int mul (int a, int b);
std::string str_concat (std::string a, std::string b);

std::vector<int> generateIntVector(size_t size, unsigned int seed);
std::vector<std::string> generateStringVector(size_t size, unsigned int seed);
void active_delay(int msecs);

int main(int argc, char* argv[]) {

    // ------------------------------------- INPUT PARAMETERS -------------------------------------

    if (argc < 4) {
        std::cout << "Parallel Scan usage:\n"
                  << argv[0]
                  << " <vec_size> <type (e = exclusive, i = inclusive)> <nw> <seed> <test>\n"
                  << "Choose a test inserting the corresponding number:\n"
                  << "1. parallel prefix on a random vector of integers, associative operation is the addition\n"
                  << "2. parallel prefix on a random vector of integers, associative operation is the multiplication\n"
                  << "3. parallel prefix on a random vector of strings, associative operation is the concatenation\n";
        return 1;
    }

    size_t vec_size;
    std::istringstream is1(argv[1]);
    if (!(is1 >> vec_size) || vec_size < 1) {
        std::cerr << "Invalid vector size." << std::endl;
        return 1;
    }

    scan_type scan_t;
    switch (argv[2][0]) {
        case 'e':
            scan_t = EXCLUSIVE;
            break;
        default:
            scan_t = INCLUSIVE;
            break;
    }

    int nw;
    std::istringstream is2(argv[3]);
    if (!(is2 >> nw) || nw < 0) {
        std::cerr << "Invalid parallelism degree." << std::endl;
        return 1;
    }

    unsigned int seed;
    if (argc == 4) {
        std::random_device random_device;
        seed = random_device();
    } else {
        std::istringstream is3(argv[4]);
        if (!(is3 >> seed)) {
            std::cerr << "Invalid seed." << std::endl;
            return 1;
        }
    }

    int test = 1;
    if (argc == 6) {
        std::istringstream is4(argv[5]);
        if (!(is4 >> test) || test < 0 || test > 3) {
            std::cerr << "Invalid test number." << std::endl;
            return 1;
        }
    }

    std::cout << "Compute " << ((scan_t) ? "e-" : "i-")
              << "scan on " << vec_size
              << " elements with " <<  nw
              << " threads: ";

    if (test == 1) {

        // Measure the addition time.
        auto ta = std::chrono::high_resolution_clock::now();
        int a = 10, b = 100;
        int res = a + b;
        auto tb = std::chrono::high_resolution_clock::now();
        auto op_time = std::chrono::duration_cast<std::chrono::milliseconds>(tb - ta).count(); // ~ 0 milliseconds

        if (nw > 0) // Parallel execution: ~ (2 * vec_size/nw * (op_time + comp_delay)) ms
            std::cout << "expected time "
                      << (((vec_size / nw - 1) * (op_time + comp_delay)) +      // reduce in workers -> O(vec_size/nw)
                          ((nw - 1) * (op_time + comp_delay)) +                 // exclusive scan in distributor -> O(nw)
                          ((vec_size / nw - 1) * (op_time + comp_delay)))       // scan in workers -> O(vec_size/nw)
                      << " ms, real time ";
        else // Sequential execution.
            std::cout << "expected time "
                      << (vec_size - 1) * (op_time + comp_delay)
                      << " ms, real time ";

        // ------------------------------------------ TEST ADD ------------------------------------------
        std::vector<int> in1 = generateIntVector(vec_size, seed);
        std::function<int(int, int)> add_op = add;

        Scan<int> my_scan1(nw);
        my_scan1.set_input(in1);
        my_scan1.set_operation(add_op, add_op_id);
        my_scan1.set_scan_type(scan_t);

        auto t1 = std::chrono::high_resolution_clock::now();

        my_scan1.compute();
        my_scan1.clean();

        auto t2 = std::chrono::high_resolution_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

        std::cout << time << " ms\n";

    } else if (test == 2) {

        // Measure the multiplication time.
        auto ta = std::chrono::high_resolution_clock::now();
        int a = 10, b = 100;
        int res = a * b;
        auto tb = std::chrono::high_resolution_clock::now();
        auto op_time = std::chrono::duration_cast<std::chrono::milliseconds>(tb - ta).count(); // ~ 0 milliseconds

        if (nw > 0) // Parallel execution: ~ (2 * vec_size/nw * (op_time + comp_delay)) ms
            std::cout << "expected time "
                      << (((vec_size / nw - 1) * (op_time + comp_delay)) +      // reduce in workers -> O(vec_size/nw)
                          ((nw - 1) * (op_time + comp_delay)) +                 // exclusive scan in distributor -> O(nw)
                          ((vec_size / nw - 1) * (op_time + comp_delay)))       // scan in workers -> O(vec_size/nw)
                      << " ms, real time ";
        else // Sequential execution.
            std::cout << "expected time "
                      << (vec_size - 1) * (op_time + comp_delay)
                      << " ms, real time ";

        // ------------------------------------------ TEST MUL ------------------------------------------
        std::vector<int> in1 = generateIntVector(vec_size, seed);
        std::function<int(int, int)> mul_op = mul;

        Scan<int> my_scan2(in1, mul_op, mul_op_id, scan_t, nw);

        auto t3 = std::chrono::high_resolution_clock::now();

        my_scan2.compute();
        my_scan2.clean();

        auto t4 = std::chrono::high_resolution_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t3).count();

        std::cout << time << " ms\n";

    } else {

        // Measure the concatenation time.
        auto ta = std::chrono::high_resolution_clock::now();
        std::string c("string_one"), d("string_two");
        std::string str_res = c + d;
        auto tb = std::chrono::high_resolution_clock::now();
        auto op_time = std::chrono::duration_cast<std::chrono::milliseconds>(tb - ta).count(); // ~ 0 milliseconds

        if (nw > 0) // Parallel execution: ~ (2 * vec_size/nw * (op_time + comp_delay)) ms
            std::cout << "expected time "
                      << (((vec_size / nw - 1) * (op_time + comp_delay)) +      // reduce in workers -> O(vec_size/nw)
                          ((nw - 1) * (op_time + comp_delay)) +                 // exclusive scan in distributor -> O(nw)
                          ((vec_size / nw - 1) * (op_time + comp_delay)))       // scan in workers -> O(vec_size/nw)
                      << " ms, real time ";
        else // Sequential execution.
            std::cout << "expected time "
                      << (vec_size - 1) * (op_time + comp_delay)
                      << " ms, real time ";


        // ------------------------------------------ TEST STR ------------------------------------------
        std::vector<std::string> in2 = generateStringVector(vec_size, seed);
        std::function<std::string(std::string, std::string)> str_op = str_concat;

        Scan<std::string> my_scan3(in2, str_op, str_conc_id, scan_t, nw);

        auto t5 = std::chrono::high_resolution_clock::now();

        my_scan3.compute();
        my_scan3.clean();

        auto t6 = std::chrono::high_resolution_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(t6 - t5).count();

        std::cout << time << " ms\n";

    }

    return 0;

}

std::vector<int> generateIntVector(size_t size, unsigned int seed) {
    std::mt19937 num_generator(seed);
    std::uniform_int_distribution<int> dist(0, 50);
    std::vector<int> v;
    for (size_t i = 0; i < size; ++i) {
        v.push_back(dist(num_generator));
    }
    return v;
}

/*
 * Generate a vector of random strings picking ASCII characters with
 * decimal encoding in the range [33, 126], as shown in this table
 * https://en.cppreference.com/w/cpp/language/ascii
 */
std::vector<std::string> generateStringVector(size_t size, unsigned int seed) {
    std::mt19937 num_generator(seed);
    std::uniform_int_distribution<int> dist(33, 126);
    std::vector<std::string> v;
    int str_len = 3;
    for (size_t i = 0; i < size; ++i) {
        // create a string composed by the repetition str_len times
        // of the char encoded by the value of dist(num_generator)
        // in the ASCII encoding
        v.emplace_back(std::string(str_len, dist(num_generator)));
    }
    return v;
}

int add (int a, int b) {
    active_delay(comp_delay);
    return a + b;
}

int mul (int a, int b) {
    active_delay(comp_delay);
    return a * b;
}

std::string str_concat (std::string a, std::string b) {
    active_delay(comp_delay);
    return a + b;
}

/*
 * Add some delay in the function (for tests).
 */
void active_delay(int msecs) {
    auto start = std::chrono::high_resolution_clock::now();
    bool end = false;
    while (!end) {
        auto now = std::chrono::high_resolution_clock::now();
        auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
        end = (msec >= msecs);
    }
}