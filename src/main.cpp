//
// Created by morteza on 02-02-2018.
//

#define __STDC_FORMAT_MACROS
#if defined(_MSC_VER)
#    include <io.h>
#else
#    include <unistd.h>
#endif

#include <iostream>
#include "def.hpp"
#include "par.hpp"
using std::cout;

int main (int argc, char* argv[])
{
    auto* p=new Parameters;
    p->parse(argc, argv);
    
    cout << p->tar << (int)p->nthr << p->ref;
    
    delete p;   // Parse cmd
    
    return 0;
}