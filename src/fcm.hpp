//
// Created by morteza on 2/4/18.
//

#ifndef SMASHPP_FCM_HPP
#define SMASHPP_FCM_HPP

#include "par.hpp"
#include "tbl.hpp"
#include "cmls.hpp"

class FCM    // Finite-context model
{
 public:
  explicit FCM      (const Param&);
  ~FCM              ();
  void buildModel   (const Param&);          // Build FCM (finite-context model)
  void compress     (const Param&)  const;
  void printTbl     (const Param&)  const;
  void printHashTbl ()              const;
  
 private:
  Table* tbl;
  CMLS*  skch;
  htbl_t htbl;
  
//  void buildTbl ();
//  void buildSkch ();
//  void compressTbl ();
//  void compressSkch ();
//void parseLevel ();
};

#endif //SMASHPP_FCM_HPP