#include "ldump.hpp"

#include "utils.hpp"
#include <cassert>
#include <iostream>

#include <math.h>

using namespace std;

#define LayoutTableStringWidth 20

Dumped::Dumped(unsigned char* bytecodeAddr)
{
    this->bytecodeAddr = bytecodeAddr;

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

void Dumped::printHeaderBlock()
{
    LayoutTable lt;


    // print the parsed header
    int o = 0;

    lt.push_back(-1, " ", " ");
    lt.push_back(-1, " ", "Lua Metadata");
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
    lt.push_back(-1, " ", "Lua Metadata(Sizes)");
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

    char buf[30];

    sprintf(buf, "%ld", hb.Lua_ExampleInt);
    lt.push_back(o, string(buf), "Example Lua Integer (0x5678=22146)");
    o += sizeof(hb.Lua_ExampleInt);

    sprintf(buf, "%lf", hb.Lua_ExampleNum);
    lt.push_back(o, string(buf), "Example Lua Number (370.5)");
    o += sizeof(hb.Lua_ExampleNum);

    lt.push_back(o, "0x" + sprintHex(hb.Lua_SizeUpvalues), "Size of Up Values");
    o += sizeof(hb.Lua_SizeUpvalues);

    lt.push_back(-1, " ", " ");
    lt.push_back(o, " ", "End of table");

    lt.print_table();
    cout << "Table Size: " << o << endl;
}

void LayoutTable::push_back(size_t offset, string content, string desc)
{
    offsets.push_back(offset);
    contents.push_back(content);
    descs.push_back(desc);
}

void LayoutTable::print_table()
{
    printf(
        "%10s  %20s  %20s\n", 
        "Offset", 
        "Content",
        "Desciption"
    );
    for (int i = 0; i < 54; ++i)
    {
        printf("=");
    }
    printf("\n");

    for (int i = 0; i < offsets.size(); ++i)
    {
        int o = offsets[i];

        int rows = ceil((double) max(contents[i].size(), descs[i].size()) / LayoutTableStringWidth);

        for (int ridx = 0; ridx < rows; ++ridx)
        {  
            string c;
            string d;

            if (ridx * LayoutTableStringWidth < contents[i].size())
            {
                c = contents[i].substr(ridx * LayoutTableStringWidth, LayoutTableStringWidth);
            } else {
                c = " ";
            }

            if (ridx * LayoutTableStringWidth < descs[i].size())
            {
                d = descs[i].substr(ridx * LayoutTableStringWidth, LayoutTableStringWidth);
            } else {
                d = " ";
            }

            if (ridx == 0 && o != -1)
            {
                printf("0x%08x", o);
            } else {
                // 10 spaces: "0x" + 8 chars for integer(4 bytes, 2 hex per byte)
                printf("          ");
            }
            printf(
                "  %-20s  %-20s\n", 
                c.c_str(), 
                d.c_str()
            );
        }

        
    }
}