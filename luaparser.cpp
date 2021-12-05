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

#define highDig(X) ((X & 0xf0) >> 4)
#define lowDig(X) (X & 0x0f)

// Refer to A No-Frills Introduction to Lua 5.1 VM Instructions

#define lua_Integer long
#define Instruction unsigned int

#pragma pack(push,1)
typedef struct HeaderBlock {
    unsigned int     Lua_Signature;    // Header signature: ESC, “Lua” or 0x1B4C7561
    unsigned char    Lua_VerNumber;    // Version number, 0x51 (81 decimal) for Lua 5.1
    unsigned char    Lua_FormatVer;    // Format version, 0=official version
    unsigned char    Lua_Data[6];      // Data To Catch Conversion Errors

    unsigned char Lua_SizeInt;         // Size of integer (in bytes) (default 4)
    unsigned char Lua_SizeSizeT;       // Size of size_t (in bytes) (default 8 on 64-bits OS)
    unsigned char Lua_SizeInstr;       // Size of Instruction (in bytes) (default 4)
    unsigned char Lua_SizeLuaInt;      // Size of lua_Integer (in bytes) (default 8)
    unsigned char Lua_SizeLuaNum;      // Size of lua_Number (in bytes) (default 8)

    lua_Integer Lua_ExampleInt;           // 0x5678 (little endian)
    double Lua_ExampleNum;                // cast_num(370.5)

    unsigned char Lua_SizeUpvalues;    // Size of up values (in bytes) (default 1)

} HeaderBlock;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct FuncBlock {
    int               Lua_LineDefined;
    int               Lua_LastLineDefined;
    // unsigned char     Lua_NumUpVal;
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
    printf("%d.%d", highDig(hb.Lua_VerNumber), lowDig(hb.Lua_VerNumber));
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

    printf("Lua Size of Example Int: ");
    printHexLittleEndian(hb.Lua_ExampleInt);
    printf("\n");

    printf("Lua Size of Example Num: ");
    printf("%lf", hb.Lua_ExampleNum);
    printf("\n");

    printf("\n");
}

void printFunctionBlock(unsigned char* fileBase)
{
    fileBase = fileBase + sizeof(HeaderBlock);

    printf("=== Lua Top-level function(chunk) metadata===\n");
    printf("Source name: ");
    cout << loadString(&fileBase) << endl;

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

    printf("\n=== Lua Top-level function code===\n");
    int numInstr = loadInt(&fileBase);
    printf("Number of instructions: %d\n", numInstr);

    for(int i = 0; i < numInstr; ++i)
    {
        Instruction asbly_code = static_cast<Instruction>(*fileBase);

        printf("[instr %d] [optcode %2d] [A %2d]: 0x", i, asbly_code & 0x3f, asbly_code &0xfc0);
        printHex(asbly_code);
        printf("\n");
        fileBase += sizeof(int);
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