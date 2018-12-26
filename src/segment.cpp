#include "segment.hpp"
using namespace smashpp;

void Segment::partition (ofstream& posF, float filtered) {
  if (filtered > thresh) {
    begun = false;
    if (begPos != endPos) {
      if (endPos - begPos >= minSize) {
        ++nSegs;
        posF << begPos << '\t' << endPos << '\t' 
             << std::fixed << setprecision(PREC_FIL) << sumEnt/numEnt << '\n';
      }
    }
    begPos=0, endPos=0, sumEnt=0, numEnt=0;
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

void Segment::partition_last (ofstream& posF) {
  if (begPos != endPos) {
    if (endPos - begPos >= minSize) {
      ++nSegs;
      posF << begPos << '\t' << endPos << '\t'
           << fixed_precision(PREC_FIL) << sumEnt/numEnt << '\n';
    }
  }
}