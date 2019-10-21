// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.

#ifndef SMASHPP_OUTPUT_HPP
#define SMASHPP_OUTPUT_HPP

#include "naming.hpp"

namespace smashpp {
struct OutRowAux {
  PosRow pos2;
  PosRow pos1;
  std::vector<PosRow> pos3;
  OutRowAux() = default;
  OutRowAux(PosRow pos2_, PosRow pos1_) : pos2(pos2_), pos1(pos1_) {}
};

inline static void write_pos_file_impl(const std::vector<OutRowAux>& out_aux,
                                       bool asym_region) {
  const auto ref{out_aux.front().pos1.ref};
  const auto tar{out_aux.front().pos1.tar};
  const auto pos_file_name{gen_name(ref, tar, Format::position)};
  std::ofstream pos_file(pos_file_name);

  // Head
  pos_file << POS_HDR << '\t' << file_name(ref) << '\t'
           << std::to_string(file_size(ref)) << '\t' << file_name(tar) << '\t'
           << std::to_string(file_size(tar)) << '\n';

  // Body
  uint64_t left_beg = 0, left_end = 0, right_beg = 0, right_end = 0;
  prc_t left_ent = 0.0, left_self_ent = 0.0, right_ent = 0.0,
        right_self_ent = 0.0;

  for (auto row : out_aux) {
    // Left hand side
    if (row.pos2.beg_pos == 0 && row.pos2.end_pos == 0 && row.pos2.ent == 0 &&
        row.pos2.self_ent == 0) {
      pos_file << DBLANK << '\t' << DBLANK << '\t' << DBLANK << '\t' << DBLANK
               << '\n';
    } else {
      left_beg = row.pos2.beg_pos;
      left_end = row.pos2.end_pos;
      left_ent = row.pos2.ent;
      left_self_ent = row.pos2.self_ent;
    }

    // Right hand side
    if (row.pos3.size() == 1) {
      if (row.pos3.front().run_num == 0) {
        right_beg = row.pos1.beg_pos + row.pos3.front().beg_pos;
        right_end = row.pos1.beg_pos + row.pos3.front().end_pos;
      } else if (row.pos3.front().run_num == 1) {
        right_beg = row.pos1.beg_pos + row.pos3.front().end_pos;
        right_end = row.pos1.beg_pos + row.pos3.front().beg_pos;
      }
      right_ent = row.pos3.front().ent;
      right_self_ent = row.pos3.front().self_ent;
    } else {
      if (row.pos1.run_num == 0) {
        right_beg = row.pos1.beg_pos;
        right_end = row.pos1.end_pos;
      } else if (row.pos1.run_num == 1) {
        right_beg = row.pos1.end_pos;
        right_end = row.pos1.beg_pos;
      }
      right_ent = row.pos1.ent;
      right_self_ent = row.pos1.self_ent;
    }

    if (!asym_region) {
      if (uint64_t(std::llabs(right_end - right_beg)) <
              (left_end - left_beg) * 1.5 &&
          uint64_t(std::llabs(right_end - right_beg)) >
              (left_end - left_beg) * 0.5)
        pos_file << left_beg << '\t' << left_end << '\t' << left_ent << '\t'
                 << left_self_ent << '\t' << right_beg << '\t' << right_end
                 << '\t' << right_ent << '\t' << right_self_ent << '\n';
    } else {
      if (left_end != 0)
        pos_file << left_beg << '\t' << left_end << '\t' << left_ent << '\t'
                 << left_self_ent << '\t' << right_beg << '\t' << right_end
                 << '\t' << right_ent << '\t' << right_self_ent << '\n';
    }
  }

  pos_file.close();
}

inline static void make_write_pos_pair(const std::vector<PosRow>& left,
                                       const std::vector<PosRow>& right1,
                                       const std::vector<PosRow>& right3,
                                       bool asym_region) {
  std::vector<OutRowAux> out_aux{std::vector<OutRowAux>()};

  if (left.empty()) {
    for (const auto& row : right1) out_aux.push_back(OutRowAux(PosRow(), row));
  } else {
    for (const auto& row_left : left) {
      for (const auto& row_right1 : right1) {
        const auto seg_name{gen_name(row_right1.run_num, row_right1.ref,
                                     row_right1.tar, Format::segment) +
                            std::to_string(row_right1.seg_num)};
        if (row_left.ref == seg_name) {
          out_aux.push_back(OutRowAux(row_left, row_right1));
          break;
        }
      }
    }

    for (const auto& row_right3 : right3) {
      for (auto row_out_aux : out_aux) {
        const auto seg_name{gen_name(row_out_aux.pos2.run_num,
                                     row_out_aux.pos2.ref, row_out_aux.pos2.tar,
                                     Format::segment) +
                            std::to_string(row_out_aux.pos2.seg_num)};
        if (row_right3.ref == seg_name) {
          row_out_aux.pos3.push_back(row_right3);
          break;
        }
      }
    }
  }

  if (!out_aux.empty()) write_pos_file_impl(out_aux, asym_region);
}

inline static void write_pos_file(const std::vector<PosRow>& pos_out,
                                  bool asym_region) {
  std::vector<PosRow> left, right1, right3;
  for (const auto& row : pos_out) {
    if (row.round == 2) left.push_back(PosRow(row));
    if (row.round == 1) right1.push_back(PosRow(row));
    if (row.round == 3) right3.push_back(PosRow(row));
  }

  make_write_pos_pair(left, right1, right3, asym_region);
}
}  // namespace smashpp

#endif  // SMASHPP_OUTPUT_HPP