#include <iostream>
#include <string>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

#include "src/utils.hpp"

using namespace std;

#define highDig(X) ((X & 0xf0) >> 4)
#define lowDig(X) (X & 0x0f)

// Refer to A No-Frills Introduction to Lua 5.1 VM Instructions

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

    unsigned long long Lua_ExampleInt;    // 0x5678 (little endian)
    double Lua_ExampleNum;                // cast_num(370.5)


    // unsigned char Lua_InternalFlag; // Integral flag (default 0), 0=floating-point, 1=integral number type
} HeaderBlock;
#pragma pack(pop)



int main(int argc, char* argv[])
{
    string filename;
    if (argc == 2)
    {
        // open the dumped lua file
        filename = string(argv[1]);

        int fd = open(filename.c_str(), O_RDONLY);
        if (fd == -1)
        {
            printf("Fail to open file.\n");
            return 1;
        }

        // mmap the dumped lua file to the virtual memory
        int fileSize = getFileSize(fd);
        unsigned char* fileAddr = (unsigned char *) mmap (
            0, fileSize, PROT_READ, MAP_PRIVATE, fd, 0);
        close(fd);

        // parse the header by structure
        HeaderBlock hb;
        std::memcpy(&hb, fileAddr, sizeof(HeaderBlock));

        // print the parsed header
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

        printf("Lua Size of Lua Number: ");
        printf("%d", hb.Lua_SizeLuaNum);
        printf("\n");

        printf("\n");

        printf("Lua Size of Example Int: ");
        printHexLittleEndian(hb.Lua_ExampleInt);
        printf("\n");

        printf("Lua Size of Example Num: ");
        printf("%lf", hb.Lua_ExampleNum);
        printf("\n");

    } else {
        cout << "Require an input file!" << endl;
        return 0;
    }
    

    return 0;
}