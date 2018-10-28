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

#include "ff/farm.hpp"
#include "ff/node.hpp"

using namespace ff;

typedef enum { INCLUSIVE, EXCLUSIVE } scan_type;
typedef enum { REDUCE, EXSCAN, SCAN } comp_phase;

template <typename T>
class Scan {
public:
    // -------------------------------------- Constructors --------------------------------------

    Scan(const std::vector<T>& v, const std::function<T (T, T)>& f, const T& id, const scan_type t, const size_t par_deg) :
            scan_t(t), nw(par_deg)
    {
        if (v.empty())
            std::__throw_invalid_argument("Invalid argument: vector is empty.");
        if (!f)
            std::__throw_invalid_argument("Invalid argument: operation is null.");

        in = v; // Input vector.
        out = std::vector<T>(in.size()); // Output vector.

        op = f; // Associative operation.
        op_id = id; // Identity for the associative operation.
    }

    Scan(size_t par_deg) : nw(par_deg) { }

    // ---------------------------------------- Methods -----------------------------------------

    /*
     * Set the input vector.
     */
    void set_input(const std::vector<T>& v) {
        if (v.empty())
            std::__throw_invalid_argument("Invalid argument: vector is empty.");
        in = v;
        out = std::vector<T>(in.size());
    }

    /*
     * Set the binary associative operation and its identity value.
     */
    void set_operation(const std::function<T (T, T)>& f, const T& id) {
        if (!f)
            std::__throw_invalid_argument("Invalid argument: operation is null.");
        op = f;
        op_id = id;
    }

    /*
     * Set the scan type to be inclusive or exclusive.
     */
    void set_scan_type(const scan_type t) {
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

        } else if (nw > 0) { // Compute parallel scan.

            ff_Farm<> farm([&]() {
                std::vector<std::unique_ptr<ff_node>> workers;
                for(size_t i = 0; i < nw; ++i)
                    workers.push_back(make_unique<Worker>(in, out, op, op_id, scan_t, i));
                return workers;
            } () );

            // Replace the default emitter and add the feedback.
            Emitter distributor(farm.getlb(), in, op, op_id);
            farm.add_emitter(distributor);
            farm.remove_collector();
            farm.wrap_around();

            if (farm.run_and_wait_end() < 0) std::__throw_runtime_error("Running farm.");
        }
    }

private:
    // --------------------------------------- Subclasses ---------------------------------------

    struct Task {
        Task(size_t start, size_t stop) : start(start), stop(stop) {}

        const size_t start, stop;   // Partition range.
        comp_phase cp;              // Computation phase.
        T reduce_value;             // Reduce result on the partition [start, stop).
        size_t worker_id;           // Worker id.
    };

    // --------------------------------------- Variables ----------------------------------------

    std::vector<T> in;
    std::vector<T> out;

    std::function<T (T, T)> op;
    T op_id;
    scan_type scan_t;

    size_t nw; // (parallelism degree == 0) ? sequential : parallel

    // ------------------------------------ Auxiliary Methods -----------------------------------

    /*
     * Exclusive sequential scan:
     * given a sequence [ x0, x1, x2, ... ] calculate output [ id, y0, y1, y2, ... ]
     * such that y[0] = id and y[i] = y[i - 1] + x[i - 1] for each i > 0.
     * Complexity is O(n).
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
     * Complexity is O(n).
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

    struct Emitter: ff_node {

        Emitter(ff_loadbalancer* const lb,
                const std::vector<T>& in,
                const std::function<T (T, T)>& op,
                const T& op_id) :
                loadbalancer(lb),
                in(in),
                op(op),
                op_id(op_id) { };

        void* svc(void* t) {
            auto t_in = reinterpret_cast<Task*>(t);
            if (t_in == nullptr) {
                nw = loadbalancer->getNWorkers();
                th_count = 0;
                reduce_vec = std::vector<T>(nw);
                intervals.reserve(nw);

                // Create partitions.
                const size_t portion = in.size() / nw;
                ssize_t more = in.size() % nw;
                size_t start = 0, stop = 0;
                for (size_t i = 0; i < nw; ++i) {
                    start = stop;
                    stop = start + portion + (more > 0 ? 1 : 0);
                    --more;
                    intervals.emplace_back(start, stop);

                    // Send task containing the range of the partition of the input vector.
                    auto t = new Task(start, stop);
                    t->cp = REDUCE;                 // Computation phase.
                    t->reduce_value = op_id;
                    t->worker_id = i;               // Worker id.
                    loadbalancer->ff_send_out_to(t, i);
                }

            } else if (t_in->cp == EXSCAN) { // Wait for all reduce values from workers.
                if (++th_count < nw) {
                    reduce_vec.at(t_in->worker_id) = t_in->reduce_value;
                }
                else {
                    reduce_vec.at(t_in->worker_id) = t_in->reduce_value;

                    std::vector<T> temp(nw);    // Compute esclusive scan.
                    temp.at(0) = op_id;
                    for (size_t i = 1; i < nw; ++i) {
                        temp.at(i) = op(temp.at(i - 1), reduce_vec.at(i - 1));
                    }
                    reduce_vec = std::move(temp);

                    // Send to each worker the value to be used in the scan computation on its partition.
                    for (size_t i = 0; i < nw; ++i) {
                        auto t = new Task(intervals.at(i).first, intervals.at(i).second);
                        t->cp = SCAN;
                        t->reduce_value = reduce_vec.at(i);
                        t->worker_id = i;
                        loadbalancer->ff_send_out_to(t, i);
                    }
                    loadbalancer->broadcast_task(EOS);
                }
            }
            return GO_ON;
        }

        ff_loadbalancer* loadbalancer;
        std::vector<T> in;
        std::function<T (T, T)> op;
        T op_id;

        size_t nw;
        size_t th_count;
        std::vector<std::pair<size_t, size_t>> intervals;
        std::vector<T> reduce_vec;

    };

    struct Worker : ff_node {

        Worker(const std::vector<T>& in,
               std::vector<T>& out,
               const std::function<T (T, T)>& op,
               const T& op_id,
               const scan_type scan_t,
               const size_t w_id) :
                in(in),
                out(out),
                op(op),
                op_id(op_id),
                scan_t(scan_t),
                w_id(w_id) { };

        void* svc(void* t) {
            Task* t_in = reinterpret_cast<Task*>(t);
            if (t_in->cp == REDUCE) {   // Compute reduce on the partition.
                T reduce_value = op_id;
                std::for_each(std::begin(in) + t_in->start, std::begin(in) + t_in->stop,
                              [&](const T& val) {
                                  reduce_value = op(reduce_value, val);
                              });

                // Send reduce result to the distributor.
                t_in->cp = EXSCAN;
                t_in->reduce_value = reduce_value;
                t_in->worker_id = w_id;
                return t_in;

            } else if (t_in->cp == SCAN) {  // Compute scan on the partition.
                if (scan_t == EXCLUSIVE) {
                    out.at(t_in->start) = t_in->reduce_value;
                    for (size_t i = t_in->start + 1; i < t_in->stop; ++i) {
                        out.at(i) = op(out.at(i - 1), in.at(i - 1));
                    }
                } else {
                    out.at(t_in->start) = op(t_in->reduce_value, in.at(t_in->start));
                    for (size_t i = t_in->start + 1; i < t_in->stop; ++i) {
                        out.at(i) = op(out.at(i - 1), in.at(i));
                    }
                }
            }
            return GO_ON;
        }

        std::vector<T> in;
        std::vector<T>& out;
        std::function<T (T, T)> op;
        T op_id;
        size_t w_id;
        scan_type scan_t;
    };

};

#endif //SPM_PROJECT_SCAN_H
