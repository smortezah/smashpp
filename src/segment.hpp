//
// Created by morteza on 26-09-2018.
//

#ifndef PROJECT_SEGMENT_HPP
#define PROJECT_SEGMENT_HPP

#include <fstream>
#include "def.hpp"

class Segment {
 public:
  bool  begun;
  u64   pos;
  u64   begPos;
  u64   endPos;
  float sum;
  float cut;
  u64   nSegs;

  Segment () : begun(false), pos(0), begPos(0), endPos(0), sum(0), cut(0),
               nSegs(0) {}

  void partition      (ofstream&);
  void partition_last (ofstream&);
//  void extract_seg (const string& fIn, const string& fOut, u64 begPos, u64 endPos);
};

#endif //PROJECT_SEGMENT_HPP