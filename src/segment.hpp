#ifndef PROJECT_SEGMENT_HPP
#define PROJECT_SEGMENT_HPP

#include <fstream>
#include "def.hpp"

namespace smashpp {
class Segment {
 public:
  bool  begun;
  u64   pos, begPos, endPos;
  u64   nSegs;
  float thresh;
  prc_t sumEnt;
  u64   numEnt;

  Segment () : begun(false), pos(0), begPos(0), endPos(0), nSegs(0), thresh(0),
    sumEnt(0), numEnt(0) {}
    
  void partition (ofstream&, float);
  void partition_last (ofstream&);
};
}

#endif //PROJECT_SEGMENT_HPP