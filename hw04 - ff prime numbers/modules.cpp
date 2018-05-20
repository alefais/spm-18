//
// Created by Alessandra Fais
// SPM course - MCSN - University of Pisa
// A.A. 2017/18
//

#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <cstdlib>
#include <ff/farm.hpp>
#include <ff/parallel_for.hpp>

#include "utils.h"
#include "modules.h"

using namespace ff;


//----------------------------- sequential ---------------------------------------------

int sequential(ull n1, ull n2) {
    std::cout << "Sequential computation:\n";

    ffTime(START_TIME);

    std::vector<ull> results;
    results.reserve((size_t)(n2 - n1) / log(n1));

    ull prime = n1;
    while (prime <= n2) {
        if (is_prime(prime)) results.push_back(prime);
        prime++;
    }

    const size_t n = results.size();

    //print_results(n1, n2, results, n);

    ffTime(STOP_TIME);
    std::cout << "time: " << ffTime(GET_TIME) << " ms\n";

    return 0;
}


//--------------------- master-worker single number ------------------------------------

/*
 * The emitter sends one number to each worker and receives
 * back from workers only those numbers that are prime.
 */
struct Emitter: ff_node_t<ull, ull> {

    Emitter(ff_loadbalancer* const lb, ull n1, ull n2) :
            loadbalancer(lb), n1(n1), n2(n2) {
        results.reserve((size_t)(n2 - n1) / log(n1));
    }

    ull* svc(ull* x) {
        if (x == nullptr) {
            ull n = n1;
            while (n <= n2) {
                // cast to (void*) avoids to allocate memory (use the fact that a long int
                // is 8 bytes - instead of 4 - and it fits a pointer that is 8 bytes long in
                // a 64bit architecture); doing this is safe since we don't have value 0
                // (it's not a prime), otherwise a problem may arise since (void*)0 is NULL.
                ff_send_out((void *) n);
                n = ++n1;
            }
            loadbalancer->broadcast_task(EOS);
            return GO_ON; // keep emitter alive
        }
        results.push_back((ull)x);
        return GO_ON;
    }

    ff_loadbalancer* const loadbalancer;
    ull n1, n2;
    std::vector<ull> results;
};

/*
 * The worker sends back the number to the emitter only if it is prime.
 */
struct Worker: ff_node_t<ull, ull> {

    ull* svc(ull* input_task) {
        if (is_prime(reinterpret_cast<ull>(input_task))) {
            return input_task;
        }
        return GO_ON; // keep worker alive
    }
};

/*
 * Master-worker schema where the granularity of a task is one single number:
 * each worker only checks the primality of one number.
 */
int master_worker(ull n1, ull n2, const size_t nw) {
    std::cout << "\nMaster-worker schema1:\n";

    ffTime(START_TIME);

    ff_Farm<> farm([&]() {
                std::vector<std::unique_ptr<ff_node>> workers;
                for(size_t i = 0; i < nw; ++i) {
                    workers.push_back(make_unique<Worker>());
                }
                return workers;
            } () );

    // replace the default emitter and add the feedback
    Emitter emitter(farm.getlb(), n1, n2);
    farm.add_emitter(emitter);
    farm.remove_collector();
    farm.wrap_around();

    if (farm.run_and_wait_end() < 0) {
        error("running farm\n");
        return -1;
    }

    const std::vector<ull>& res = emitter.results;
    const size_t n = res.size();

    //print_results(n1, n2, res, n);

    std::cout << "time: " << ffTime(STOP_TIME) << " ms\n"
              << "farm time: " << farm.ffTime() << " ms\n";

    return 0;
}


//--------------------- master-worker partition ---------------------------------------

/*
 * Struct containing the start and end points of the sub-interval
 * that has to be processed by the worker and an array to fill with
 * the result (prime numbers in the given sub-interval [start, end]).
 */
struct Task {

    Task(ull n1, ull n2) : n1(n1), n2(n2) {};

    const ull n1, n2;
    std::vector<ull> v;
};

/*
 * The emitter creates partitions of the initial interval [n1, n2],
 * sends each partition to a worker and receives back from each worker
 * a std::vector<ull> (primes numbers in the sub-range).
 */
struct Emitter2: ff_node_t<Task> {

    Emitter2(ff_loadbalancer* const lb, ull _n1, ull _n2) :
        loadbalancer(lb), n1(_n1), n2(_n2) {
        results.reserve((size_t)(n2 - n1) / log(n1));
    }

    Task* svc(Task* input) {
        if (input == nullptr) {
            const size_t nw = loadbalancer->getNWorkers();
            const size_t size = (n2 - n1) / nw;
            ssize_t remained = (n2 - n1) % nw;
            ull begin = n1;
            ull end = n1;

            for (size_t i = 0; i < nw; ++i) {
                begin = end;
                end = begin + size + ((remained > 0) ? 1 : 0);
                --remained;

                Task* newtask = new Task(begin, end);
                loadbalancer->ff_send_out_to(newtask, i);
            }

            loadbalancer->broadcast_task(EOS);
            return GO_ON;
        }

        std::vector<ull>& vec = input->v;
        if (vec.size() > 0) {
            results.insert(std::upper_bound(results.begin(), results.end(), vec[0]), vec.begin(), vec.end());
        }
        delete(input);
        return GO_ON;
    }

    ff_loadbalancer* const loadbalancer;
    ull n1, n2;
    std::vector<ull> results;
};

/*
 * The worker computes sequentially on the received partition.
 */
struct Worker2: ff_node_t<Task> {

    Task* svc(Task* input) {
        std::vector<ull>& vec = input->v;
        ull begin = input->n1;
        ull end = input->n2;

        ull iter = begin;
        while (iter <= end) {
            if (is_prime(iter)) vec.push_back(iter);
            iter++;
        }

        //std::cout << "Worker" << get_my_id() << " found " << vec.size() << " primes\n";
        return input;
    }
};

/*
 * Master-worker schema where the granularity of a task is a range (sub-interval)
 * of the initial interval [n1, n2]: each worker computes sequentially the
 * number of prime numbers in the received sub-interval.
 */
int master_worker_intervals(ull n1, ull n2, const size_t nw) {
    std::cout << "\nMaster-worker schema2:\n";

    ffTime(START_TIME);

    ff_Farm<> farm([&]() {
                std::vector<std::unique_ptr<ff_node>> workers;
                for (size_t i = 0; i < nw; ++i) {
                    workers.push_back(make_unique<Worker2>());
                }
                return workers;
            } () );

    Emitter2 e(farm.getlb(), n1, n2);
    farm.add_emitter(e);
    farm.remove_collector();
    farm.wrap_around();

    if (farm.run_and_wait_end() < 0) {
        error("running farm\n");
        return -1;
    }

    const std::vector<ull>& res = e.results;
    const size_t n = res.size();

    //print_results(n1, n2, res, n);

    std::cout << "time: " << ffTime(STOP_TIME) << " ms\n"
              << "farm time: " << farm.ffTime() << " ms\n";

    return 0;
}


//----------------------------- parallel for ------------------------------------------

int parallel_for(ull n1, ull n2, long chunk, const size_t nw) {

    std::cout << "\nParallelFor:\n";

    std::vector<ull> results;
    results.reserve((size_t)(n2 - n1) / log(n1));
    std::vector<std::vector<ull>> res_ranges(nw);

    ffTime(START_TIME);

    ParallelFor par_for(nw);
    par_for.parallel_for_idx(n1, n2 + 1, 1, chunk,
        [&](const long begin, const long end, const long th_id) {
        for (long i = begin; i < end; ++i) {
            if (is_prime(i)) res_ranges[th_id].push_back(i);
        }
    });

    for (size_t i = 0; i < nw; ++i) {
        results.insert(std::upper_bound(results.begin(), results.end(), res_ranges[i][0]),
                        res_ranges[i].begin(), res_ranges[i].end());
    }

    std::sort(results.begin(), results.end(),
        [](const ull a, const ull b) { return a < b; });

    const size_t n = results.size();

    //print_results(n1, n2, results, n);

    ffTime(STOP_TIME);
    std::cout << "time: " << ffTime(GET_TIME) << " ms\n";

    return 0;
}