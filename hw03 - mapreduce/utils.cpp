//
// Created by Alessandra Fais on 23/09/2018.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "utils.h"

bool pairs_sort(const pair<string, int>* p1, const pair<string, int>* p2) {
    string a = p1->first;
    string b = p2->first;
    transform(a.begin(), a.end(), a.begin(), ::tolower);
    transform(b.begin(), b.end(), b.begin(), ::tolower);
    return a < b;
}

void print_results(char* of, vector< pair<string, int>* >& v, ull tc, print_flag f) {
    ofstream out_file;
    out_file.open(of, ios::app); // append content at the end of the file
    if (out_file.is_open()) {
        if (f == partial_results) out_file << "Parsed " << tc << " tokens.\n";
        else out_file << "Reduced " << tc << " tokens.\n";
        for (auto p : v) {
            out_file << "< " << p->first << ", " << p->second << " >\n";
        }
        out_file.close();
    }
    else cout << "Unable to open file";
}
