//
// Created by morteza on 2/4/18.
//

#ifndef SMASHPP_FCM_HPP
#define SMASHPP_FCM_HPP

#include <memory>
#include "par.hpp"
#include "tbl64.hpp"
#include "tbl32.hpp"
#include "logtbl8.hpp"
#include "cmls4.hpp"

struct STMMPar;

struct MMPar {
  u8    k;         // Context size
  u64   w;         // Width of count-min-log sketch
  u8    d;         // Depth of count-min-log sketch
  u8    ir;        // Inverted repeat
  float alpha;
  float gamma;
  Container cont;  // Tbl 64, Tbl 32, LogTbl 8, Sketch 4
  shared_ptr<STMMPar> child;
  MMPar (u8, u64, u8, u8, float, float);
  MMPar (u8, u8, float, float);
};

struct STMMPar {
  u8    k;
  u8    thresh;
  u8    ir;
  float alpha;
  float gamma;
  u8    miss;
  bool  enabled;
//  shared_ptr<MMPar> parent; //todo. check if necessary
  STMMPar (u8, u8, u8, float, float);
};

struct ProbPar {
  float  alpha;
  double sAlpha;
  u64    mask;
  u8     shl;
  u64    l;
  u8     numSym;
  u64    r;
  u8     revNumSym;
  ProbPar () = default;
  ProbPar (float, u64, u8);
  void config (char, u64);
  void config (char, u64, u64);
};

class FCM    // Finite-context models
{
 public:
  double aveEnt;
  
  explicit FCM (const Param&);
  void store (const Param&);   // Build FCM (finite-context models)
  void compress (const Param&);
////  void report     (const Param&) const;

 private:
  vector<MMPar>                 Ms;     // Markov models
  vector<STMMPar>               TMs;    // Tolerant Markov models
  vector<shared_ptr<Table64>>   tbl64;
  vector<shared_ptr<Table32>>   tbl32;
  vector<shared_ptr<LogTable8>> lgtbl8;
  vector<shared_ptr<CMLS4>>     cmls4;
  
  void config (const Param&); // Set models parameters
  template <class InIter, class Vec>  //Split by dlim
  void split (InIter, InIter, char, Vec&) const;
  void set_cont ();
  void alloc_model ();            // Allocate memory to models
  void store_1 (const Param&);    // Build models one thread
  void store_n (const Param&);    // Build models multiple threads
  template <class Mask, class CnerIter>
  void store_impl (const string&, Mask, CnerIter);  // Fill data structure
  // Compress data structure
  template <class CnerIter>
  void compress_1 (const string&, CnerIter);        // 1 Markov models
  void compress_n (const string&);
  template <class CnerIter, class ProbParIter>
  double prob (CnerIter, ProbParIter) const;  // Probability
  template <class CnerIter, class ProbParIter>
  double probIr (CnerIter, ProbParIter) const;  // Prob. IR
  
  template <class CnerIter, class ProbParIter>
  bool is_tm_enabled(CnerIter, ProbParIter);//todo const
  
  template <class CnerIter, class ProbParIter>
  double prob_best (CnerIter, ProbParIter) const;  // Probability
  template <class CnerIter, class ProbParIter>
  double probIr_best (CnerIter, ProbParIter) const;  // Prob. IR
  
  
  double entropy (double) const;
  template <class OutIter, class InIter>
  double entropy (OutIter, InIter, InIter) const;
  template <class OutIter, class InIter>
  void update_weights (OutIter, InIter, InIter) const;
  template <class OutIter, class InIter>
  void normalize (OutIter, InIter, InIter) const;
  template <class ProbParIter>
  void update_ctx (u64&, ProbParIter) const;
  template <class ProbParIter>
  void update_ctx (u64&, u64&, ProbParIter) const;
};

#endif //SMASHPP_FCM_HPP