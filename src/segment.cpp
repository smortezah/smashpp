#include "segment.hpp"
using namespace smashpp;

void Segment::partition (std::ofstream& posF, float filtered) {
  const uint64_t begGuard {maxCtx + PART_GUARD};
  const uint64_t endGuard {PART_GUARD};

  if (filtered > thresh) {
    begun = false;
    if (begPos != endPos) {
      if (endPos - begPos >= minSize) {
        ++nSegs;
        posF << (begPos < begGuard ? 0 : begPos-begGuard) << '\t'
             << endPos+endGuard << '\t'
             << std::fixed << std::setprecision(PREC_FIL) << sumEnt/numEnt 
             << '\n';
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

void Segment::partition_last (std::ofstream& posF) {
  const uint64_t begGuard {maxCtx + PART_GUARD};
  const uint64_t endGuard {PART_GUARD};

  if (begPos != endPos) {
    if (endPos - begPos >= minSize) {
      ++nSegs;
      posF << (begPos < begGuard ? 0 : begPos-begGuard) << '\t'
           << endPos+endGuard << '\t'
           << fixed_precision(PREC_FIL) << sumEnt/numEnt << '\n';
    }
  }
}