#ifndef PROJECT_SEGMENT_HPP
#define PROJECT_SEGMENT_HPP

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
  uint8_t maxCtx;

  Segment () : begun(false), pos(0), begPos(0), endPos(0), nSegs(0), thresh(0),
    sumEnt(0), numEnt(0), minSize(MIN_SSIZE) {}
    
  void partition (std::ofstream&, float);
  void partition_last (std::ofstream&);
};
}

#endif //PROJECT_SEGMENT_HPP