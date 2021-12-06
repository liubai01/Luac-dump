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

// specialization for print in hex
void printHex(unsigned char* str, int s)
{
    for (int i = 0; i < s; ++i)
    {
        printChar(str[i]);
    }
}

string sprintHex(unsigned char* str, int s)
{
    string ret = "";
    char buf[3];
    buf[2] = '\0';

    for (int i = 0; i < s; ++i)
    {
        sprintf(buf, "%x%x", (str[i] & 0xF0) >> 4, str[i] & 0x0F);
        ret += string(buf);
    }
    
    return ret;
}


// specialization for load and proceed
template<>
string loadAndProceed(unsigned char** addr)
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


// more efficient
template<>
unsigned char loadAndProceed(unsigned char** addr)
{
    unsigned ret = (*addr)[0];
    ++(*addr);

    return ret;
}
