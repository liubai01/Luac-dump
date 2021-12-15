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

    if (ssize == 0)
    {
        *addr = nowAddr;
        return ret;
    }

    

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

unsigned char* openMmapRO(string filename)
{
    // open the dumped lua file
    int fd = open(filename.c_str(), O_RDONLY);
    if (fd == -1)
    {
        printf("Fail to open file.\n");
        return nullptr;
    }

    // mmap the dumped lua file to the virtual memory
    int fileSize = getFileSize(fd);
    unsigned char* fileBase = (unsigned char *) mmap (
        0, fileSize, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);

    return fileBase;
}

// https://stackoverflow.com/questions/1001307/detecting-endianness-programmatically-in-a-c-program
bool isBigEndian()
{
    union {
        uint32_t i;
        char c[4];
    } bint = {0x01020304};

    return bint.c[0] == 1; 
}

Instruction toBigEnd(Instruction instr)
{
    Instruction ret;

    if(!isBigEndian())
    {
        for (int i = 0; i < sizeof(ret); ++i)
        {
            ret = ret << 4 | (instr & 0xf);
            instr = instr >> 4;
        }
        
    }

    return ret;
}

vector<string> splitStr(string s, string delimiter) {
    vector<string> ret;
    
    auto start = 0U;
    auto end = s.find(delimiter);
    while (end != std::string::npos)
    {
        if (end - start > 0)
        {
            ret.push_back(s.substr(start, end - start));
        }
        
        start = end + delimiter.length();
        end = s.find(delimiter, start);
    }
    if (start < s.length())
    {
        if (s.length() - start + 1 > 0)
        {
            ret.push_back(s.substr(start, s.length() - start + 1));
        }
    }

    return ret;
}