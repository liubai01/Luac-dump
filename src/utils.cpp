#include "utils.hpp"

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

#include <iostream>

int getFileSize(int fd)
{
    struct stat s;
    fstat (fd, &s);

    return s.st_size;
}

void printChar(unsigned char c)
{
    printf("%x", c);
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