#ifndef UTILS_H // include guard
#define UTILS_H

#include <string>
#include <cstring>

using namespace std;

#define hghByte(X) ((X & 0xf0) >> 4)     // second half if bytes (high half byte, bit 2, 3)
#define lowByte(X) (X & 0x0f)            // first half of bytes  (low half byte, bit 0, 1)

int getFileSize(int fd);

void printChar(unsigned char c);


template<class T>
void printHex(T ui)
{
    unsigned char* tmp = reinterpret_cast<unsigned char*>(&ui);
    for (size_t i = 0; i < sizeof(ui); ++i)
    {
        printChar(tmp[i]);
    }
}

template<class T>
string sprintHex(T ui)
{
    string ret = "";
    char* tmp = reinterpret_cast<char*>(&ui);
    char buf[3];
    buf[2] = '\0';

    for (size_t i = 0; i < sizeof(ui); ++i)
    {
        sprintf(buf, "%x%x", (tmp[i] & 0xF0) >> 4, tmp[i] & 0x0F);
        ret += string(buf);
    }

    return ret;
}

void printHex(unsigned char* str, int s);
string sprintHex(unsigned char* str, int s);

// non-specialized template must be visible to a translation unit that uses it.
template<class T>
T loadAndProceed(unsigned char** addr)
{
    unsigned char* nowAddr = *addr;
    T ret;

    memcpy(&ret, nowAddr, sizeof(T));

    nowAddr += sizeof(T);

    *addr = nowAddr;
    return ret;
}

#endif