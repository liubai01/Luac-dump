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

string Instr::comment(const Instruction& instr, const ProtoData& ptdb)
{
    return "TBD";
}

// Instruction unknown

InstrUnknown::InstrUnknown()
{
    this->opcode = 127;
    this->name   = "UNKNOWN";
}

string InstrUnknown::comment(const Instruction& instr, const ProtoData& ptdb)
{
    // opcode is low 6 bits, mask should be 0x00111111(0x3f)
    return string_format("opcode(%d)", instr &0x3f);
}

// Instruction move

InstrMove::InstrMove()
{
    this->opcode = 0;
    this->name   = "MOVE";
}

string InstrMove::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);

    string ret = string_format(
        "R(%d) := R(%d)",
        A, B
    );

    return ret;
}

// Instruction load boolean

InstrLoadBool::InstrLoadBool()
{
    this->opcode = 3;
    this->name   = "LOADBOOL";
}

string InstrLoadBool::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);
    int C = GetC(instr);

    string ret = string_format(
        "R(%d) := (Bool)%d; if (%d) pc++",
        A, B, C
    );

    string bl = B ? "true" : "false";

    ret += "\n" + string_format(
        "R(%d) := %s; if (%d) pc++",
        A, bl.c_str(), C
    );
    return ret;
}


// Instruction load nill

InstrLoadNil::InstrLoadNil()
{
    this->opcode = 4;
    this->name   = "LOADNIL";
}

string InstrLoadNil::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);

    string ret;

    if (B > 1)
    {
        ret = string_format(
            "R(%d), R(%d), ..., R(%d) := nil",
            A, A + 1, A + B
        );
    } else if (B) {
        ret = string_format(
            "R(%d), R(%d) := nil",
            A, A + B
        );
    } else {
        ret = string_format(
            "R(%d) := nil",
            A
        );
    }
    return ret;
}


// Instruction load constant

InstrLoadK::InstrLoadK()
{
    this->opcode = 1;
    this->name   = "LOADK";
}

string InstrLoadK::comment(const Instruction& instr, const ProtoData& ptdb)
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

// Instruction load up value

InstrGetUpVal::InstrGetUpVal()
{
    this->opcode = 5;
    this->name   = "GETUPVAL";
}

string InstrGetUpVal::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);

    string ret = string_format(
        "R(%d) := UpValue[%d]",
        A, B
    );

    string updisplay = ptdb.upDisplay[B];

    ret += "\n" + string_format(
        "R(%d) := %s",
        A, updisplay.c_str()
    );
    return ret;
}

// Instruction Get Table

InstrGetTabUp::InstrGetTabUp()
{
    this->opcode = 6;
    this->name   = "GETTABUP";
}

string InstrGetTabUp::comment(const Instruction& instr, const ProtoData& ptdb)
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
        "R(%d) := %s[%s]",
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

string InstrSetTabUp::comment(const Instruction& instr, const ProtoData& ptdb)
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

// Instruction Add

InstrAdd::InstrAdd()
{
    this->opcode = 13;
    this->name   = "ADD";
}

string InstrAdd::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);
    int C = GetC(instr);
    string ret = string_format(
        "R(%d) := RK(%d) + RK(%d)",
        A, B, C
    );

    string RKB = B > 255 ? ptdb.kdisplay[B - 256] : string_format("R(%d)", B);
    string RKC = C > 255 ? ptdb.kdisplay[C - 256] : string_format("R(%d)", C);

    ret += "\n" + string_format(
        "R(%d) := %s + %s",
        A, RKB.c_str(), RKC.c_str()
    );
    return ret;
}

// Instruction Substraction

InstrSub::InstrSub()
{
    this->opcode = 14;
    this->name   = "SUB";
}

string InstrSub::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);
    int C = GetC(instr);
    string ret = string_format(
        "R(%d) := RK(%d) - RK(%d)",
        A, B, C
    );

    string RKB = B > 255 ? ptdb.kdisplay[B - 256] : string_format("R(%d)", B);
    string RKC = C > 255 ? ptdb.kdisplay[C - 256] : string_format("R(%d)", C);

    ret += "\n" + string_format(
        "R(%d) := %s - %s",
        A, RKB.c_str(), RKC.c_str()
    );
    return ret;
}

// Instruction Multiplication

InstrMul::InstrMul()
{
    this->opcode = 15;
    this->name   = "MUL";
}

string InstrMul::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);
    int C = GetC(instr);
    string ret = string_format(
        "R(%d) := RK(%d) * RK(%d)",
        A, B, C
    );

    string RKB = B > 255 ? ptdb.kdisplay[B - 256] : string_format("R(%d)", B);
    string RKC = C > 255 ? ptdb.kdisplay[C - 256] : string_format("R(%d)", C);

    ret += "\n" + string_format(
        "R(%d) := %s * %s",
        A, RKB.c_str(), RKC.c_str()
    );
    return ret;
}

// Instruction Module

InstrMod::InstrMod()
{
    this->opcode = 16;
    this->name   = "MOD";
}

string InstrMod::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);
    int C = GetC(instr);
    string ret = string_format(
        "R(%d) := RK(%d) % RK(%d)",
        A, B, C
    );

    string RKB = B > 255 ? ptdb.kdisplay[B - 256] : string_format("R(%d)", B);
    string RKC = C > 255 ? ptdb.kdisplay[C - 256] : string_format("R(%d)", C);

    ret += "\n" + string_format(
        "R(%d) := %s %% %s",
        A, RKB.c_str(), RKC.c_str()
    );
    return ret;
}

// Instruction Power

InstrPow::InstrPow()
{
    this->opcode = 17;
    this->name   = "POW";
}

string InstrPow::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);
    int C = GetC(instr);
    string ret = string_format(
        "R(%d) := RK(%d) ^ RK(%d)",
        A, B, C
    );

    string RKB = B > 255 ? ptdb.kdisplay[B - 256] : string_format("R(%d)", B);
    string RKC = C > 255 ? ptdb.kdisplay[C - 256] : string_format("R(%d)", C);

    ret += "\n" + string_format(
        "R(%d) := %s ^ %s",
        A, RKB.c_str(), RKC.c_str()
    );
    return ret;
}

// Instruction Division

InstrDiv::InstrDiv()
{
    this->opcode = 18;
    this->name   = "DIV";
}

string InstrDiv::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);
    int C = GetC(instr);
    string ret = string_format(
        "R(%d) := RK(%d) / RK(%d)",
        A, B, C
    );

    string RKB = B > 255 ? ptdb.kdisplay[B - 256] : string_format("R(%d)", B);
    string RKC = C > 255 ? ptdb.kdisplay[C - 256] : string_format("R(%d)", C);

    ret += "\n" + string_format(
        "R(%d) := %s / %s",
        A, RKB.c_str(), RKC.c_str()
    );
    return ret;
}

// Instruction Integral Division

InstrIDiv::InstrIDiv()
{
    this->opcode = 19;
    this->name   = "IDIV";
}

string InstrIDiv::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);
    int C = GetC(instr);
    string ret = string_format(
        "R(%d) := RK(%d) // RK(%d)",
        A, B, C
    );

    string RKB = B > 255 ? ptdb.kdisplay[B - 256] : string_format("R(%d)", B);
    string RKC = C > 255 ? ptdb.kdisplay[C - 256] : string_format("R(%d)", C);

    ret += "\n" + string_format(
        "R(%d) := %s // %s",
        A, RKB.c_str(), RKC.c_str()
    );
    return ret;
}

// Instruction Binary And

InstrBAnd::InstrBAnd()
{
    this->opcode = 20;
    this->name   = "BAND";
}

string InstrBAnd::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);
    int C = GetC(instr);
    string ret = string_format(
        "R(%d) := RK(%d) & RK(%d)",
        A, B, C
    );

    string RKB = B > 255 ? ptdb.kdisplay[B - 256] : string_format("R(%d)", B);
    string RKC = C > 255 ? ptdb.kdisplay[C - 256] : string_format("R(%d)", C);

    ret += "\n" + string_format(
        "R(%d) := %s & %s",
        A, RKB.c_str(), RKC.c_str()
    );
    return ret;
}

// Instruction Binary Or

InstrBOr::InstrBOr()
{
    this->opcode = 21;
    this->name   = "BOR";
}

string InstrBOr::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);
    int C = GetC(instr);
    string ret = string_format(
        "R(%d) := RK(%d) | RK(%d)",
        A, B, C
    );

    string RKB = B > 255 ? ptdb.kdisplay[B - 256] : string_format("R(%d)", B);
    string RKC = C > 255 ? ptdb.kdisplay[C - 256] : string_format("R(%d)", C);

    ret += "\n" + string_format(
        "R(%d) := %s | %s",
        A, RKB.c_str(), RKC.c_str()
    );
    return ret;
}

// Instruction Binary XOr

InstrBXOr::InstrBXOr()
{
    this->opcode = 22;
    this->name   = "BXOR";
}

string InstrBXOr::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);
    int C = GetC(instr);
    string ret = string_format(
        "R(%d) := RK(%d) ~ RK(%d)",
        A, B, C
    );

    string RKB = B > 255 ? ptdb.kdisplay[B - 256] : string_format("R(%d)", B);
    string RKC = C > 255 ? ptdb.kdisplay[C - 256] : string_format("R(%d)", C);

    ret += "\n" + string_format(
        "R(%d) := %s ~ %s",
        A, RKB.c_str(), RKC.c_str()
    );
    return ret;
}

// Instruction Shift Left

InstrShL::InstrShL()
{
    this->opcode = 23;
    this->name   = "SHL";
}

string InstrShL::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);
    int C = GetC(instr);
    string ret = string_format(
        "R(%d) := RK(%d) << RK(%d)",
        A, B, C
    );

    string RKB = B > 255 ? ptdb.kdisplay[B - 256] : string_format("R(%d)", B);
    string RKC = C > 255 ? ptdb.kdisplay[C - 256] : string_format("R(%d)", C);

    ret += "\n" + string_format(
        "R(%d) := %s << %s",
        A, RKB.c_str(), RKC.c_str()
    );
    return ret;
}

// Instruction Shift Right

InstrShR::InstrShR()
{
    this->opcode = 24;
    this->name   = "SHR";
}

string InstrShR::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);
    int C = GetC(instr);
    string ret = string_format(
        "R(%d) := RK(%d) >> RK(%d)",
        A, B, C
    );

    string RKB = B > 255 ? ptdb.kdisplay[B - 256] : string_format("R(%d)", B);
    string RKC = C > 255 ? ptdb.kdisplay[C - 256] : string_format("R(%d)", C);

    ret += "\n" + string_format(
        "R(%d) := %s >> %s",
        A, RKB.c_str(), RKC.c_str()
    );
    return ret;
}

// Instruction call

InstrCall::InstrCall()
{
    this->opcode = 36;
    this->name   = "CALL";
}

string InstrCall::comment(const Instruction& instr, const ProtoData& ptdb)
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
    } else if (B == 2) {
        // no returned value, single argument
        ret = string_format(
            "R(%d) (R(%d), )",
            A, A + 1
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

// Instruction tail call

InstrTailCall::InstrTailCall()
{
    this->opcode = 37;
    this->name   = "TAILCALL";
}

string InstrTailCall::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);

    string ret;

    if (B == 2) {
        // single argument
        ret = string_format(
            "return R(%d)(R(%d),)",
            A, A + 1
        );
    } else {
        ret = string_format(
            "return R(%d)(R(%d), ... ,R(%d))",
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

string InstrReturn::comment(const Instruction& instr, const ProtoData& ptdb)
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

// Instruction return

InstrClosure::InstrClosure()
{
    this->opcode = 44;
    this->name   = "CLOSURE";
}

string InstrClosure::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int Bx = GetBx(instr);

    string ret = string_format(
        "R(%d) := closure(KPROTO[%d])",
        A, Bx
    );
    return ret;
}

ParserInstr::ParserInstr()
{
    // setup dictionary
    Instr* instrobj;

    REGCMD(InstrMove);        // opcode:  0
    REGCMD(InstrLoadK);       // opcode:  1
    REGCMD(InstrLoadBool);    // opcode:  3
    REGCMD(InstrLoadNil);     // opcode:  4
    REGCMD(InstrGetUpVal);    // opcode:  5
    REGCMD(InstrGetTabUp);    // opcode:  6
    REGCMD(InstrSetTabUp);    // opcode:  8
    REGCMD(InstrAdd);         // opcode: 13
    REGCMD(InstrSub);         // opcode: 14
    REGCMD(InstrMul);         // opcode: 15
    REGCMD(InstrMod);         // opcode: 16
    REGCMD(InstrPow);         // opcode: 17
    REGCMD(InstrDiv);         // opcode: 18
    REGCMD(InstrIDiv);        // opcode: 19
    REGCMD(InstrBAnd);        // opcode: 20
    REGCMD(InstrBOr);         // opcode: 21
    REGCMD(InstrBXOr);        // opcode: 22
    REGCMD(InstrShL);         // opcode: 23
    REGCMD(InstrShR);         // opcode: 24
    REGCMD(InstrCall);        // opcode: 36
    REGCMD(InstrTailCall);    // opcode: 37
    REGCMD(InstrReturn);      // opcode: 38
    REGCMD(InstrClosure);     // opcode: 44
    REGCMD(InstrUnknown);     // opcode: 127 (reserved)
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