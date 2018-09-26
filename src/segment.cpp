//
// Created by morteza on 26-09-2018.
//

#include "segment.hpp"

inline void Segment::partition (ofstream& ff) {
  if (sum > cut) {
    begun = false;
    if (begPos != endPos) {
      ++nParts;
      ff << begPos << '\t' << endPos << '\n';
    }
    begPos = endPos = 0;
  }
  else {
    if (!begun) {
      begun  = true;
      begPos = pos;
    }
    endPos = pos;
  }
}

inline void Segment::partition_last (ofstream& ff) {
  if (begPos != endPos) {
    ++nParts;
    ff << begPos << '\t' << endPos << '\n';
  }
}