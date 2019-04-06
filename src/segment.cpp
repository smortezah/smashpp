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
        posF << (begPos < beg_guard ? 0 : begPos - beg_guard) << '\t'
             << (endPos + end_guard > totalSize ? totalSize
                                                : endPos + end_guard)
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
      posF << (begPos < beg_guard ? 0 : begPos - beg_guard) << '\t'
           << (endPos + end_guard > totalSize ? totalSize : endPos + end_guard)
           << '\t' << fixed_precision(PREC_FIL) << sumEnt / numEnt << '\n';
    }
  }
}

void Segment::set_guards(uint8_t maxCtx, int16_t ref_beg_guard,
                         int16_t ref_end_guard, int16_t tar_beg_guard,
                         int16_t tar_end_guard) {
  // beg_guard = static_cast<uint64_t>(maxCtx + PART_GUARD);
  // end_guard = static_cast<uint64_t>(PART_GUARD);
  // todo
  beg_guard = static_cast<uint64_t>(maxCtx + ref_beg_guard);
  end_guard = static_cast<uint64_t>(ref_end_guard);
}