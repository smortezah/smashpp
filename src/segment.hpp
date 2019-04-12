// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.

#ifndef SMASHPP_SEGMENT_HPP
#define SMASHPP_SEGMENT_HPP

#include <fstream>
// #include "def.hpp"
#include "par.hpp"
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
  int16_t beg_guard;
  int16_t end_guard;

  Segment()
      : begun(false),
        pos(0),
        begPos(0),
        endPos(0),
        nSegs(0),
        thresh(0),
        sumEnt(0),
        numEnt(0),
        minSize(MIN_SSIZE)
        {}

  void partition(std::vector<PosRow>&, float);
  void partition_last(std::vector<PosRow>&);
  void set_guards(uint8_t, int16_t, int16_t);
};
}  // namespace smashpp

#endif  // SMASHPP_SEGMENT_HPP