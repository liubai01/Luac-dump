#include "linstr.hpp"

#include <iostream>

using namespace std;

#define REGCMD(cmd) \
instrobj = new cmd(); \
opcode2Instr[instrobj->opcode] = instrobj;

int Instr::GetA(const Instruction& instr)
{
    return instr >> 6 & 0xff;
}

int Instr::GetB(const Instruction& instr)
{
    return instr >> 14 & 0x1ff;
}

int Instr::GetC(const Instruction& instr)
{
    return instr >> 23 & 0x1ff;
}

string Instr::comment(const Instruction& instr)
{
    return "TBD";
}

InstrUnknown::InstrUnknown()
{
    this->opcode = 127;
    this->name   = "UNKNOWN";
}

string InstrUnknown::comment(const Instruction& instr)
{
    return " ";
}

InstrMove::InstrMove()
{
    this->opcode = 0;
    this->name   = "MOVE";
}

InstrSetTabUp::InstrSetTabUp()
{
    this->opcode = 8;
    this->name   = "SETTABUP";
}

string InstrSetTabUp::comment(const Instruction& instr)
{
    string ret = string_format(
        "UpValue[%d][RK(%d)] := RK(%d)",
        GetA(instr), GetB(instr) - 256, GetC(instr) - 256
    );
    return ret;
}

InstrReturn::InstrReturn()
{
    this->opcode = 38;
    this->name   = "RETURN";
}

string InstrReturn::comment(const Instruction& instr)
{
    int A = GetA(instr);
    int B = GetB(instr);
    string ret;

    if (B == 0)
    {
        // If B is 0, the set of values range from R(A) to the top of the stack.
        ret = string_format(
            "return R(:%d)",
            A
        );
    } else {
        ret = string_format(
            "return R(%d), ... ,R(%d+%d-2)",
            A, A, B
        );
    }

    return ret;
}

ParserInstr::ParserInstr()
{
    // setup dictionary
    Instr* instrobj;

    REGCMD(InstrMove);
    REGCMD(InstrSetTabUp);
    REGCMD(InstrReturn);
    REGCMD(InstrUnknown);
}

ParserInstr::~ParserInstr()
{
    // clean up
    for (auto i: opcode2Instr)
    {
        delete i.second;
    }

}

Instr* ParserInstr::parseInstr(Instruction assembly_code)
{
    Instr* ret = nullptr;
    char opcode = assembly_code & 0x3f;

    auto f = opcode2Instr.find(opcode);
    if (f != opcode2Instr.end())
    {
        ret = f->second;
    } else {
        ret = opcode2Instr.find(127)->second;
    }


    return ret;
}