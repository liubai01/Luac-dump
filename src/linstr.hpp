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

    int GetBx(const Instruction& instr);
};

class InstrUnknown : public Instr {
public:
    InstrUnknown();
    string comment(const Instruction& instr, const ProtoDebug& ptdb);
};

// opcode 0
class InstrMove : public Instr {
public:
    InstrMove();
};

// opcode 1
class InstrLoadK : public Instr {
public:
    InstrLoadK();
    string comment(const Instruction& instr, const ProtoDebug& ptdb);
};

// opcode 3
class InstrLoadBool : public Instr {
public:
    InstrLoadBool();
    string comment(const Instruction& instr, const ProtoDebug& ptdb);
};

// opcode 5
class InstrGetUpVal : public Instr {
public:
    InstrGetUpVal ();
    string comment(const Instruction& instr, const ProtoDebug& ptdb);
};

// opcode 6
class InstrGetTabUp : public Instr {
public:
    InstrGetTabUp();
    string comment(const Instruction& instr, const ProtoDebug& ptdb);
};

// opcode 8
class InstrSetTabUp : public Instr {
public:
    InstrSetTabUp();
    string comment(const Instruction& instr, const ProtoDebug& ptdb);
};

// opcode 13
class InstrAdd : public Instr {
public:
    InstrAdd();
    string comment(const Instruction& instr, const ProtoDebug& ptdb);
};

// opcode 36
class InstrCall : public Instr {
public:
    InstrCall();
    string comment(const Instruction& instr, const ProtoDebug& ptdb);
};

// opcode 38
class InstrReturn : public Instr {
    // Returns to the calling function, with optional return values.
public:
    InstrReturn();
    string comment(const Instruction& instr, const ProtoDebug& ptdb);
};

// opcode 44
class InstrClosure : public Instr {
    // Creates an instance (or closure) of a function prototype.
public:
    InstrClosure();
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