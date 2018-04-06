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
  Table64*         tbl64;
  Table32*         tbl32;
  LogTable8*       logtbl8;
  CMLS4*           sketch4;
  u8               MODE_COMB;
  u8               IR_COMB;
  
  void setModels    (const Param&); // Set models parameters
  void allocModels  ();             // Allocate memory to models
  void setModesComb ();             // Set combination of modes of models
  void setIRsComb   ();             // Set combination of inv. repeats of models
  void bldMdlOneThr (const Param&); // Build models one thread
  void bldMdlMulThr (const Param&); // Build models multiple threads
  // Create data structure
  template <typename mask_t, typename ds_t>
  void createDS (const string&, mask_t, ds_t&);
  // Compress data structure
  template <typename mask_t, typename ds_t>
  void compDS1 (const string&, mask_t, const ds_t&) const;
  template <typename mask0_t, typename mask1_t, typename ds0_t, typename ds1_t>
  void compDS2 (const string&, mask0_t,mask1_t,const ds0_t&,const ds1_t&) const;
//  template <typename mask0_t, typename mask1_t, typename mask2_t,
//    typename ds0_t, typename ds1_t, typename ds2_t>
//  void compDS3 (const string&, mask0_t, mask1_t, mask2_t, const ds0_t&,
//                const ds1_t&, const ds2_t&) const;
  
  
  template <typename coef_t>
//  double log2Prob (const coef_t[4], float, char) const;
  double log2Prob (coef_t, coef_t, coef_t, coef_t, float, char) const;
};

#endif //SMASHPP_FCM_HPP