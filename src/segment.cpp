//
// Created by morteza on 26-09-2018.
//

#include "segment.hpp"
#include "fn.hpp"
using namespace smashpp;

void Segment::partition (ofstream& posF, float filtered) {
  if (filtered > thresh) {
    begun = false;
    if (begPos != endPos) {
      ++nSegs;
      posF << begPos << '\t' << endPos << '\t' << sumEnt/numEnt << '\n';
    }
    begPos = endPos = 0;
    sumEnt = 0;
    numEnt = 0;
  }
  else {
    if (!begun) {
      begun  = true;
      begPos = pos;
    }
    endPos = pos;
    sumEnt += filtered;
    ++numEnt;
  }
}

void Segment::partition_last (ofstream& ff) {
  if (begPos != endPos) {
    ++nSegs;
    ff << begPos << '\t' << endPos << '\t' << sumEnt/numEnt << '\n';
  }
}