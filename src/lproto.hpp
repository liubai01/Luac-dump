#ifndef LPROTO_H // include guard
#define LPROTO_H

#include <vector>
#include "utils.hpp"

using namespace std;

class Proto {
public:
    // instruction VM assembly codes
    vector<Instruction> instrs;
    // constant display names
    vector<string> kdisplay;
    // local variable display names
    vector<string> locDisplay{};
    // sub functions
    vector<Proto> subprotos;

    void print(int lvl, int idx, string prompt="");
};

#endif