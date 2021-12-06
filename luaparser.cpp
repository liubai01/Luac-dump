#include <iostream>
#include <string>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cassert>

#include "src/utils.hpp"

using namespace std;



// Refer to A No-Frills Introduction to Lua 5.1 VM Instructions

#define lua_Integer  long
#define lua_Number   double
#define Instruction  unsigned int

#define LUA_TNUMFLT (3 | (0 << 4))  /* float numbers */
#define LUA_TNUMINT (3 | (1 << 4))  /* integer numbers */

#pragma pack(push,1)
typedef struct HeaderBlock {
    unsigned int     Lua_Signature;       // Header signature: ESC, “Lua” or 0x1B4C7561
    unsigned char    Lua_VerNumber;       // Version number, 0x53 for Lua 5.3
    unsigned char    Lua_FormatVer;       // Format version, 0=official version
    unsigned char    Lua_Data[6];         // Data To Catch Conversion Errors

    unsigned char    Lua_SizeInt;         // Size of integer (in bytes) (default 4)
    unsigned char    Lua_SizeSizeT;       // Size of size_t (in bytes) (default 8 on 64-bits OS)
    unsigned char    Lua_SizeInstr;       // Size of Instruction (in bytes) (default 4)
    unsigned char    Lua_SizeLuaInt;      // Size of lua_Integer (in bytes) (default 8)
    unsigned char    Lua_SizeLuaNum;      // Size of lua_Number (in bytes) (default 8)

    lua_Integer      Lua_ExampleInt;      // 0x5678
    lua_Number       Lua_ExampleNum;      // cast_num(370.5)

    unsigned char    Lua_SizeUpvalues;    // Size of up values (in bytes) (default 1)

} HeaderBlock;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct FuncBlock {
    int               Lua_LineDefined;
    int               Lua_LastLineDefined;

    unsigned char     Lua_NumParam;
    unsigned char     Lua_VarArgFlag;
    unsigned char     Lua_MaxStackSize;
} FuncBlock;
#pragma pack(pop)



void printHeaderBlock(unsigned char* fileBase)
{
    // parse the header by structure
    HeaderBlock hb;
    std::memcpy(&hb, fileBase, sizeof(HeaderBlock));

    // print the parsed header
    printf("Size of header block: %ld\n", sizeof(HeaderBlock));
    printf("=== Lua Metadata ===\n");
    printf("Lua Signature: ");
    printHex(hb.Lua_Signature);
    printf("\n");

    printf("Lua Version: ");
    printf("%d.%d", hghByte(hb.Lua_VerNumber), lowByte(hb.Lua_VerNumber));
    printf("\n");

    printf("Lua Format Version(Official=0): ");
    printf("%d", hb.Lua_FormatVer);
    printf("\n");

    printf("Lua Data(detect conversion error, \\x19\\x93\\r\\n\\x1a\\n): ");
    printHex(hb.Lua_Data, 6);
    printf("\n");

    printf("\n");


    printf("=== Lua Sizes ===\n");
    printf("Lua Size of Integer: ");
    printf("%d", hb.Lua_SizeInt);
    printf("\n");

    printf("Lua Size of size_t: ");
    printf("%d", hb.Lua_SizeSizeT);
    printf("\n");


    printf("Lua Size of Instruction: ");
    printf("%d", hb.Lua_SizeInstr);
    printf("\n");

    printf("Lua Size of Lua Integer: ");
    printf("%d", hb.Lua_SizeLuaInt);
    printf("\n");
    assert(static_cast<size_t>(hb.Lua_SizeLuaInt) == sizeof(lua_Integer));

    printf("Lua Size of Lua Number: ");
    printf("%d", hb.Lua_SizeLuaNum);
    printf("\n");

    printf("Lua Size of Up value: ");
    printf("%d", hb.Lua_SizeUpvalues);
    printf("\n");

    printf("\n");

    printf("Lua Size of Example Int(0x5678=22146): ");
    printf("%ld", hb.Lua_ExampleInt);
    printf("\n");

    printf("Lua Size of Example Num(370.5): ");
    printf("%lf", hb.Lua_ExampleNum);
    printf("\n");

    printf("\n");
}

void printFunctionBlock(unsigned char* fileBase)
{
    unsigned char* initBase = fileBase;
    fileBase = fileBase + sizeof(HeaderBlock);

    printf("=== Lua Function(chunk) Metadata===\n");
    printf("Source name: ");
    cout << loadAndProceed<string>(&fileBase) << endl;

    FuncBlock fb;
    std::memcpy(&fb, fileBase, sizeof(FuncBlock));
    fileBase = fileBase + sizeof(FuncBlock);

    printf("Function lind defined: ");
    cout << fb.Lua_LineDefined << endl;

    printf("Function last lind defined: ");
    cout << fb.Lua_LastLineDefined << endl;

    printf("Number of params: ");
    printf("%d", fb.Lua_NumParam);
    printf("\n");

    printf("`is_vararg` flag: ");
    printf("%d", fb.Lua_VarArgFlag);
    printf("\n");

    printf("Maximum stack size: ");
    printf("%d", fb.Lua_MaxStackSize);
    printf("\n");

    printf("\n=== Lua Top-level Function Code===\n");
    int numInstr = loadAndProceed<int>(&fileBase);
    printf("Number of instructions: %d\n", numInstr);

    for(int i = 0; i < numInstr; ++i)
    {
        Instruction asbly_code = static_cast<Instruction>(*fileBase);

        // 0x3f  : [0, 5 ] bits mask = 00000000111111
        // 0xfc0 : [7, 14] bits mask = 01111111000000
        printf("[instr %d] [optcode %2d] [A %2d]: 0x", i, asbly_code & 0x3f, asbly_code &0xfc0);
        printHex(asbly_code);
        printf("\n");
        fileBase += sizeof(int);
    }

    printf("\n=== Lua Top-level Constants===\n");
    int numConsant = loadAndProceed<int>(&fileBase);
    printf("Number of constants: %d\n", numConsant);

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
        int constType = loadAndProceed<unsigned char>(&fileBase);
        int nonvarTag = lowByte(constType);
        int varTag    = hghByte(constType);
        // printf("Offset In Bytes: %ld\n", fileBase - initBase);
        printf("Constant ID: %d\n", constType);
        switch(nonvarTag)
        {
            case 0:
                printf("Constant Type: NIL\n");
                break;
            case 1:
                printf("Constant Type: Boolean\n");
                printf("TBD\n");
                break;
            case 3:
                if (varTag == 0)
                {
                    printf("Constant Type: Number(Double)\n");
                    printf("Constant Content: %lf\n", loadAndProceed<lua_Number>(&fileBase));
                } else {
                    printf("Constant Type: Number(Integer)\n");
                    printf("Constant Content: %ld\n", loadAndProceed<lua_Integer>(&fileBase));
                }
                break;
            case 4:
                printf("Constant Type: String\n");
                printf("Constant Content: ");
                cout << loadAndProceed<string>(&fileBase) << endl;
                break;
            default:
                printf("Unknown Type\n");
                break;
        }
        printf("\n");
    }

    printf("=== Lua Up Values ===\n");
    int numUpVal = loadAndProceed<int>(&fileBase);
    // assert(numNestedFunc == 0);
    printf("Up Values Num: %d\n", numUpVal);
    for (int i = 0; i < numUpVal; ++i)
    {
        int instack = loadAndProceed<unsigned char>(&fileBase);  // whether it is in stack (register)
        int idx     = loadAndProceed<unsigned char>(&fileBase);  // index of upvalue (in stack or in outer function's list)

        printf("Up values[%d]: instack?: %d, idx: %d\n", i, instack, idx);
    }

    printf("\n=== Lua Nested Functions (Protos) ===\n");
    int numNestedFuncs = loadAndProceed<int>(&fileBase);
    printf("Nested Function Nums: %d\n", numNestedFuncs);
    assert (numNestedFuncs == 0); // TBD, for nested functions

    printf("\n=== Debug Infos - Line Info ===\n");
    printf("map from opcodes to source lines (debug information)\n");
    // It is stored in Proto structure's lineinfo, as int* (an integer array)

    int numLineInfo = loadAndProceed<int>(&fileBase);
    for (int i = 0; i < numLineInfo; ++i)
    {
        printf("opcode line [%d] -> source code line [%d]\n", i, loadAndProceed<int>(&fileBase));
    }

    printf("\n=== Debug Infos - Local Variables ===\n");
    int numLocVars = loadAndProceed<int>(&fileBase);
    printf("Number of local variables %d\n", numLocVars);

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

        cout << "[Name: " << loadAndProceed<string>(&fileBase) << "]" << " ";
        printf("start pc: %d, ", loadAndProceed<int>(&fileBase));
        printf("end pc: %d\n", loadAndProceed<int>(&fileBase));

    }

    printf("\n=== Debug Infos - Up Values Names ===\n");
    int numUpVals = loadAndProceed<int>(&fileBase);
    printf("Number of up values %d\n", numUpVals);

    // by default, there is a _ENV, you could refer to lua-users.org/wiki/EnvironmentsTutorial

    for (int i = 0; i < numUpVals; ++i)
    {

        cout << "[Name: " << loadAndProceed<string>(&fileBase) << "]" << endl;

    }
}

int main(int argc, char* argv[])
{
    string filename;
    if (argc == 2)
    {
        filename = string(argv[1]);

    } else {
        cout << "Require an input file!" << endl;
        return 0;
    }

    // open the dumped lua file
    int fd = open(filename.c_str(), O_RDONLY);
    if (fd == -1)
    {
        printf("Fail to open file.\n");
        return 1;
    }

    // mmap the dumped lua file to the virtual memory
    int fileSize = getFileSize(fd);
    unsigned char* fileBase = (unsigned char *) mmap (
        0, fileSize, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);

    printHeaderBlock(fileBase);
    printFunctionBlock(fileBase);

    return 0;
}