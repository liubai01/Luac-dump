#ifndef UTILS_H // include guard
#define UTILS_H

#include <string>
#include <cstring>

#include <stdexcept>
#include <memory>

using namespace std;

#define hghByte(X) ((X & 0xf0) >> 4)     // second half if bytes (high half byte, bit 2, 3)
#define lowByte(X) (X & 0x0f)            // first half of bytes  (low half byte, bit 0, 1)

int getFileSize(int fd);

// https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
template<typename ... Args>
std::string string_format(const std::string& format, Args ... args )
{
    int size_s = std::snprintf(nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
    auto size = static_cast<size_t>( size_s );
    auto buf = std::make_unique<char[]>( size );
    std::snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}

void printChar(unsigned char c);

unsigned char* openMmapRO(string filename);

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