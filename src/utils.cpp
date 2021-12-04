#include "utils.hpp"

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

#include <iostream>

using namespace std;

int getFileSize(int fd)
{
    struct stat s;
    fstat (fd, &s);

    return s.st_size;
}

void printChar(unsigned char c)
{
    printf("%x", (c & 0xF0) >> 4);
    printf("%x", c & 0x0F);
    return;
}

void printHex(unsigned int ui)
{
    unsigned char* tmp = reinterpret_cast<unsigned char*>(&ui);
    for (int i = 0; i < 4; ++i)
    {
        printChar(tmp[i]);
    }
}

void printHex(unsigned long long ui)
{
    unsigned char* tmp = reinterpret_cast<unsigned char*>(&ui);
    for (int i = 0; i < 8; ++i)
    {
        printChar(tmp[i]);
    }
}

void printHexLittleEndian(unsigned long long ui)
{
    // assume machine is big endian
    unsigned char* tmp = reinterpret_cast<unsigned char*>(&ui);
    for (int i = 7; i >= 0; --i)
    {
        printChar(tmp[i]);
    }
}

void printHex(unsigned char* str, int s)
{
    for (int i = 0; i < s; ++i)
    {
        printChar(str[i]);
    }
}

string loadString(unsigned char** addr)
{
    unsigned char* nowAddr = *addr;
    string ret = "";

    // Assume here string size is < 0xFF
    int ssize = nowAddr[0];
    nowAddr += 1;

    // lua 5.3 truncate the zero
    ret.append(reinterpret_cast<const char*>(nowAddr), ssize - 1);
    nowAddr += ssize - 1;

    *addr = nowAddr;
    return ret;
}

int loadInt(unsigned char** addr)
{
    unsigned char* nowAddr = *addr;
    int ret = 0;
    unsigned char* toAddr = reinterpret_cast<unsigned char*>(&ret);

    std::memcpy(&ret, nowAddr, sizeof(int));

    nowAddr += sizeof(int);

    *addr = nowAddr;
    return ret;
}