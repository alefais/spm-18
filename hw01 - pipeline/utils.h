//
// Created by Alessandra Fais
// SPM course - MCSN - University of Pisa
// A.A. 2017/18
//

#ifndef SPM_HW1_UTILS_H
#define SPM_HW1_UTILS_H

std::vector<double>* generateStreamItem(int n);
void generateStream(channel_queue< std::vector<double>* >& ch1, int m, int n);

void addOne(std::vector<double>& element, long iter);
void doubleItems(std::vector<double>& element, long iter);

void p1(channel_queue< std::vector<double>* >& ch_in, channel_queue< std::vector<double>* >& ch_out, long iter);
void p2(channel_queue< std::vector<double>* >& ch_in, channel_queue< std::vector<double>* >& ch_out, long iter);

void printElement(std::vector<double>& element, const char *stage);
void printResults(channel_queue< std::vector<double>* >& ch_in);

double waste(long iter, double x);

#endif //SPM_HW1_UTILS_H
