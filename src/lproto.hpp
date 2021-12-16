#ifndef LPROTO_H // include guard
#define LPROTO_H

#include <vector>
#include "utils.hpp"
#include "lobject.hpp"

using namespace std;

class Proto {
public:
    ProtoData ptdb;
    // sub functions
    vector<Proto> subprotos;

    void print(string name="main", string prompt="");
};

#endif