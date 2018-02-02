//
// Created by morteza on 02-02-2018.
//

#ifndef SMASHPP_PAR_HPP
#define SMASHPP_PAR_HPP

#include "def.hpp"

class Parameters    // Command line
{
public:
    string tar;
    string ref;
    bool   verbose;
    u8     nthr;
    bool   help;
    
    Parameters () : verbose(false), nthr(DEF_THR), help(false) {};
    inline void parse (int&, char**&);
};

void Parameters::parse (int& argc, char**& argv)
{
    if(argc < 2) {
        help();
    }
}

#endif //SMASHPP_PAR_HPP