//
// Created by Alessandra Fais
// SPM course - MCSN - University of Pisa
// A.A. 2017/18
//

#ifndef SPM_HW2_COLLECTION_H
#define SPM_HW2_COLLECTION_H

#include <vector>
#include <mutex>

const int MAX_TASK = 10000;

class collection {
protected:
    std::vector<int> coll;
    int n_tasks;

protected:
    collection(int init_size, int init_value);
};


class task_collection: public collection {
private:
    int pick;

public:
    task_collection(int init_size);
    int pop(long iter);
    int get_distributed();
    void printCollection(std::string& name);
};


class result_collection: public collection {
private:
    std::vector<int> res;
    int done_tasks;
    std::mutex mux;

public:
    result_collection(int init_size, int init_value);
    void push(const int& prime_num, int task);
    int get_value(int task);
    int get_biggest_computed(int mytask);
    int get_done();
    void printResults(std::string& name);
};

#endif //SPM_HW2_COLLECTION_H