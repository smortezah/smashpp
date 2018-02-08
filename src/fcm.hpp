//
// Created by morteza on 2/4/18.
//

#ifndef SMASHPP_FCM_HPP
#define SMASHPP_FCM_HPP

#include "par.hpp"

class FCM
{
public:
    char    mode;    //todo.test. Table or Hash table
    
    
    explicit FCM (const Parameters& p);
    ~FCM ();
    void buildModel (const Parameters&);
    void compress (const Parameters&) const;
    
    void printTbl (const Parameters&) const; //todo.test
    void printHashTbl () const; //todo.test
    
private:
//    char    mode;    // Table or Hash table
    double* tbl;
    htbl_t  htbl;
};

#endif //SMASHPP_FCM_HPP