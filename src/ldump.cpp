#include "ldump.hpp"

#include "utils.hpp"
#include "lytbl.hpp"
#include <cassert>
#include <iostream>



using namespace std;

Dumped::Dumped(unsigned char* bytecodeAddr)
{
    this->bytecodeAddr = bytecodeAddr;
    HeaderBlock hb;

    // parse the header by HeaderBlock structure
    std::memcpy(&hb, bytecodeAddr, sizeof(HeaderBlock));

    // big endian or litte endian, signature differs
    assert(hb.Lua_Signature ==  0x1b4c7561 || hb.Lua_Signature == 0x61754c1b);

    // check whether lua version is 5.3
    assert(hghByte(hb.Lua_VerNumber) == 5);
    assert(lowByte(hb.Lua_VerNumber) == 3);

    // check whether data conversion is correction
    assert(strcmp(reinterpret_cast<const char*>(hb.Lua_Data), "\x19\x93\r\n\x1a\n"));
}

int getFunctionBlockSize(unsigned char* baseAddr)
{
    unsigned char* initBase = baseAddr;

    // source name
    loadAndProceed<string>(&baseAddr);

    // base
    baseAddr = baseAddr + sizeof(FuncBlock);

    // instruction list
    int numInstr = loadAndProceed<int>(&baseAddr);
    baseAddr += sizeof(int) * numInstr;
   

    // constant list
    int numConsant = loadAndProceed<int>(&baseAddr);

    for (int i = 0; i < numConsant; ++i)
    {
        // Constant Layout: 
        //  (1) 1 byte of type constant
        //  (2) variant bytes of constant content

        // The low byte(0-3) bits of tt_ w.r.t. constant decides the type
        int constType = loadAndProceed<unsigned char>(&baseAddr);
        int nonvarTag = lowByte(constType);
        int varTag    = hghByte(constType);

        switch(nonvarTag)
        {
            case 0:
                // NIL
                break;
            case 1:
                // Boolean: TBD
                break;
            case 3:
                if (varTag == 0)
                {
                    // double
                    baseAddr += sizeof(lua_Number);
                } else {
                    // long
                    baseAddr += sizeof(lua_Integer);
                }
                break;
            case 4:
                loadAndProceed<string>(&baseAddr);
                break;
            default:
                // should not reach here
                assert(0 == 1);
                break;
        }
    }

    // up value lists
    int numUpVal = loadAndProceed<int>(&baseAddr);
    baseAddr += 2 * numUpVal;

    // nested function list
    int numNestedFuncs = loadAndProceed<int>(&baseAddr);

    for (int i = 0; i < numNestedFuncs; ++i)
    {
        baseAddr += getFunctionBlockSize(baseAddr);
    }

    // line info list (debug)
    int numLineInfo = loadAndProceed<int>(&baseAddr);
    baseAddr += sizeof(int) * numLineInfo;

    // local vars (debug)
    int numLocVars = loadAndProceed<int>(&baseAddr);
    for (int i = 0; i < numLocVars; ++i)
    {
        // original dump code in ldump.c DumpDebug function for reference: 
        // DumpString(f->locvars[i].varname, D);
        // DumpInt(f->locvars[i].startpc, D);
        // DumpInt(f->locvars[i].endpc, D);

        loadAndProceed<string>(&baseAddr);
        baseAddr += sizeof(int) * 2;
    }

    // Up Values Names
    int numUpVals = loadAndProceed<int>(&baseAddr);

    // by default, there is a _ENV, you could refer to lua-users.org/wiki/EnvironmentsTutorial
    for (int i = 0; i < numUpVals; ++i)
    {
        loadAndProceed<string>(&baseAddr);
    }

    return baseAddr - initBase;
}

void Dumped::printHeaderBlock()
{
    LayoutTable lt;
    HeaderBlock hb;

    // parse the header by HeaderBlock structure
    std::memcpy(&hb, bytecodeAddr, sizeof(HeaderBlock));

    // print the parsed header
    int o = 0;

    lt.push_back(-1, " ", " ");
    lt.push_back(-1, " ", "**Lua Metadata**");
    lt.push_back(-1, " ", " ");

    lt.push_back(o, "0x" + sprintHex(hb.Lua_Signature), "Lua signature");
    o += sizeof(hb.Lua_Signature);


    lt.push_back(o, "0x" + sprintHex(hb.Lua_VerNumber), "Lua version");
    o += sizeof(hb.Lua_VerNumber);

    lt.push_back(o, "0x" + sprintHex(hb.Lua_FormatVer), "Lua Format Version");
    o += sizeof(hb.Lua_VerNumber);

    lt.push_back(o, "0x" + sprintHex(hb.Lua_Data, 6), "Lua Data");
    o += 6;

    lt.push_back(-1, " ", " ");
    lt.push_back(-1, " ", "**Lua Metadata(Sizes)**");
    lt.push_back(-1, " ", " ");

    lt.push_back(o, "0x" + sprintHex(hb.Lua_SizeInt), "Size of Integer");
    o += sizeof(hb.Lua_SizeInt);

    lt.push_back(o, "0x" + sprintHex(hb.Lua_SizeSizeT), "Size of size_t");
    o += sizeof(hb.Lua_SizeSizeT);

    lt.push_back(o, "0x" + sprintHex(hb.Lua_SizeInstr), "Size of Instruction");
    o += sizeof(hb.Lua_SizeInstr);

    lt.push_back(o, "0x" + sprintHex(hb.Lua_SizeLuaInt), "Size of Lua Integer");
    o += sizeof(hb.Lua_SizeLuaInt);
    assert(static_cast<size_t>(hb.Lua_SizeLuaInt) == sizeof(lua_Integer));

    lt.push_back(o, "0x" + sprintHex(hb.Lua_SizeLuaNum), "Size of Lua Number");
    o += sizeof(hb.Lua_SizeLuaNum);
    assert(static_cast<size_t>(hb.Lua_SizeLuaNum) == sizeof(lua_Number));

    lt.push_back(-1, " ", " ");
    lt.push_back(-1, " ", "**Lua Examples**");
    lt.push_back(-1, " ", " ");

    char buf[30];

    sprintf(buf, "%ld", hb.Lua_ExampleInt);
    lt.push_back(o, string(buf), "Example Lua Integer (0x5678=22146)");
    o += sizeof(hb.Lua_ExampleInt);

    sprintf(buf, "%lf", hb.Lua_ExampleNum);
    lt.push_back(o, string(buf), "Example Lua Number (370.5)");
    o += sizeof(hb.Lua_ExampleNum);

    lt.push_back(-1, " ", " ");

    lt.push_back(o, "0x" + sprintHex(hb.Lua_SizeUpvalues), "Size of Up Values");
    o += sizeof(hb.Lua_SizeUpvalues);

    lt.push_back(-1, " ", " ");
    lt.push_back(o, " ", "End of table");

    lt.print_table();

    printf("\n");
    cout << "Table Size: " << o << endl;
}

vector<unsigned char*> Dumped::subFuncsAddrs(unsigned char* startAddr)
{
    vector<unsigned char*> ret;

    unsigned char* baseAddr;

    if (startAddr)
    {
        baseAddr = startAddr;
    } else {
        baseAddr = bytecodeAddr + sizeof(HeaderBlock);
    }

    // source name
    loadAndProceed<string>(&baseAddr);

    // base
    baseAddr = baseAddr + sizeof(FuncBlock);

    // instruction list
    int numInstr = loadAndProceed<int>(&baseAddr);
    baseAddr += sizeof(int) * numInstr;
   

    // constant list
    int numConsant = loadAndProceed<int>(&baseAddr);

    for (int i = 0; i < numConsant; ++i)
    {
        // Constant Layout: 
        //  (1) 1 byte of type constant
        //  (2) variant bytes of constant content

        // The low byte(0-3) bits of tt_ w.r.t. constant decides the type
        int constType = loadAndProceed<unsigned char>(&baseAddr);
        int nonvarTag = lowByte(constType);
        int varTag    = hghByte(constType);

        switch(nonvarTag)
        {
            case 0:
                // NIL
                break;
            case 1:
                // Boolean: TBD
                break;
            case 3:
                if (varTag == 0)
                {
                    // double
                    baseAddr += sizeof(lua_Number);
                } else {
                    // long
                    baseAddr += sizeof(lua_Integer);
                }
                break;
            case 4:
                loadAndProceed<string>(&baseAddr);
                break;
            default:
                // should not reach here
                assert(0 == 1);
                break;
        }
    }

    // up value lists
    int numUpVal = loadAndProceed<int>(&baseAddr);
    baseAddr += 2 * numUpVal;

    // nested function list
    int numNestedFuncs = loadAndProceed<int>(&baseAddr);

    for (int i = 0; i < numNestedFuncs; ++i)
    {
        ret.push_back(baseAddr);
        baseAddr += getFunctionBlockSize(baseAddr);
    }

    return ret;
}

void Dumped::printFunctionBlock(unsigned char* startAddr)
{
    LayoutTable lt;
    int o; // save the offset

    unsigned char* initBase = bytecodeAddr;
    unsigned char* bytecodeAddr;

    if (startAddr) {
        bytecodeAddr = startAddr;
    } else {
        bytecodeAddr = this->bytecodeAddr + sizeof(HeaderBlock);
    }

    lt.push_back(-1, " ", " ");
    lt.push_back(-1, " ", "**Lua Function Metadata**");
    lt.push_back(-1, " ", " ");


    o = bytecodeAddr - initBase;
    string sourceName = loadAndProceed<string>(&bytecodeAddr);
    lt.push_back(o, sourceName, "Source name");

    // dump in bytecodes to the func block
    FuncBlock fb;
    std::memcpy(&fb, bytecodeAddr, sizeof(FuncBlock));
    
    o = bytecodeAddr - initBase;
    lt.push_back(o, "0x" + sprintHex(fb.Lua_LineDefined), "First line defined");
    bytecodeAddr = bytecodeAddr + sizeof(fb.Lua_LineDefined);

    o = bytecodeAddr - initBase;
    lt.push_back(o, "0x" + sprintHex(fb.Lua_LastLineDefined), "Last lind defined");
    bytecodeAddr = bytecodeAddr + sizeof(fb.Lua_LastLineDefined);

    o = bytecodeAddr - initBase;
    lt.push_back(o, to_string(fb.Lua_NumParam), "Number of params");
    bytecodeAddr = bytecodeAddr + sizeof(fb.Lua_NumParam);

    o = bytecodeAddr - initBase;
    lt.push_back(o, "0x" + sprintHex(fb.Lua_VarArgFlag), "`is_vararg` flag");
    bytecodeAddr = bytecodeAddr + sizeof(fb.Lua_VarArgFlag);

    o = bytecodeAddr - initBase;
    lt.push_back(o, to_string(fb.Lua_MaxStackSize), "Maximum stack size");
    bytecodeAddr = bytecodeAddr + sizeof(fb.Lua_MaxStackSize);

    lt.push_back(-1, " ", " ");
    lt.push_back(-1, " ", "**Function Instructions**");
    lt.push_back(-1, " ", " ");

    o = bytecodeAddr - initBase;
    int numInstr = loadAndProceed<int>(&bytecodeAddr);
    lt.push_back(o, to_string(numInstr), "Number of instructions");

    for(int i = 0; i < numInstr; ++i)
    {
        Instruction asbly_code = loadAndProceed<Instruction>(&bytecodeAddr);

        // 0x3f  : [0, 5 ] bits mask = 00000000111111
        // 0xfc0 : [7, 14] bits mask = 01111111000000

        o = bytecodeAddr - initBase;
        lt.push_back(
            o, 
            "0x" + sprintHex(toBigEnd(asbly_code)), 
            string_format("instr %d, optcode %2d", i, asbly_code & 0x3f)
        );
    }

    lt.push_back(-1, " ", " ");
    lt.push_back(-1, " ", "**Constants**");
    lt.push_back(-1, " ", " ");

    o = bytecodeAddr - initBase;
    int numConsant = loadAndProceed<int>(&bytecodeAddr);
    lt.push_back(o, to_string(numConsant), "Number of constants");

    // About Constants List In Lua
    // (1) Lua function itself is a Proto structure defined in `lobject.h` 
    // (2) Constants are Proto.k, an array of TValue.
    // (3) TValue is also defined in `lobject.h`. It owns two fields: value(of type Value), tt_(of type int).
    //      (3.1) Value type is a C Union of lua_Integer, lua_Number, lua_CFunction, ... etc.
    //      (3.2) tt_ itself is a int, indicating the type of that TValue
    //      (3.3) Defined by macro in lua.h, the low byte(0-3 bits) of tt_ decides the type by:
    //               #define LUA_TNIL                0
    //               #define LUA_TBOOLEAN            1
    //               #define LUA_TNUMBER             3
    //               #define LUA_TSTRING             4
    //

    for (int i = 0; i < numConsant; ++i)
    {
        // Constant Layout: 
        //  (1) 1 byte of type constant
        //  (2) variant bytes of constant content

        // The low byte(0-3) bits of tt_ w.r.t. constant decides the type
        lt.push_back(
            -1, 
            " ", 
            string_format("Constant [%d]", i)
        );

        int constType = loadAndProceed<unsigned char>(&bytecodeAddr);
        int nonvarTag = lowByte(constType);
        int varTag    = hghByte(constType);

        o = bytecodeAddr - initBase;
        lt.push_back(
            o, 
            "0x" + sprintHex(static_cast<unsigned char>(constType)), 
            "Constant ID"
        );

        switch(nonvarTag)
        {
            case 0:
                lt.push_back(-1, " ", "NIL");
                break;
            case 3:
                if (varTag == 0)
                {
                    lt.push_back(-1, " ", "Number(Double)");
                    o = bytecodeAddr - initBase;
                    lt.push_back(o, to_string(loadAndProceed<lua_Number>(&bytecodeAddr)), "Content");
                } else {
                    lt.push_back(-1, " ", "Number(Integer)");
                    o = bytecodeAddr - initBase;
                    lt.push_back(o, to_string(loadAndProceed<lua_Integer>(&bytecodeAddr)), "Content");
                }
                break;
            case 4:
                lt.push_back(-1, " ", "String");
                o = bytecodeAddr - initBase;
                lt.push_back(o, loadAndProceed<string>(&bytecodeAddr), "Content");
                break;
            default:
                lt.push_back(-1, " ", "Unknown(TBD)");
                break;
        }
        printf("\n");
    }

    lt.push_back(-1, " ", " ");
    lt.push_back(-1, " ", "**UpValues**");
    lt.push_back(-1, " ", " ");

    o = bytecodeAddr - initBase;
    int numUpVal = loadAndProceed<int>(&bytecodeAddr);
    lt.push_back(o, to_string(numUpVal), "Number of upvalues");

    for (int i = 0; i < numUpVal; ++i)
    {
        lt.push_back(
            -1, 
            " ", 
            string_format("Upvalue [%d]", i)
        );

        o = bytecodeAddr - initBase;
        int instack = loadAndProceed<unsigned char>(&bytecodeAddr);  // whether it is in stack (register)
        int idx     = loadAndProceed<unsigned char>(&bytecodeAddr);  // index of upvalue (in stack or in outer function's list)

        lt.push_back(o, 
            to_string(instack), 
            "is instack"
        );

        lt.push_back(o + 1, 
            to_string(idx), 
            "upval index"
        );
    }

    lt.push_back(-1, " ", " ");
    lt.push_back(-1, " ", "**Sub Functions**");
    lt.push_back(-1, " ", " ");

    o = bytecodeAddr - initBase;
    int numNestedFuncs = loadAndProceed<int>(&bytecodeAddr);
    lt.push_back(o, to_string(numNestedFuncs), "Sub Function Nums");

    for (int i = 0; i < numNestedFuncs; ++i)
    {
        int funcSize = getFunctionBlockSize(bytecodeAddr);
        o = bytecodeAddr - initBase;
        bytecodeAddr += funcSize;

        lt.push_back(
            o, 
            "see table below", 
            string_format("Sub-Function [%d]: size %d", i, funcSize)
        );
    }

    lt.push_back(-1, " ", " ");
    lt.push_back(-1, " ", "**Debug Infos - Line Info(opcode -> src. line)**");
    lt.push_back(-1, " ", " ");
    // map from opcodes to source lines (debug information)
    // It is stored in Proto structure's lineinfo, as int* (an integer array)

    int numLineInfo = loadAndProceed<int>(&bytecodeAddr);
    for (int i = 0; i < numLineInfo; ++i)
    {
        o = bytecodeAddr - initBase;
        int to_line = loadAndProceed<int>(&bytecodeAddr);
        lt.push_back(
            o, 
            string_format("op code [%d]", i),
            string_format("source code line [%d]", to_line)
        );
    }

    lt.push_back(-1, " ", " ");
    lt.push_back(-1, " ", "**Debug Infos - Local Variables**");
    lt.push_back(-1, " ", " ");

    o = bytecodeAddr - initBase;
    int numLocVars = loadAndProceed<int>(&bytecodeAddr);
    lt.push_back(o, to_string(numLocVars), "Number of local variables");

    // local variable debug info is stored in Proto structure's LocVar *locvars
    // where LocVar is:
    // typedef struct LocVar {
    //   TString *varname;
    //   int startpc;   first point where variable is active
    //   int endpc;    first point where variable is dead
    // } LocVar;

    for (int i = 0; i < numLocVars; ++i)
    {
        // original dump code in ldump.c DumpDebug function for reference: 
        // DumpString(f->locvars[i].varname, D);
        // DumpInt(f->locvars[i].startpc, D);
        // DumpInt(f->locvars[i].endpc, D);

        o = bytecodeAddr - initBase;
        lt.push_back(
            o, 
            loadAndProceed<string>(&bytecodeAddr), 
            string_format("[%d] Name", i)
        );

        o = bytecodeAddr - initBase;
        lt.push_back(
            o, 
            to_string(loadAndProceed<int>(&bytecodeAddr)), 
            string_format("[%d] start pc", i)
        );

        o = bytecodeAddr - initBase;
        lt.push_back(
            o, 
            to_string(loadAndProceed<int>(&bytecodeAddr)), 
            string_format("[%d] end pc", i)
        );

    }

    lt.push_back(-1, " ", " ");
    lt.push_back(-1, " ", "**Debug Infos - Up Values Names**");
    lt.push_back(-1, " ", " ");

    o = bytecodeAddr - initBase;
    int numUpVals = loadAndProceed<int>(&bytecodeAddr);
    lt.push_back(o, to_string(numUpVals), "Number of up values");

    // by default, there is a _ENV, you could refer to lua-users.org/wiki/EnvironmentsTutorial

    for (int i = 0; i < numUpVals; ++i)
    {
        o = bytecodeAddr - initBase;
        lt.push_back(
            o, 
            loadAndProceed<string>(&bytecodeAddr), 
            string_format("[%d] Name", i)
        );
    }

    lt.push_back(-1, " ", " ");
    lt.push_back(o, " ", "End of table");

    lt.print_table();

    o = bytecodeAddr - initBase - sizeof(HeaderBlock);

    printf("\n");
    cout << "Table Size: " << o << endl;
}

void __dumpProtos(unsigned char* startAddr, string name, Proto * proot)
{
    unsigned char* baseAddr = startAddr;

    // source name
    loadAndProceed<string>(&baseAddr);

    // base
    baseAddr = baseAddr + sizeof(FuncBlock);

    // instruction list
    int numInstr = loadAndProceed<int>(&baseAddr);
    // lt.push_back(o, to_string(numInstr), "Number of instructions");

    for(int i = 0; i < numInstr; ++i)
    {
        Instruction asbly_code = loadAndProceed<Instruction>(&baseAddr);
        proot->ptdb.instrs.push_back(asbly_code);
    }

    // constant list
    int numConsant = loadAndProceed<int>(&baseAddr);

    for (int i = 0; i < numConsant; ++i)
    {
        int constType = loadAndProceed<unsigned char>(&baseAddr);
        int nonvarTag = lowByte(constType);
        int varTag    = hghByte(constType);

        switch(nonvarTag)
        {
            case 0:
                proot->ptdb.kdisplay.push_back("NIL");
                break;
            case 3:
                if (varTag == 0)
                {
                    proot->ptdb.kdisplay.push_back(to_string(loadAndProceed<lua_Number>(&baseAddr)));
                } else {
                    proot->ptdb.kdisplay.push_back(to_string(loadAndProceed<lua_Integer>(&baseAddr)));
                }
                break;
            case 4:
                proot->ptdb.kdisplay.push_back("\"" + loadAndProceed<string>(&baseAddr) + "\"");
                break;
            default:
                proot->ptdb.kdisplay.push_back("unknown");
                break;
        }
    }

    // up value lists
    int numUpVal = loadAndProceed<int>(&baseAddr);
    baseAddr += 2 * numUpVal;

    // nested function list
    int numNestedFuncs = loadAndProceed<int>(&baseAddr);

    for (int i = 0; i < numNestedFuncs; ++i)
    {
        proot->subprotos.emplace_back();
        __dumpProtos(
            baseAddr, 
            string_format("%s.%d", name.c_str(), i), 
            &proot->subprotos[proot->subprotos.size() - 1]
        );
        baseAddr += getFunctionBlockSize(baseAddr);
    }


    int numLineInfo = loadAndProceed<int>(&baseAddr);
    baseAddr += sizeof(int) * numLineInfo;


    int numLocVars = loadAndProceed<int>(&baseAddr);

    for (int i = 0; i < numLocVars; ++i)
    {
        proot->ptdb.locDisplay.push_back("\"" + loadAndProceed<string>(&baseAddr) + "\"");
        baseAddr += sizeof(int) * 2;
    }

    int numUpVals = loadAndProceed<int>(&baseAddr);

    // by default, there is a _ENV, you could refer to lua-users.org/wiki/EnvironmentsTutorial
    for (int i = 0; i < numUpVals; ++i)
    {
        proot->ptdb.upDisplay.push_back(loadAndProceed<string>(&baseAddr));
    }
}

void Dumped::printFunctionCompact(unsigned char* startAddr)
{
    Proto proot;

    if (!startAddr)
    {
        startAddr = bytecodeAddr + sizeof(HeaderBlock);
    }
    __dumpProtos(startAddr, "main", &proot);

    // display part
    proot.print(); 
}