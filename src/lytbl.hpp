#ifndef LYTBL_H // include guard
#define LYTBL_H

#include <vector>
#include <string>

using namespace std;

class LayoutTable {
public:
    vector<int> offsets;
    vector<string> contents;
    vector<string> descs;

    void push_back(size_t offsets, string content, string desc);

    void print_table();
};

#endif