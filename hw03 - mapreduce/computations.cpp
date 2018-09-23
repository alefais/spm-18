//
// Created by Alessandra Fais on 23/09/2018.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iterator>

#include "computations.h"
#include "utils.h"

void sequential(char* ifile, char* ofile) {
    // MAP

    string line;
    string delimiter = " ";
    ifstream in_file(ifile);
    vector< pair< string, int >* > tokens;
    ull token_count = 0;
    if (in_file.is_open()) {
        while (getline(in_file, line)) {

            // TODO send each line to a worker to be processed (parallel version, workers create pairs -> map phase)
            // TODO each worker does a reduce phase on its vector of pairs

            size_t last = 0;
            size_t next = 0;
            string token;
            while ((next = line.find(delimiter, last)) != string::npos) {
                token = line.substr(last, next - last);
                auto p = new pair<string, int>(token, 1);
                if (!token.empty()) {
                    tokens.push_back(p);
                    token_count++;
                }
                last = next + delimiter.length();
                //line.erase(0, next + delimiter.length());
            }
            //if (!token.empty()) {
            tokens.push_back(new pair<string, int>(line.substr(last), 1));
            token_count++;
            //}
        }
        in_file.close();
    }
    else cout << "Unable to open file";


    // SHUFFLE (put together all pairs with the same string value)
    //template <class RAI, class StrictWeakOrdering> void sort(RAI first, RAI last, StrictWeakOrdering compare)

    sort(tokens.begin(), tokens.end(), pairs_sort);

    print_results(ofile, tokens, token_count, partial_results);


    // REDUCE

    vector< pair<string, int>* > tokens_red;
    string cur_token = tokens.at(0)->first;
    int cur_occ = 0;
    for (auto cur_p : tokens) {
        if (cur_token == cur_p->first) {
            cur_occ += cur_p->second;
        } else {
            auto new_p = new pair<string, int>(cur_token, cur_occ);
            tokens_red.push_back(new_p);
            cur_token = cur_p->first;
            cur_occ = 1;
        }
    }
    auto new_p = new pair<string, int>(cur_token, cur_occ);
    tokens_red.push_back(new_p);

    print_results(ofile, tokens_red, tokens_red.size(), final_results);
}