//
// Created by morteza on 26-09-2018.
//

#ifndef PROJECT_SEGMENT_HPP
#define PROJECT_SEGMENT_HPP

#include <fstream>
#include "def.hpp"

class Segment {
 public:
  bool   begun;
  u64    pos;
  u64    begPos;
  u64    endPos;
  u64    nSegs;
  float  thresh;
  prec_t sumEnt;
  u64    numEnt;

  Segment () : begun(false), pos(0), begPos(0), endPos(0), nSegs(0),
               thresh(0), sumEnt(0), numEnt(0) {}

  void partition      (ofstream&, float);
  void partition_last (ofstream&);
};

#endif //PROJECT_SEGMENT_HPP