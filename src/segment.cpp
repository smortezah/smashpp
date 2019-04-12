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

        if (static_cast<int64_t>(begPos - beg_guard) < 0)
          posF << 0;
        else
          posF << begPos - beg_guard;
        posF << '\t'
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

      if (static_cast<int64_t>(begPos - beg_guard) < 0)
        posF << 0;
      else
        posF << begPos - beg_guard;
      posF << '\t'
           << (endPos + end_guard > totalSize ? totalSize : endPos + end_guard)
           << '\t' << fixed_precision(PREC_FIL) << sumEnt / numEnt << '\n';
    }
  }
}

void Segment::partition(float filtered) {
if (filtered > thresh) {
    begun = false;

    if (begPos != endPos) {
      if (endPos - begPos >= minSize) {
        ++nSegs;

        pos_out->row[pos_out->current_row].beg_ref =
            (static_cast<int64_t>(begPos - beg_guard) < 0) ? 0
                                                           : begPos - beg_guard;
        pos_out->row[pos_out->current_row].end_ref =
            (endPos + end_guard > totalSize) ? totalSize : endPos + end_guard;
        pos_out->row[pos_out->current_row].ent_ref = sumEnt / numEnt;

        ++pos_out->current_row;
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

void Segment::partition_last() {
  if (begPos != endPos) {
    if (endPos - begPos >= minSize) {
      ++nSegs;

      pos_out->row[pos_out->current_row].beg_ref =
          (static_cast<int64_t>(begPos - beg_guard) < 0) ? 0
                                                         : begPos - beg_guard;
      pos_out->row[pos_out->current_row].end_ref =
          (endPos + end_guard > totalSize) ? totalSize : endPos + end_guard;
      pos_out->row[pos_out->current_row].ent_ref = sumEnt / numEnt;

      ++pos_out->current_row;
    }
  }
}

void Segment::set_guards(uint8_t max_ctx, int16_t beg, int16_t end) {
  beg_guard = beg;  // beg_guard = maxCtx + beg;
  end_guard = end;
}