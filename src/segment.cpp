// SPDX-FileCopyrightText: 2018-2026 Morteza Hosseini
// SPDX-License-Identifier: GPL-3.0-only

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
    const auto beg_pos = begPos * sample_step;
    const auto end_pos = endPos * sample_step;

    if ((round == 1 && end_pos - beg_pos >= minSize) || round != 1) {
      ++nSegs;

      const auto beg = (static_cast<int64_t>(beg_pos - beg_guard) < 0) ? 0 : beg_pos - beg_guard;
      const auto end = (end_pos + end_guard > totalSize) ? totalSize : end_pos + end_guard;
      const auto ent = sumEnt / numEnt;

      pos_out.push_back(PosRow(beg, end, ent));
    }
  }
}

void Segment::set_guards(int16_t beg, int16_t end) {
  beg_guard = beg;  // beg_guard = maxCtx + beg;
  end_guard = end;
}
