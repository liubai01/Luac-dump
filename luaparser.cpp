#include <iostream>
#include <string>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cassert>

#include "src/utils.hpp"
#include "src/ldump.hpp"

using namespace std;


int main(int argc, char* argv[])
{
    string filename;
    if (argc == 2)
    {
        filename = string(argv[1]);

    } else {
        cout << "Require an input file!" << endl;
        return 0;
    }

    unsigned char* fileBase = openMmapRO(filename);
    if (!fileBase)
    {
        return 1;
    }

    Dumped d(fileBase);
    d.printHeaderBlock();
    d.printFunctionBlock();
    // cout << getFunctionBlockSize(fileBase + sizeof(HeaderBlock)) << endl;

    return 0;
}