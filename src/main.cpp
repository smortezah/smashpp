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
//#include "def.hpp"
//#include "par.hpp"
#include "fcm.hpp"
using std::cout;


int main (int argc, char* argv[])
{
    auto* p = new Parameters;
//    auto* m = new FCM;
    
    p->parse(argc, argv);
    
    
//    m->buildModel(*p);
    
    
    delete p;
//    delete m, p;
    
    return 0;
}