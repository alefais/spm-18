//
// Created by Alessandra Fais
// SPM course - MCSN - University of Pisa
// A.A. 2017/18
//

#include <iostream>
#include <sstream>
#include <vector>

#include "modules.h"
#include "utils.h"

void emitter(task_collection& collection1, std::vector< channel_queue<int>* >& ch, int nw, std::string& msg, long delay) {
    auto start = std::chrono::high_resolution_clock::now();

    std::string ic("Collection of tasks: ");
    collection1.printCollection(ic);

    int task = collection1.pop(delay);
    int i = 0;

    std::stringstream s;
    s << "+ Emitter -> tasks: [ ";
    while (task != -1) {
        int j = 0;
        while (!ch.at(j % nw)->getAck()) { j++; }
        ch.at(j % nw)->push(task);
        i++;
        //s << "(" << i << ", " << task << ") "; // prints tasks distributed by the emitter
        task = collection1.pop(delay);
    }

    for (i = 0; i < nw; ++i) {
        ch.at(i)->push(EOS);
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();

    s << "], distributed: " << collection1.get_distributed() << " tasks, completion time: " << time << " usecs\n";
    msg.append(s.str());
}

void worker_i(channel_queue<int>* ch_in, result_collection& collection2, worker_type mode, int debugID, std::string& msg) {
    auto start = std::chrono::high_resolution_clock::now();

    int task_count = 0;
    int task = ch_in->pop();
    int prime_count = 0;

    std::stringstream s;
    s << "+ Worker" << debugID << " -> tasks: [ ";
    while (task != EOS) {
        switch (mode) {
            case SEQUENTIAL:
                prime_count = interval_primes_seq(task, collection2);
                break;
            case SEQUENTIAL_OPTIMIZED:
                prime_count = interval_primes_seq_opt(task);
                break;
            case MAPREDUCE:

                break;
            default:
                break;
        }

        collection2.push(prime_count, task);
        task_count++;
        //s << "(" << task << ", " << prime_count << ") "; // prints tasks computed by each worker
        task = ch_in->pop();
    }
    s << "], computed: " << task_count << " tasks, ";

    auto stop = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();

    s << " completion time: " << time << " usecs\n";
    msg.append(s.str());
}