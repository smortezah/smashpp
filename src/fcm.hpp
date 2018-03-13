//
// Created by morteza on 2/4/18.
//

#ifndef SMASHPP_FCM_HPP
#define SMASHPP_FCM_HPP

#include "par.hpp"
#include "cmls.hpp"

class FCM    // Finite-context model
{
 public:
  explicit FCM      (const Param&);
//  FCM               () = default;
  ~FCM              ();
  void buildModel   (const Param&);          // Build FCM (finite-context model)
//  void buildTable ();
//  void buildSketch ();
  void compress     (const Param&)  const;
  void printTbl     (const Param&)  const;
  void printHashTbl ()              const;
  
 private:
  double* tbl;
  htbl_t  htbl;
  CMLS*   skch;
};

#endif //SMASHPP_FCM_HPP