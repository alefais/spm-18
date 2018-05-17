//
// Created by Alessandra Fais
// SPM course - MCSN - University of Pisa
// A.A. 2017/18
//

#ifndef SPM_HW2_MODULES_H
#define SPM_HW2_MODULES_H

#include "collection.h"
#include "channel_queue.h"

const int EOS = -1;
typedef enum { SEQUENTIAL, SEQUENTIAL_OPTIMIZED, MAPREDUCE} worker_type;

void emitter(task_collection& collection1, std::vector<channel_queue < int>* >& ch, int nw, std::string& msg, long delay);
void worker_i(channel_queue<int>* ch_in, result_collection& collection2, worker_type mode, int debugID, std::string& msg);

#endif //SPM_HW2_MODULES_H
