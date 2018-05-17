//
// Created by Alessandra Fais
// SPM course - MCSN - University of Pisa
// A.A. 2017/18
//

#include <iostream>
#include <thread>
#include <sstream>

#include "channel_queue.h"
#include "modules.h"
#include "utils.h"

void sequential_comp(int ntasks, int nw, long delay, worker_type mode) {
    auto collection1 = new task_collection(ntasks); // collection of tasks
    auto collection2 = new result_collection(ntasks, -1); // collection for the results

    auto start = std::chrono::high_resolution_clock::now();

    // create nw synchronous channels between emitter and workers
    auto ch_tasks = new std::vector< channel_queue<int>* >(nw);
    for (auto& ch : *ch_tasks) {
        ch = new channel_queue<int>();
    }

    auto msg = new std::vector<std::string>(nw + 2);
    std::vector<std::thread> threads;
    threads.push_back(std::thread(emitter, std::ref(*collection1), std::ref(*ch_tasks), nw, std::ref(msg->at(0)), delay));
    for (int i = 0; i < nw; ++i) {
        threads.push_back(std::thread(worker_i, ch_tasks->at(i), std::ref(*collection2), mode, i, std::ref(msg->at(i + 1))));
    }

    for (auto& th : threads) {
        th.join();
    }

    std::stringstream temp;
    if (ntasks == collection2->get_done()) {
        temp << "All " << collection2->get_done() << "/" << ntasks << " tasks have been correctly computed.\n";
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();

    if (mode == 0)
        temp << "Computation sequential finished in " << time << " usecs\n";
    else if (mode == 1)
        temp << "Computation sequential optimized finished in " << time << " usecs\n";

    msg->at(nw + 1).append(temp.str());
    for (const std::string s : *msg) {
        std::cout << s;
    }

    std::string fc;
    fc.append("Collection of results (pairs <task, #prime_num>):\n");
    collection2->printResults(fc);

    delete(collection1);
    delete(collection2);
    delete(ch_tasks);
}

int main(int argc, char* const argv[]) {

    int hw_th = std::thread::hardware_concurrency();
    int n_tasks = 0; // total number of tasks
    int nw = 0;
    long delay = 0;

    if (argc == 2) {
        n_tasks = atoi(argv[1]);
        nw = hw_th;
    } else if (argc == 4) {
        n_tasks = atoi(argv[1]);

        if (argv[2] == std::string("-nw")) {
            int par_deg = atoi(argv[3]);
            nw = (par_deg > 0) ? std::min(par_deg, hw_th) : hw_th;
        } else if (argv[2] == std::string("-d")) {
            delay = atoi(argv[3]);
            nw = hw_th;
        } else
            return usage(argv[0]);
    } else if (argc == 6) {
        n_tasks = atoi(argv[1]);

        if (argv[2] == std::string("-nw")) {
            int par_deg = atoi(argv[3]);
            nw = (par_deg > 0) ? std::min(par_deg, hw_th) : hw_th;
        } else
            return usage(argv[0]);

        if (argv[4] == std::string("-d")) {
            delay = atoi(argv[5]);
        } else
            return usage(argv[0]);
    }
    else
        return usage(argv[0]);

    std::cout << "Executing with parallelism degree nw = " << nw <<
              " (total number of hardware threads is " << hw_th << ")" << std::endl;
    std::cout << "Number of tasks in the initial set: " << n_tasks << std::endl;
    std::cout << "Delay set to " << delay << " iterations" << std::endl;


    std::cout << "\n--------------------------------------------------\n"
              << "Sequential computation" << std::endl;
    sequential_comp(n_tasks, nw, delay, SEQUENTIAL); // evaluates isPrime(x) for each x in [last_comp_task, my_task]
    std::cout << "\n--------------------------------------------------\n"
                 << "Sequential computation optimized" << std::endl;
    sequential_comp(n_tasks, nw, delay, SEQUENTIAL_OPTIMIZED); // uses Sieve Of Eratosthenes algorithm
    return 0;
}