#ifndef LDUMP_H // include guard
#define LDUMP_H

#include "lproto.hpp"
#include <vector>
#include <string>

using namespace std;

// Refer to A No-Frills Introduction to Lua 5.1 VM Instructions and lua source code

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

class Dumped {
public:
    Dumped(unsigned char* bytecodeAddr);

    unsigned char* bytecodeAddr;

    void printHeaderBlock();
    void printFunctionBlock(unsigned char* startAddr=nullptr);
    void printFunctionCompact(unsigned char* startAddr=nullptr, int lvl=0, int idx=0);

    vector<unsigned char*> subFuncsAddrs(unsigned char* startAddr=nullptr);
};

#endif