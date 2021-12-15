#include "lproto.hpp"
#include "linstr.hpp"

#include <iostream>
#include <string>
#include <math.h>

using namespace std;


void Proto::print(int lvl, int idx, string prompt)
{
    ParserInstr pInstr;

    cout << prompt;
    printf("**function %d (lvl. %d)**\n", idx, lvl);
    
    prompt += "| ";
    cout << prompt << endl;

    cout << prompt;
    printf(".const\n");
    for (int i = 0; i < ptdb.kdisplay.size(); ++i)
    {
        cout << prompt;
        printf("[%d]  %s\n", i, ptdb.kdisplay[i].c_str());
    }
    cout << prompt << endl;

    cout << prompt;
    printf(".local\n");
    for (int i = 0; i < ptdb.locDisplay.size(); ++i)
    {
        cout << prompt;
        printf("[%d]  %s\n", i, ptdb.locDisplay[i].c_str());
    }
    cout << prompt << endl;

    cout << prompt;
    printf(".upvalue\n");
    for (int i = 0; i < ptdb.upDisplay.size(); ++i)
    {
        cout << prompt;
        printf("[%d]  %s\n", i, ptdb.upDisplay[i].c_str());
    }
    cout << prompt << endl;

    for (int i = 0; i < subprotos.size(); ++i)
    {
        auto& p = subprotos[i];
        p.print(lvl + 1, i, prompt + "    ");
        cout << prompt << endl;
    }

    cout << prompt;
    printf(".instructions\n");
    cout << prompt << endl;
    for(size_t i = 0; i < ptdb.instrs.size(); ++i)
    {
        Instr* instrObj = pInstr.parseInstr(ptdb.instrs[i]);
        vector<string> comments = splitStr(instrObj->comment(ptdb.instrs[i], ptdb), "\n");

        for (size_t j = 0; j < comments.size(); ++j)
        {
            string cmt = comments[j];

            if (j)
            {
                cout << prompt;
                cout << "    ";
                cout << "        " << "    ";
                cout << "            ";
            } else {
                cout << prompt;
                cout << "[" << i << "] ";
                cout << sprintHex(toBigEnd(ptdb.instrs[i])) << "    ";
                printf("%10s  ", instrObj->name.c_str());
            }
            
            printf("%s", cmt.c_str());

            cout << endl;
        }

    }
    cout << prompt << endl;

    cout << prompt.substr(0, prompt.size() - 2);
    printf("**end of func. [%d] (lvl. %d)**\n", idx, lvl);
}