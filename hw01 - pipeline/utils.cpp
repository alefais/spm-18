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

std::vector<double>* generateStreamItem(int n) {
    std::random_device random_device;
    std::mt19937 num_generator(random_device());
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    auto vec = new std::vector<double>(n);
    for (int i = 0; i < n; ++i) {
        (*vec).at(i) = dist(num_generator);
    }
    printElement((*vec), "source");
    return vec;
}

void generateStream(channel_queue< std::vector<double>* >& ch1, int m, int n) {
    for (int i = 0; i < m; ++i) {
        ch1.push(generateStreamItem(n));
    }
    ch1.push(nullptr);
}

void addOne(std::vector<double>& element, long iter) {
    for (int i = 0; i < element.size(); ++i) {
        double w = waste(iter, element[i]);
        element[i] = element[i] + 1.0 + w;
    }
}

void doubleItems(std::vector<double>& element, long iter) {
    for (int i = 0; i < element.size(); ++i) {
        double w = waste(iter, element[i]);
        element[i] = 2.0 * element[i] + w;
    }
}

void p1(channel_queue< std::vector<double>* >& ch_in,
        channel_queue< std::vector<double>* >& ch_out, long iter) {
    std::vector<double>* element = ch_in.pop();
    while (element != nullptr) {
        addOne(*element, iter);
        //printElement(*element, "stage 1");
        ch_out.push(element);
        element = ch_in.pop();
    }
    ch_out.push(nullptr);
}

void p2(channel_queue< std::vector<double>* >& ch_in,
        channel_queue< std::vector<double>* >& ch_out, long iter) {
    std::vector<double>* element = ch_in.pop();
    while (element != nullptr) {
        doubleItems(*element, iter);
        //printElement(*element, "stage 2");
        ch_out.push(element);
        element = ch_in.pop();
    }
    ch_out.push(nullptr);
}

void printElement(std::vector<double>& element, const char *stage) {
    std::cout << "Stage " << stage << ": stream element \t [ ";
    for (const auto& el : element) {
        std::cout << el << " ";
    }
    std::cout << "]" << std::endl;
}

void printResults(channel_queue< std::vector<double>* >& ch_in) {
    std::vector<double>* element = ch_in.pop();
    while (element != nullptr) {
        printElement(*element, "drain");
        element = ch_in.pop();
    }
}

double waste(long iter, double x) {
    for (long i = 0; i < iter; ++i)
        x = sin(x);
    return x;
}