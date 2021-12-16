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

int Instr::GetC(const Instruction& instr)
{
    int mag = instr >> 14 & 0x1ff; // the magnitude

    return mag;
}

int Instr::GetB(const Instruction& instr)
{
    int mag = instr >> 23 & 0x1ff; // the magnitude
    return mag;
}

int Instr::GetBx(const Instruction& instr)
{
    int mag = instr >> 14; // the magnitude
    return mag;
}

string Instr::comment(const Instruction& instr, const ProtoDebug& ptdb)
{
    return "TBD";
}

// Instruction unknown

InstrUnknown::InstrUnknown()
{
    this->opcode = 127;
    this->name   = "UNKNOWN";
}

string InstrUnknown::comment(const Instruction& instr, const ProtoDebug& ptdb)
{
    return " ";
}

// Instruction move

InstrMove::InstrMove()
{
    this->opcode = 0;
    this->name   = "MOVE";
}


// Instruction load constant

InstrLoadK::InstrLoadK()
{
    this->opcode = 1;
    this->name   = "LOADK";
}

string InstrLoadK::comment(const Instruction& instr, const ProtoDebug& ptdb)
{
    int A = GetA(instr);
    int Bx = GetBx(instr);

    string ret = string_format(
        "R(%d) := Kst(%x)",
        A, Bx
    );

    string kdisplay = ptdb.kdisplay[Bx];

    ret += "\n" + string_format(
        "R(%d) := %s",
        A, kdisplay.c_str()
    );
    return ret;
}

// Instruction Get Table

InstrGetTabUp::InstrGetTabUp()
{
    this->opcode = 6;
    this->name   = "GETTABUP";
}

string InstrGetTabUp::comment(const Instruction& instr, const ProtoDebug& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);
    int C = GetC(instr);
    string ret = string_format(
        "R(%d) := UpValue[%d][RK(%d)]",
        A, B, C
    );

    string updisplay = ptdb.upDisplay[B];
    string RKC = C > 255 ? ptdb.kdisplay[C - 256] : string_format("R(%d)", C);

    ret += "\n" + string_format(
        "R(%d) := UpValue[%s][%s]",
        A, updisplay.c_str(), RKC.c_str()
    );
    return ret;
}

// Instruction Set Table Up

InstrSetTabUp::InstrSetTabUp()
{
    this->opcode = 8;
    this->name   = "SETTABUP";
}

string InstrSetTabUp::comment(const Instruction& instr, const ProtoDebug& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);
    int C = GetC(instr);
    string ret = string_format(
        "UpValue[%d][RK(%d)] := RK(%d)",
        A, B, C
    );

    string updisplay = ptdb.upDisplay[A];
    string RKB = B > 255 ? ptdb.kdisplay[B - 256] : string_format("R(%d)", B);
    string RKC = C > 255 ? ptdb.kdisplay[C - 256] : string_format("R(%d)", C);

    ret += "\n" + string_format(
        "%s[%s] = %s",
        updisplay.c_str(), RKB.c_str(), RKC.c_str()
    );
    return ret;
}

// Instruction call

InstrCall::InstrCall()
{
    this->opcode = 36;
    this->name   = "CALL";
}

string InstrCall::comment(const Instruction& instr, const ProtoDebug& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);
    int C = GetC(instr);

    string ret;

    if (C != 1)
    {
        ret = string_format(
            "R(%d), ... ,R(%d) := R(%d) (R(%d), ... ,R(%d))",
            A, A + C - 2, A, A + 1, A + B - 1
        );
    } else {
        // no returned value
        ret = string_format(
            "R(%d) (R(%d), ... ,R(%d))",
            A, A + 1, A + B - 1
        );
    }



    return ret;
}

// Instruction return

InstrReturn::InstrReturn()
{
    this->opcode = 38;
    this->name   = "RETURN";
}

string InstrReturn::comment(const Instruction& instr, const ProtoDebug& ptdb)
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
    } else if (B == 1) {
        ret = string_format(
            "return void",
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
    REGCMD(InstrLoadK);
    REGCMD(InstrGetTabUp);
    REGCMD(InstrSetTabUp);
    REGCMD(InstrCall);
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