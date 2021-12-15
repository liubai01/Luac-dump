#ifndef LPROTO_H // include guard
#define LPROTO_H

#include <vector>
#include "utils.hpp"
#include "lobject.hpp"

using namespace std;

class Proto {
public:
    ProtoDebug ptdb;
    // sub functions
    vector<Proto> subprotos;

    void print(int lvl, int idx, string prompt="");
};

#endif