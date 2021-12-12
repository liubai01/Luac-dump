#include "lytbl.hpp"
#include "utils.hpp"

#include <iostream>
#include <math.h>

using namespace std;

void LayoutTable::push_back(size_t offset, string content, string desc)
{
    offsets.push_back(offset);
    contents.push_back(content);
    descs.push_back(desc);
}

void LayoutTable::print_table()
{   
    int owidth = 10;
    int cwidth = 12;
    int dwidth = 36;
    vector<int> widths = {owidth, cwidth, dwidth};
    string fmt = "";
    for (int i = 0; i < widths.size() - 1; ++i)
    {
        fmt += "| %" + string_format("-%d", widths[i]) + "s ";
    }
    fmt += "| %" + string_format("-%d", widths[widths.size() - 1]) + "s |\n";


    printf(
        fmt.c_str(), 
        "Offset", 
        "Content",
        "Desciption"
    );

    for (int i = 0; i < widths.size(); ++i)
    {
        printf("| ");
        for (int j = 0; j < widths[i]; ++j)
        {
            printf("-");
        }
        printf(" ");
    }

    printf("|\n");


    for (int i = 0; i < offsets.size(); ++i)
    {
        int o = offsets[i];


        int rows = ceil(
            max(
                ((double) contents[i].size()) / widths[1], 
                ((double) descs[i].size()) / widths[2]
            )
        ); 

        string ofmt = "| 0x%" + string_format("0%dx", widths[0] - 2);
        string cdfmt = " | %-" + to_string(widths[1]) + "s | %-" + to_string(widths[2]) + "s | \n";

        for (int ridx = 0; ridx < rows; ++ridx)
        {  
            // take the substring that fits to this output row
            string c;
            string d;

            if (ridx * widths[1] < contents[i].size())
            {
                c = contents[i].substr(ridx * widths[1], widths[1]);
            } else {
                c = " ";
            }

            if (ridx * widths[2] < descs[i].size())
            {
                d = descs[i].substr(ridx * widths[2], widths[2]);
            } else {
                d = " ";
            }

            // output the offset
            if (ridx == 0 && o != -1)
            {
                printf(ofmt.c_str(), o);
            } else {
                printf("| ");
                for (int i = 0; i < widths[0]; ++i)
                {
                    printf(" ");
                }
            }

            // output content and description
            printf(
                cdfmt.c_str(), 
                c.c_str(), 
                d.c_str()
            );
        }

        
    }
}