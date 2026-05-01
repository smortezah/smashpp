// SPDX-FileCopyrightText: 2018-2026 Morteza Hosseini
// SPDX-License-Identifier: GPL-3.0-only

#include "segment.hpp"

#include <algorithm>

using namespace smashpp;

void Segment::partition(std::vector<PosRow>& pos_out, float filtered) {
  if (filtered > thresh) {
    finalize_partition(pos_out);

    begun = false;
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
  if (begun) {
    const auto max_pos = totalSize == 0 ? 0 : totalSize - 1;
    const auto beg_pos = std::min(begPos * sample_step, max_pos);
    const auto sampled_end = (endPos + 1) * sample_step - 1;
    const auto end_pos = std::min(sampled_end, max_pos);

    if ((round == 1 && end_pos - beg_pos + 1 >= minSize) || round != 1) {
      ++nSegs;

      const auto clamp_pos = [max_pos](int64_t pos) {
        if (pos < 0) {
          return 0ull;
        }
        return std::min(static_cast<uint64_t>(pos), max_pos);
      };
      const auto beg = clamp_pos(static_cast<int64_t>(beg_pos) - beg_guard);
      const auto end = clamp_pos(static_cast<int64_t>(end_pos) + end_guard);
      const auto ent = sumEnt / numEnt;

      pos_out.push_back(PosRow(beg, end, ent));
    }
  }
}

void Segment::set_guards(int16_t beg, int16_t end) {
  beg_guard = beg;  // beg_guard = maxCtx + beg;
  end_guard = end;
}
