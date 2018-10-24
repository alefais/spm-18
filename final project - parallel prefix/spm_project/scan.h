//
// Created by Alessandra Fais
// SPM project - MCSN - University of Pisa
// A.A. 2017/18
//

#ifndef SPM_PROJECT_SCAN_H
#define SPM_PROJECT_SCAN_H

#include <chrono>
#include <condition_variable>
#include <deque>
#include <functional>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>
#include <vector>

typedef enum { INCLUSIVE, EXCLUSIVE } scan_type;

template <typename T>
class Scan {
public:
    // -------------------------------------- Constructors --------------------------------------

    Scan(std::vector<T>& v, std::function<T (T, T)>& f, T id, scan_type t, size_t par_deg) :
            scan_t(t),
            nw(par_deg),
            barrier(par_deg + 1)
    {
        if (v.empty())
            std::__throw_invalid_argument("Invalid argument: vector is empty.");
        if (!f)
            std::__throw_invalid_argument("Invalid argument: operation is null.");

        in = v; // Input vector.
        out = std::vector<T>(in.size()); // Output vector.

        op = f; // Associative operation.
        op_id = id; // Identity for the associative operation.

        // Shared data structures for the parallel implementation.
        reduce_vec = std::vector<T>(par_deg);
        to_workers.reserve(par_deg);
        threads.reserve(par_deg + 1);
    }

    Scan(size_t par_deg) :
            nw(par_deg),
            barrier(par_deg + 1)
    {
        // Shared data structures for the parallel implementation.
        reduce_vec = std::vector<T>(par_deg);
        to_workers.reserve(par_deg);
        threads.reserve(par_deg + 1);
    }

    // ---------------------------------------- Methods -----------------------------------------

    /*
     * Set the input vector.
     */
    void set_input(std::vector<T>& v) {
        if (v.empty())
            std::__throw_invalid_argument("Invalid argument: vector is empty.");
        in = v;
        out = std::vector<T>(in.size());
    }

    /*
     * Set the binary associative operation and its identity value.
     */
    void set_operation(std::function<T (T, T)>& f, T id) {
        if (!f)
            std::__throw_invalid_argument("Invalid argument: operation is null.");
        op = f;
        op_id = id;
    }

    /*
     * Set the scan type to be inclusive or exclusive.
     */
    void set_scan_type(scan_type t) {
        scan_t = t;
    }

    /*
     * Get the input vector.
     */
    std::vector<T> get_input() {
        return in;
    }

    /*
     * Get the output vector.
     */
    std::vector<T> get_output() {
        return out;
    }

    /*
     * Print the input vector.
     */
    void print_in_vec() {
        std::cout << "Input vector:  " << print(in);
    }

    /*
     * Print the output vector.
     */
    void print_out_vec() {
        std::cout << "Output vector:  " << print(out);
    }

    /*
     * Compute the scan over the input vector:
     * if the parallelism degree is nw = 0 then a sequential scan is performed,
     * otherwise a parallel scan is executed with parallelism degree nw > 0.
     */
    void compute() {
        if (nw == 0) {
            // Compute sequential scan.
            (scan_t == EXCLUSIVE) ? seq_exclusive_scan() : seq_inclusive_scan();

        } else if (nw > 0) {

            // Create channels.
            for (size_t i = 0; i < nw; ++i) {
                to_workers.push_back(new Channel());
            }

            // Create parallel modules (scatter + gather module and workers).
            threads.emplace_back(&Scan::distributor, this);
            for (size_t i = 0; i < nw; ++i) {
                threads.emplace_back(&Scan::worker, this, i);
            }

        }
    }

    /*
     * If the parallelism degree is nw = 0 then this is a NOP.
     */
    void clean() {
        if (nw > 0) {
            for (auto &th : threads) {
                th.join();
            }
            for (size_t i = 0; i < nw; ++i) {
                delete (to_workers.at(i));
            }
        }
    }


private:
    // --------------------------------------- Subclasses ---------------------------------------

    struct Task {
        Task(size_t start, size_t stop) : start(start), stop(stop) {}

        const size_t start, stop;
    };

    class Channel {
    public:

        void push_task(const Task& t) {
            std::unique_lock<std::mutex> lock(mux);
            ch.push_front(t);
            cond.notify_one();
        }

        Task pop_task() {
            std::unique_lock<std::mutex> lock(mux);
            cond.wait(lock, [=] { return !ch.empty(); });

            Task t(std::move(ch.back()));
            ch.pop_back();

            return t;
        }

    private:
        std::mutex mux;
        std::condition_variable cond;
        std::deque<Task> ch;

    };

    class Barrier {
    public:

        explicit Barrier(size_t count) : th_count(count), init_threshold(count), generation_num(0) {}

        void wait() {
            std::unique_lock<std::mutex> lock(mux);
            auto actual_gen = generation_num;
            if (--th_count == 0) {
                generation_num++;
                th_count = init_threshold;
                cond.notify_all();
            }
            else
                cond.wait(lock, [this, actual_gen] { return actual_gen != generation_num; });
        }

    private:
        std::mutex mux;
        std::condition_variable cond;
        size_t th_count;
        size_t init_threshold;
        size_t generation_num;
    };

    // --------------------------------------- Variables ----------------------------------------

    std::vector<T> in;
    std::vector<T> reduce_vec;
    std::vector<T> out;

    std::function<T (T, T)> op;
    T op_id;
    scan_type scan_t;

    size_t nw; // (parallelism degree == 0) ? sequential : parallel

    Barrier barrier;
    std::vector<Channel*> to_workers;
    std::vector<std::thread> threads;

    // ------------------------------------ Auxiliary Methods -----------------------------------

    /*
     * Exclusive sequential scan:
     * given a sequence [ x0, x1, x2, ... ] calculate output [ id, y0, y1, y2, ... ]
     * such that y[0] = id and y[i] = y[i - 1] + x[i - 1] for each i > 0.
     * Complexity is O(n) (n additions for a vector of n elements).
     */
    void seq_exclusive_scan() {
        out.at(0) = op_id;
        for (size_t i = 1; i < in.size(); ++i) {
            out.at(i) = op(out.at(i - 1), in.at(i - 1));
        }
    }

    /*
     * Inclusive sequential scan:
     * given a sequence [ x0, x1, x2, ... ] calculate output [ y0, y1, y2, ... ]
     * such that y[0] = x[0] and y[i] = y[i - 1] + x[i] for each i > 0.
     * Complexity is O(n) (n additions for a vector of n elements).
     */
    void seq_inclusive_scan() {
        out.at(0) = op(op_id, in.at(0));
        for (size_t i = 1; i < in.size(); ++i) {
            out.at(i) = op(out.at(i - 1), in.at(i));
        }
    }

    /*
     * Generate a string that represents the content of the vector.
     */
    std::string print(const std::vector<T>& v) {
        std::stringstream ss;
        ss << "{ ";
        for (auto el : v) {
            ss << el << " ";
        }
        ss << "}\n";
        return ss.str();
    }

    // ------------------------------------ Parallel Modules ------------------------------------

    void distributor() {
        //std::cout << "DIST: partitioning vector...\n" << print(in); // TODO comment

        // Distribute vector chunks to workers.
        const size_t portion = in.size() / nw;
        ssize_t more = in.size() % nw;
        size_t start = 0, stop = 0;
        for (size_t i = 0; i < nw; ++i) {
            start = stop;
            stop = start + portion + (more > 0 ? 1 : 0);
            --more;

            Task t(start, stop);
            to_workers.at(i)->push_task(t);
        }

        // Wait on the barrier for the completion of the reduce phase of the workers.
        barrier.wait();

        //std::cout << "DIST: reduce vector " << print(reduce_vec); // TODO comment

        // Apply exclusive scan on the vector containing the reduce values.
        std::vector<T> temp(nw);
        temp.at(0) = op_id;
        for (size_t i = 1; i < nw; ++i) {
            temp.at(i) = op(temp.at(i - 1), reduce_vec.at(i - 1));
        }
        reduce_vec = std::move(temp);

        //std::cout << "DIST: reduce vector after ex-scan " << print(reduce_vec); // TODO comment

        // Wait on the barrier for the completion of the scan phase of the workers.
        barrier.wait();

        // NOP, synchronization only

        barrier.wait();

        //std::cout << "DIST: result computed!\n" << print(out); // TODO comment
    }

    void worker(size_t worker_id) {
        std::stringstream ss;

        // Pop a new task.
        Task t = to_workers.at(worker_id)->pop_task();

        // Read your partition of the input vector.
        //ss << "W" << worker_id << ": partition { ";
        //std::for_each(std::begin(in) + t.start, std::begin(in) + t.stop, // TODO comment
        //              [&](const T& val) { ss << val << " "; });

        // Evaluate reduce on your partition.
        T reduce_value = op_id;
        std::for_each(std::begin(in) + t.start, std::begin(in) + t.stop,
                      [&](const T& val) {
                          reduce_value = op(reduce_value, val);
                      });
        //ss << "} reduce value { " << reduce_value << " }\n"; // TODO comment
        //std::cout << ss.str();

        // Write the reduce value in the reduce vector.
        reduce_vec.at(worker_id) = reduce_value;

        barrier.wait();
        //ss = std::stringstream("");
        //ss << "W" << worker_id << " passes the barrier 1.\n";
        //std::cout << ss.str();

        // NOP, synchronization only

        barrier.wait();
        //ss = std::stringstream("");
        //ss << "W" << worker_id << " passes the barrier 2.\n";
        //std::cout << ss.str();

        if (scan_t == EXCLUSIVE) {
            out.at(t.start) = reduce_vec.at(worker_id);
            for (size_t i = t.start + 1; i < t.stop; ++i) {
                out.at(i) = op(out.at(i - 1), in.at(i - 1));
            }
        } else {
            out.at(t.start) = op(reduce_vec.at(worker_id), in.at(t.start));
            for (size_t i = t.start + 1; i < t.stop; ++i) {
                out.at(i) = op(out.at(i - 1), in.at(i));
            }
        }

        barrier.wait();
        //ss = std::stringstream("");
        //ss << "W" << worker_id << " passes the barrier 3.\n";
        //std::cout << ss.str();

        //ss = std::stringstream("");                         // TODO comment
        //ss << "W" << worker_id << " computation ended!\n";
        //std::cout << ss.str();

    }
};

#endif //SPM_PROJECT_SCAN_H
