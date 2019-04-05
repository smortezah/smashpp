// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.

#ifndef SMASHPP_SEGMENT_HPP
#define SMASHPP_SEGMENT_HPP

#include <fstream>
#include "def.hpp"
#include "number.hpp"

namespace smashpp {
class Segment {
 public:
  bool begun;
  uint64_t pos;
  uint64_t begPos;
  uint64_t endPos;
  uint64_t nSegs;
  float thresh;
  prc_t sumEnt;
  uint64_t numEnt;
  uint32_t minSize;
  uint64_t totalSize;
  uint64_t begGuard;
  uint64_t endGuard;

  Segment()
      : begun(false),
        pos(0),
        begPos(0),
        endPos(0),
        nSegs(0),
        thresh(0),
        sumEnt(0),
        numEnt(0),
        minSize(MIN_SSIZE) {}

  void partition(std::ofstream&, float);
  void partition_last(std::ofstream&);
  void set_guards(uint8_t);
};
}  // namespace smashpp

#endif  // SMASHPP_SEGMENT_HPP