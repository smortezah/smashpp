// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.

#include "segment.hpp"
using namespace smashpp;

void Segment::partition(std::vector<PosRow>& pos_row, float filtered) {
  if (filtered > thresh) {
    begun = false;

    if (begPos != endPos) {
      if (endPos - begPos >= minSize) {
        ++nSegs;

        int64_t beg = (static_cast<int64_t>(begPos - beg_guard) < 0)
                          ? 0
                          : begPos - beg_guard;
        int64_t end =
            (endPos + end_guard > totalSize) ? totalSize : endPos + end_guard;
        prc_t ent = sumEnt / numEnt;

        pos_row.push_back(PosRow(beg,end,ent));
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

void Segment::partition_last(std::vector<PosRow>& pos_row) {
  if (begPos != endPos) {
    if (endPos - begPos >= minSize) {
      ++nSegs;

      int64_t beg = (static_cast<int64_t>(begPos - beg_guard) < 0)
                        ? 0
                        : begPos - beg_guard;
      int64_t end =
          (endPos + end_guard > totalSize) ? totalSize : endPos + end_guard;
      prc_t ent = sumEnt / numEnt;

      pos_row.push_back(PosRow(beg, end, ent));
    }
  }
}

void Segment::set_guards(uint8_t max_ctx, int16_t beg, int16_t end) {
  beg_guard = beg;  // beg_guard = maxCtx + beg;
  end_guard = end;
}