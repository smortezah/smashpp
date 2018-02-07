//
// Created by morteza on 2/4/18.
//

#ifndef SMASHPP_FCM_HPP
#define SMASHPP_FCM_HPP

#include "par.hpp"

class FCM //: public Parameters
{
public:
    explicit FCM (Parameters& p);
    ~FCM ();
////    void buildModel (const Parameters&);
////    void compress (const Parameters&);
//    void buildModel ();
//    void compress ();
//
//    void printTable (); //todo. test
////    void printTable (const Parameters&); //todo. test
    
private:
    char    mode;    // Table or Hash table
    double* tbl;
};

#endif //SMASHPP_FCM_HPP