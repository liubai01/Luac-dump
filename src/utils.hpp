#ifndef UTILS_H // include guard
#define UTILS_H

int getFileSize(int fd);


void printHex(unsigned int ui);

void printHex(unsigned long long ui);

void printHex(unsigned char* str, int s);


void printHexLittleEndian(unsigned long long ui);



#endif