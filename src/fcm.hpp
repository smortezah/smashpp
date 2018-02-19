//
// Created by morteza on 2/4/18.
//

#ifndef SMASHPP_FCM_HPP
#define SMASHPP_FCM_HPP

#include "par.hpp"

class FCM
{
 public:
//  explicit FCM      (const Param&);
  FCM               () = default;
  ~FCM              ();
  void buildModel   (const Param&);
  void compress     (const Param&)  const;
  void printTbl     (const Param&)  const;
  void printHashTbl ()              const;
  
  char    mode;     //todo.test. to print in main()
 private:
//  char    mode;    // Table or Hash table
  double* tbl;
  htbl_t  htbl;
};

#endif //SMASHPP_FCM_HPP