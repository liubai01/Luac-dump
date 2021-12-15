#ifndef LINSTR_H // include guard
#define LINSTR_H

#include "utils.hpp"
#include "lobject.hpp"

#include <unordered_map>

using namespace std;

// by reference to:
// https://the-ravi-programming-language.readthedocs.io/en/latest/lua_bytecode_reference.html

// The comment is from lua 5.3 lopcodes.h

/*===========================================================================
  We assume that instructions are unsigned numbers.
  All instructions have an opcode in the first 6 bits.
  Instructions can have the following fields:
    'A' : 8 bits
    'B' : 9 bits
    'C' : 9 bits
    'Ax' : 26 bits ('A', 'B', and 'C' together)
    'Bx' : 18 bits ('B' and 'C' together)
    'sBx' : signed Bx
  A signed argument is represented in excess K; that is, the number
  value is the unsigned value minus K. K is exactly the maximum value
  for that argument (so that -max is represented by 0, and +max is
  represented by 2*max), which is half the maximum for the corresponding
  unsigned argument.
===========================================================================*/

class Instr {
public:
    char           opcode;
    string           name;

    virtual string comment(const Instruction& instr, const ProtoDebug& ptdb);

    int GetA(const Instruction& instr);
    int GetB(const Instruction& instr);
    int GetC(const Instruction& instr);
};

class InstrUnknown : public Instr {
public:
    InstrUnknown();
    string comment(const Instruction& instr, const ProtoDebug& ptdb);
};

class InstrMove : public Instr {
public:
    InstrMove();
};

class InstrSetTabUp : public Instr {
public:
    InstrSetTabUp();
    string comment(const Instruction& instr, const ProtoDebug& ptdb);
};

class InstrReturn : public Instr {
    // Returns to the calling function, with optional return values.
public:
    InstrReturn();
    string comment(const Instruction& instr, const ProtoDebug& ptdb);
};

class ParserInstr {
    unordered_map<char, Instr*> opcode2Instr;
public:
    ParserInstr();
    ~ParserInstr();

    Instr* parseInstr(Instruction instr);
};

#endif