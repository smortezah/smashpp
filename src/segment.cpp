// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.

#include "segment.hpp"
using namespace smashpp;

void Segment::partition(std::ofstream& posF, float filtered) {
  if (filtered > thresh) {
    begun = false;
    if (begPos != endPos) {
      if (endPos - begPos >= minSize) {
        ++nSegs;
        posF << (begPos < begGuard ? 0 : begPos - begGuard) << '\t'
             << (endPos + endGuard > totalSize ? totalSize : endPos + endGuard)
             << '\t' << fixed_precision(PREC_FIL) << sumEnt / numEnt << '\n';
      }
    }
    begPos = 0, endPos = 0, sumEnt = 0, numEnt = 0;
  } else {
    if (!begun) {
      begun = true;
      begPos = pos;
    }
    endPos = pos;
    sumEnt += filtered;
    ++numEnt;
  }
}

void Segment::partition_last(std::ofstream& posF) {
  if (begPos != endPos) {
    if (endPos - begPos >= minSize) {
      ++nSegs;
      posF << (begPos < begGuard ? 0 : begPos - begGuard) << '\t'
           << (endPos + endGuard > totalSize ? totalSize : endPos + endGuard)
           << '\t' << fixed_precision(PREC_FIL) << sumEnt / numEnt << '\n';
    }
  }
}

void Segment::set_guards(uint8_t maxCtx) {
  begGuard = static_cast<uint64_t>(maxCtx + PART_GUARD);
  endGuard = static_cast<uint64_t>(PART_GUARD);
}