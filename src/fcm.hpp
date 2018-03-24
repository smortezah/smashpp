//
// Created by morteza on 2/4/18.
//

#ifndef SMASHPP_FCM_HPP
#define SMASHPP_FCM_HPP

#include "par.hpp"
#include "tbl64.hpp"
#include "tbl32.hpp"
#include "logtbl8.hpp"
#include "cmls4.hpp"

struct ModelPar {
  u8    ir;      // Inverted repeat
  u8    k;       // Context size
  float alpha;
  u8    mode;    // Tbl 64, Tbl 32, LogTbl 8, Sketch 4
  u64   w;       // Width of count-min-log sketch
  u8    d;       // Depth of count-min-log sketch
};

class FCM    // Finite-context model
{
 public:
  explicit FCM    (const Param&);
  ~FCM            ();
  void buildModel (const Param&);          // Build FCM (finite-context model)
  void compress   (const Param&) const;
  
 private:
  vector<ModelPar> model;
  Table64*   tbl64;
  Table32*   tbl32;
  LogTable8* logtbl8;
  CMLS4*     sketch4;
  
  void setModels (const Param&);
  
  // Create data structure
  template <typename T, typename U>
  void createDS (const string&, T, U&);
  // Compress data structure
//  void compressDS1 (const string&) const;
  template <typename T, typename Y, typename U>
  void compressDS1 (const string&, const ModelPar&, T, Y&, const U&) const;

//  template <typename T>
//  void prob(T) const;
//  template <typename T>
//  T dtStruct (u8) const;
};

#endif //SMASHPP_FCM_HPP