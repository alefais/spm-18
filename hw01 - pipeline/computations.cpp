//
// Created by Alessandra Fais
// SPM course - MCSN - University of Pisa
// A.A. 2017/18
//

#include <iostream>
#include <random>
#include <thread>
#include <vector>

#include "channel_queue.h"
#include "utils.h"
#include "computations.h"

void seq_comp(int m, int n, long iter) {
    for (int i = 0; i < m; ++i) {
        std::random_device random_device;
        std::mt19937 num_generator(random_device());
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        auto vec = new std::vector<double>(n);
        for (int i = 0; i < n; ++i) {
            (*vec).at(i) = dist(num_generator);
        }
        printElement((*vec), "main_gen");
        for (int i = 0; i < n; ++i) {
            double w = waste(iter, (*vec).at(i));
            (*vec).at(i) = ((*vec).at(i) + 1.0) * 2.0 + w;
        }
        printElement((*vec), "main_res");
    }
}

void par_comp_n(int m, int n, long w1, long w2) {
    channel_queue< std::vector<double>* > ch1;
    channel_queue< std::vector<double>* > ch2;
    channel_queue< std::vector<double>* > ch3;

    std::thread source(generateStream, std::ref(ch1), m, n);
    std::thread stage_p1(p1, std::ref(ch1), std::ref(ch2), w1); // P1 executes f1(x)
    std::thread stage_p2(p2, std::ref(ch2), std::ref(ch3), w2); // P2 executes f2(f1(x))
    std::thread sink(printResults, std::ref(ch3));

    source.join();
    stage_p1.join();
    stage_p2.join();
    sink.join();
}

void par_comp_one(int m, int n, long w1, long w2) {
    channel_queue< std::vector<double>* > ch1_;
    channel_queue< std::vector<double>* > ch2_;
    channel_queue< std::vector<double>* > ch3_;

    generateStream(ch1_, m, n);
    p1(ch1_, ch2_, w1);
    p2(ch2_, ch3_, w2);
    printResults(ch3_);
}