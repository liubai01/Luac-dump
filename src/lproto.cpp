#include "lproto.hpp"

#include <iostream>
#include <string>


void Proto::print(int lvl, int idx, string prompt)
{
    cout << prompt;
    printf("**function %d (lvl. %d)**\n", idx, lvl);
    
    prompt += "| ";
    cout << prompt << endl;

    cout << prompt;
    printf(".const\n");
    for (int i = 0; i < kdisplay.size(); ++i)
    {
        cout << prompt;
        printf("[%d]  %s\n", i, kdisplay[i].c_str());
    }
    cout << prompt << endl;

    cout << prompt;
    printf(".local\n");
    for (int i = 0; i < locDisplay.size(); ++i)
    {
        cout << prompt;
        printf("[%d]  %s\n", i, locDisplay[i].c_str());
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
    for(size_t i = 0; i < instrs.size(); ++i)
    {
        cout << prompt;
        cout << "[" << i << "] ";
        cout << sprintHex(instrs[i]) << endl;
    }
    cout << prompt << endl;

    cout << prompt.substr(0, prompt.size() - 2);
    printf("**end of func. [%d] (lvl. %d)**\n", idx, lvl);
}