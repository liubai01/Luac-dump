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
#include "src/lobject.hpp"

using namespace std;

void printUsage()
{
    cout << "Usage: ./luacdump <file> <options>"                             << endl;
    cout << "  -h                     Print header block memory layout"      << endl;
    cout << "  -f                     Print function block memory layout"    << endl;
    cout << "  -i                     Print function block intergrated info" << endl;
}

int main(int argc, char* argv[])
{
    string filename;
    if (argc > 1)
    {
        filename = string(argv[1]);

    } else {
        printUsage();
        return -1;
    }

    unsigned char* fileBase = openMmapRO(filename);
    if (!fileBase)
    {
        return -1;
    }

    Dumped d(fileBase);
    char c;
    bool hasOption = false;

    while ((c = getopt (argc - 1, argv + 1, "hfi")) != -1)
    {
        switch (c)
        {
            case 'h':
                // print header
                hasOption = true;
                d.printHeaderBlock();
                break;
            case 'f':
                hasOption = true;
                {
                    vector<unsigned char*> addrs{nullptr};
                    vector<int> funcDepth{0};
                    int p = 0;

                    while (p != addrs.size())
                    {
                        vector<unsigned char*> subs = d.subFuncsAddrs(addrs[p]);
                        funcDepth.push_back(funcDepth[p] + 1);
                        for (auto& s: subs)
                        {
                            addrs.push_back(s);
                        }
                        printf("\nFunction[%d]: Level %d\n", p, funcDepth[p]);
                        d.printFunctionBlock(addrs[p]);
                        ++p;
                    }
                }
                break;
            case 'i':
                hasOption = true;
                d.printFunctionCompact();
                break;
            case ':':
            // requires an option
                printf("requires an option\n");
                printUsage();
                return -1;
                break; 
            case '?':
            // unknown option
                printf("unknown option\n");
                printUsage();
                return -1;
                break; 
        }
    }

    // by default output compacted info
    if (!hasOption)
    {
        d.printFunctionCompact();
    }


    return 0;
}