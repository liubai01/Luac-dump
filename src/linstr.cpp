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

int Instr::GetSBx(const Instruction& instr)
{
    int mag = instr >> 14;
    return mag - (1 << 17) + 1;
}

int Instr::GetAx(const Instruction& instr)
{
    return instr >> 6;
}

string Instr::GetRDisplay(int ridx, const ProtoData& ptdb)
{
    string rdisplay = 
        ridx < ptdb.locDisplay.size() ? 
            ptdb.locDisplay[ridx].substr(1, ptdb.locDisplay[ridx].size() - 2) 
          : string_format("R(%d)", ridx)
    ;

    return rdisplay;
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
    string radisplay = GetRDisplay(A, ptdb);

    ret += "\n" + string_format(
        "%s := %s",
        radisplay.c_str(), kdisplay.c_str()
    );
    return ret;
}

// Instruction load constant (when #cont. >= 262143)

InstrLoadKx::InstrLoadKx()
{
    this->opcode = 2;
    this->name   = "LOADKX";
}

string InstrLoadKx::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);

    string ret = string_format(
        "R(%d) := Kst(extra arg)",
        A
    );

    string radisplay = GetRDisplay(A, ptdb);

    ret += "\n" + string_format(
        "%s := Kst(extra arg)",
        radisplay.c_str()
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
        "R(%d) := (Bool) %d; if (%d) pc++",
        A, B, C
    );

    string bl = B ? "true" : "false";
    string pcpp = C ? "pc++" : "";

    ret += "\n" + string_format(
        "R(%d) := %s; %s",
        A, bl.c_str(), pcpp.c_str()
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

// Instruction Get Table

InstrGetTable::InstrGetTable()
{
    this->opcode = 7;
    this->name   = "GETTABLE";
}

string InstrGetTable::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);
    int C = GetC(instr);
    string ret = string_format(
        "R(%d) := R(%d)[RK(%d)]",
        A, B, C
    );

    string RKC = C > 255 ? ptdb.kdisplay[C - 256] : string_format("R(%d)", C);

    ret += "\n" + string_format(
        "R(%d) := R(%d)[%s]",
        A, B, RKC.c_str()
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

// Instruction Set Up Value

InstrSetUpVal::InstrSetUpVal()
{
    this->opcode = 9;
    this->name   = "SETUPVAL";
}

string InstrSetUpVal::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);

    string ret = string_format(
        "UpValue[%d] := R(%d)",
        B, A
    );

    string updisplay = ptdb.upDisplay[B];

    ret += "\n" + string_format(
        "%s = R(%d)",
        updisplay.c_str(), A
    );
    return ret;
}

// Instruction Set Table

InstrSetTable::InstrSetTable()
{
    this->opcode = 10;
    this->name   = "SETTABLE";
}

string InstrSetTable::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);
    int C = GetC(instr);
    string ret = string_format(
        "R(%d)[RK(%d)] := RK(%d)",
        A, B, C
    );

    string RKB = B > 255 ? ptdb.kdisplay[B - 256] : string_format("R(%d)", B);
    string RKC = C > 255 ? ptdb.kdisplay[C - 256] : string_format("R(%d)", C);

    ret += "\n" + string_format(
        "R(%d)[%s] := %s",
        A, RKB.c_str(), RKC.c_str()
    );
    return ret;
}


// Instruction New Table

InstrNewTable::InstrNewTable()
{
    this->opcode = 11;
    this->name   = "NEWTABLE";
}

string InstrNewTable::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);
    int C = GetC(instr);
    string ret = string_format(
        "R(%d) := {} (array.size = %d, hash.size = %d)",
        A, luaO_int2fb(B), luaO_int2fb(C)
    );

    return ret;
}

// Instruction Self

InstrSelf::InstrSelf()
{
    this->opcode = 12;
    this->name   = "SELF";
}

string InstrSelf::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);
    int C = GetC(instr);
    string ret = string_format(
        "R(%d) := R(%d); R(%d) := R(%d)[RK(%d)]",
        A + 1, B, A, B, C
    );

    string RKC = C > 255 ? ptdb.kdisplay[C - 256] : string_format("R(%d)", C);

    ret += string_format(
        "\nR(%d) := R(%d); R(%d) := R(%d)[%s]",
        A + 1, B, A, B, RKC.c_str()
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

// Instruction Unary Minus

InstrUNM::InstrUNM()
{
    this->opcode = 25;
    this->name   = "UNM";
}

string InstrUNM::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);

    string ret = string_format(
        "R(%d) := -R(%d)",
        A, B
    );

    return ret;
}

// Instruction Bit-wise Not Operator

InstrBNot::InstrBNot()
{
    this->opcode = 26;
    this->name   = "BNOT";
}

string InstrBNot::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);

    string ret = string_format(
        "R(%d) := ~R(%d)",
        A, B
    );

    return ret;
}

// Instruction Bit-wise Not Operator

InstrNot::InstrNot()
{
    this->opcode = 27;
    this->name   = "NOT";
}

string InstrNot::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);

    string ret = string_format(
        "R(%d) := not R(%d)",
        A, B
    );

    return ret;
}

// Instruction Length

InstrLen::InstrLen()
{
    this->opcode = 28;
    this->name   = "LEN";
}

string InstrLen::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);

    string ret = string_format(
        "R(%d) := length of R(%d)",
        A, B
    );

    return ret;
}

// Instruction Concatenate

InstrConcat::InstrConcat()
{
    this->opcode = 29;
    this->name   = "CONCAT";
}

string InstrConcat::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);
    int C = GetC(instr);

    string ret = string_format(
        "R(%d) := R(%d).. ... ..R(%d)",
        A, B, C
    );

    return ret;
}

// Instruction Jump

InstrJmp::InstrJmp()
{
    this->opcode = 30;
    this->name   = "JMP";
}

string InstrJmp::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int sBx = GetSBx(instr);
    string ret;

    if (A)
    {
        ret += string_format(
            "pc += %d; close all upvalues >= R(%d)",
            sBx, A - 1
        );
    } else {
        ret += string_format(
            "pc += %d",
            sBx
        );
    }

    return ret;
}

// Instruction Equal To

InstrEQ::InstrEQ()
{
    this->opcode = 31;
    this->name   = "EQ";
}

string InstrEQ::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);
    int C = GetC(instr);

    string ret = string_format(
        "if ((RK(%d) == RK(%d)) ~= %d) then pc++",
        B, C, A
    );

    string RKB = B > 255 ? ptdb.kdisplay[B - 256] : string_format("R(%d)", B);
    string RKC = C > 255 ? ptdb.kdisplay[C - 256] : string_format("R(%d)", C);

    ret += string_format(
        "\nif ((%s == %s) ~= %d) then pc++",
        RKB.c_str(), RKC.c_str(), A
    );

    return ret;
}

// Instruction Less than

InstrLT::InstrLT()
{
    this->opcode = 32;
    this->name   = "LT";
}

string InstrLT::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);
    int C = GetC(instr);

    string ret = string_format(
        "if ((RK(%d) < RK(%d)) ~= %d) then pc++",
        B, C, A
    );

    string RKB = B > 255 ? ptdb.kdisplay[B - 256] : string_format("R(%d)", B);
    string RKC = C > 255 ? ptdb.kdisplay[C - 256] : string_format("R(%d)", C);

    ret += string_format(
        "\nif ((%s < %s) ~= %d) then pc++",
        RKB.c_str(), RKC.c_str(), A
    );

    return ret;
}

// Instruction Less than or equal to

InstrLE::InstrLE()
{
    this->opcode = 33;
    this->name   = "LE";
}

string InstrLE::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);
    int C = GetC(instr);

    string ret = string_format(
        "if ((RK(%d) <= RK(%d)) ~= %d) then pc++",
        B, C, A
    );

    string RKB = B > 255 ? ptdb.kdisplay[B - 256] : string_format("R(%d)", B);
    string RKC = C > 255 ? ptdb.kdisplay[C - 256] : string_format("R(%d)", C);

    ret += string_format(
        "\nif ((%s <= %s) ~= %d) then pc++",
        RKB.c_str(), RKC.c_str(), A
    );

    return ret;
}

// Instruction Test

InstrTest::InstrTest()
{
    this->opcode = 34;
    this->name   = "TEST";
}

string InstrTest::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);
    int C = GetC(instr);

    string ret = string_format(
        "if (boolean(R(%d)) != %d)\n  then pc++",
        B, C
    );

    return ret;
}

// Instruction Test And Set

InstrTestSet::InstrTestSet()
{
    this->opcode = 35;
    this->name   = "TESTSET";
}

string InstrTestSet::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);
    int C = GetC(instr);

    string ret = string_format(
        "if (boolean(R(%d)) != %d)\n  then pc++ else R(%d) := R(%d)",
        B, C, A, B
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

    string retval;
    string callargs;

    if (C == 1)
    {
        retval = "";
    } else if (C == 2) {
        retval = string_format("R(%d) := ", A);
    } else if (C == 3) {
        retval = string_format("R(%d), R(%d) := ", A, A + 1);
    } else {
        retval = string_format("R(%d), ..., R(%d) := ", A, A + C - 2);
    }

    if (B == 2)
    {
        callargs = string_format("(R(%d), )", A + 1);
    } else if (B == 3) {
        callargs = string_format("(R(%d), R(%d))", A + 1, A + B - 1);
    } else {
        callargs = string_format("(R(%d), ..., R(%d))", A + 1, A + B - 1);
    }

    ret = string_format("%sR(%d) %s", retval.c_str(), A, callargs.c_str());

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
    } else if (B == 2) {
        ret = string_format(
            "return R(%d)",
            A
        );
    } else {
        ret = string_format(
            "return R(%d), ... ,R(%d)",
            A, A + B - 2
        );
    }

    return ret;
}

// Instruction for loop

InstrForLoop::InstrForLoop()
{
    this->opcode = 39;
    this->name   = "FORLOOP";
}

string InstrForLoop::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int sBx = GetSBx(instr);

    string ret = string_format(
        "R(%d) += R(%d)",
        A, A + 2
    );

    ret += string_format(
        "\nif R(%d) <?= R(%d) then \n{\n   pc+=%d\n   R(%d) = R(%d)\n}",
        A, A + 1, sBx, A + 3, A
    );

    return ret;
}

// Instruction for loop preparation

InstrForPrep::InstrForPrep()
{
    this->opcode = 40;
    this->name   = "FORPREP";
}

string InstrForPrep::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int sBx = GetSBx(instr);

    string ret = string_format(
        "R(%d) -= R(%d); pc += %d",
        A, A + 2, sBx
    );

    ret += string_format(
        "\n%s -= %s; pc += %d",
        GetRDisplay(A, ptdb).c_str(), GetRDisplay(A + 2, ptdb).c_str(), sBx, A + 3, A
    );

    return ret;
}

// Instruction Trim For Call

InstrTForCall::InstrTForCall()
{
    this->opcode = 41;
    this->name   = "TFORCALL";
}

string InstrTForCall::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);
    int C = GetC(instr);
    string ret;

    string retvals;

    if (C == 1)
    {
        retvals = string_format("R(%d)", A + 3);
    } else if (C == 2) {
        retvals = string_format("R(%d), R(%d)", A + 3, A + 4);
    } else {
        retvals = string_format("R(%d), ... ,R(%d)", A + 3, A + 2 + C);
    }

    ret = string_format(
        "%s := R(%d)(R(%d), R(%d))",
        retvals.c_str(), A, A + 1, A + 2
    );
 

    return ret;
}

// Instruction Trim For Loop

InstrTForLoop::InstrTForLoop()
{
    this->opcode = 42;
    this->name   = "TFORLOOP";
}

string InstrTForLoop::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A   = GetA(instr);
    int sBx = GetSBx(instr);

    string ret = string_format(
        "if R(%d) ~= nil then { R(%d)=R(%d); pc += %d }",
        A + 1, A, A + 1, sBx
    );

    return ret;
}

// Instruction set list

InstrSetList::InstrSetList()
{
    this->opcode = 43;
    this->name   = "SETLIST";
}

string InstrSetList::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);
    int C = GetC(instr);
    string ret;

    string range = B ? string_format("1 <= i <= %d", B) : "1 <= i <= stk.top";

    if (C - 1)
    {
        ret = string_format(
            "R(%d)[%d * FPF + i] := R(%d + i), %s",
            A, C - 1, A, range.c_str()
        );
    } else {
        ret = string_format(
            "R(%d)[i] := R(%d + i), %s",
            A, A, range.c_str()
        );
    }


    return ret;
}

// Instruction closure

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

InstrVarArg::InstrVarArg()
{
    this->opcode = 45;
    this->name   = "VARARG";
}

string InstrVarArg::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int A = GetA(instr);
    int B = GetB(instr);
    string ret;

    if (B == 0)
    {
        ret = string_format(
            "R(%d), ... = vararg",
            A
        );
    } else if (B == 1) {
        ret = string_format(
            "R(%d) = vararg",
            A
        );
    } else if (B == 2) {
        ret = string_format(
            "R(%d), R(%d) = vararg",
            A, A + 1
        );
    } else {
        ret = string_format(
            "R(%d), R(%d), ..., R(%d) = vararg",
            A, A + 1, A + B - 1
        );
    }

    return ret;
}

InstrExtraArg::InstrExtraArg()
{
    this->opcode = 46;
    this->name   = "EXTRAARG";
}

string InstrExtraArg::comment(const Instruction& instr, const ProtoData& ptdb)
{
    int Ax = GetAx(instr);
    string ret = string_format(
        "extra arg = %d",
        Ax
    );

    return ret;
}

ParserInstr::ParserInstr()
{
    // setup dictionary
    Instr* instrobj;

    REGCMD(InstrMove);        // opcode:  0
    REGCMD(InstrLoadK);       // opcode:  1
    REGCMD(InstrLoadKx);      // opcode:  2
    REGCMD(InstrLoadBool);    // opcode:  3
    REGCMD(InstrLoadNil);     // opcode:  4
    REGCMD(InstrGetUpVal);    // opcode:  5
    REGCMD(InstrGetTabUp);    // opcode:  6
    REGCMD(InstrGetTable);    // opcode:  7
    REGCMD(InstrSetTabUp);    // opcode:  8
    REGCMD(InstrSetUpVal);    // opcode:  9
    REGCMD(InstrSetTable);    // opcode: 10
    REGCMD(InstrNewTable);    // opcode: 11
    REGCMD(InstrSelf);        // opcode: 12
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
    REGCMD(InstrUNM);         // opcode: 25
    REGCMD(InstrBNot);        // opcode: 26
    REGCMD(InstrNot);         // opcode: 27
    REGCMD(InstrLen);         // opcode: 28
    REGCMD(InstrConcat);      // opcode: 29
    REGCMD(InstrJmp);         // opcode: 30
    REGCMD(InstrEQ);          // opcode: 31
    REGCMD(InstrLT);          // opcode: 32
    REGCMD(InstrLE);          // opcode: 33
    REGCMD(InstrTest);        // opcode: 34
    REGCMD(InstrTestSet);     // opcode: 35
    REGCMD(InstrCall);        // opcode: 36
    REGCMD(InstrTailCall);    // opcode: 37
    REGCMD(InstrReturn);      // opcode: 38
    REGCMD(InstrForLoop);     // opcode: 39
    REGCMD(InstrForPrep);     // opcode: 40
    REGCMD(InstrTForCall);    // opcode: 41
    REGCMD(InstrTForLoop);    // opcode: 42
    REGCMD(InstrSetList);     // opcode: 43
    REGCMD(InstrClosure);     // opcode: 44
    REGCMD(InstrVarArg);      // opcode: 45
    REGCMD(InstrExtraArg);    // opcode: 46
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