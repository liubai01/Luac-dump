#ifndef UTILS_H // include guard
#define UTILS_H

#include <string>
#include <cstring>

using namespace std;

int getFileSize(int fd);


void printHex(unsigned int ui);

void printHex(unsigned long long ui);

void printHex(unsigned char* str, int s);


void printHexLittleEndian(unsigned long long ui);


string loadString(unsigned char** addr);

int loadInt(unsigned char** addr);

unsigned char loadByte(unsigned char** addr);


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