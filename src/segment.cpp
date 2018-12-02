#include "segment.hpp"
using namespace smashpp;

void Segment::partition (ofstream& posF, float filtered) {
  if (filtered > thresh) {
    begun = false;
    if (begPos != endPos) {
      ++nSegs;
      posF << begPos << '\t' << endPos << '\t' 
           << std::fixed << setprecision(FIL_PREC) << sumEnt/numEnt << '\n';
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
    ff << begPos << '\t' << endPos << '\t'
       << std::fixed << setprecision(FIL_PREC) << sumEnt/numEnt << '\n';
  }
}