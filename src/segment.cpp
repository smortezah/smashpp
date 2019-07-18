// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.

#include "segment.hpp"
using namespace smashpp;

void Segment::partition(std::vector<PosRow>& pos_out, float filtered) {
  if (filtered > thresh) {
    begun = false;

    finalize_partition(pos_out);

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

void Segment::finalize_partition(std::vector<PosRow>& pos_out) {
  if (endPos != begPos) {
    // todo
    // std::cerr << begPos << ' ' << endPos << '\n';
    begPos *= sample_step;
    endPos *= sample_step;
    totalSize *= sample_step;
    // todo
    // std::cerr << begPos << ' ' << endPos << "\n\n";

    // if (endPos != begPos && endPos - begPos >= minSize) {
    if ((round == 1 && endPos - begPos >= minSize) || round != 1) {
      ++nSegs;

      const auto beg = (static_cast<int64_t>(begPos - beg_guard) < 0)
                           ? 0
                           : begPos - beg_guard;
      const auto end =
          (endPos + end_guard > totalSize) ? totalSize : endPos + end_guard;
      const auto ent = sumEnt / numEnt;

      // todo
      std::cerr << beg << ' ' << end << '\n';

      pos_out.push_back(PosRow(beg, end, ent));
    }
  }
}

void Segment::set_guards(uint8_t max_ctx, int16_t beg, int16_t end) {
  beg_guard = beg;  // beg_guard = maxCtx + beg;
  end_guard = end;
}