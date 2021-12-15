#ifndef LOBJECT_H // include guard
#define LOBJECT_H

#include <vector>
#include <string>

using namespace std;

#define lua_Integer  long
#define lua_Number   double
#define Instruction  unsigned int



// data structure for proto debug infos
class ProtoDebug
{
public:
    // instruction VM assembly codes
    vector<Instruction> instrs;
    // constant display names
    vector<string> kdisplay;
    // local variable display names
    vector<string> locDisplay{};
    // up display names
    vector<string> upDisplay{};
};

#endif