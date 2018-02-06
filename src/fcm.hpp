//
// Created by morteza on 2/4/18.
//

#ifndef SMASHPP_FCM_HPP
#define SMASHPP_FCM_HPP

#include "par.hpp"

class FCM
{
public:
    FCM () = default;
    ~FCM () { delete tbl; }
    void buildModel (const Parameters&);
    void compress (const Parameters&);
    
    void printTable (const Parameters&); //todo. test
    
private:
    u64* tbl;
//    double* tbl;

    u64 countSyms (const string &);
};

#endif //SMASHPP_FCM_HPP