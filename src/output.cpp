// SPDX-FileCopyrightText: 2018-2026 Morteza Hosseini
// SPDX-License-Identifier: GPL-3.0-only

#include "output.hpp"

#include <format>

#include "file.hpp"
using namespace smashpp;

void PositionFile::dump(const std::vector<PosRow>& pos_out, bool asym_region, Format format) {
  std::string pos_content;
  stream_pos(pos_content, pos_out, asym_region);
  auto file_name = gen_name(info->ref, info->tar, format);
  std::ofstream out_file(file_name);

  if (format == Format::json) {
    out_file << pos2json(pos_content);
  } else {
    out_file << pos_content;
  }

  out_file.close();
}

std::string& PositionFile::stream_pos(std::string& out, const std::vector<PosRow>& pos_out,
                                      bool asym_region) const {
  auto out_aux = pos_pairs(pos_out);
  stream_pos_impl(out, out_aux, asym_region);
  return out;
}

std::vector<OutRowAux> PositionFile::pos_pairs(const std::vector<PosRow>& pos_out) const {
  std::vector<PosRow> left, right1, right3;
  for (const auto& row : pos_out) {
    if (row.round == 2) {
      left.push_back(PosRow(row));
    }
    if (row.round == 1) {
      right1.push_back(PosRow(row));
    }
    if (row.round == 3) {
      right3.push_back(PosRow(row));
    }
  }

  // Make position pairs
  std::vector<OutRowAux> out_aux;

  if (left.empty()) {
    for (const auto& row : right1) {
      out_aux.push_back(OutRowAux(PosRow(), row));
    }
  } else {
    for (const auto& row_left : left) {
      for (const auto& row_right1 : right1) {
        const auto seg_name{std::format(
            "{}{}", gen_name(row_right1.run_num, row_right1.ref, row_right1.tar, Format::segment),
            row_right1.seg_num)};
        if (row_left.ref == seg_name) {
          out_aux.push_back(OutRowAux(row_left, row_right1));
          break;
        }
      }
    }
    for (const auto& row_right3 : right3) {
      for (auto& row_out_aux : out_aux) {
        const auto seg_name{std::format("{}{}",
                                        gen_name(row_out_aux.pos2.run_num, row_out_aux.pos2.ref,
                                                 row_out_aux.pos2.tar, Format::segment),
                                        row_out_aux.pos2.seg_num)};
        if (row_right3.ref == seg_name) {
          row_out_aux.pos3.push_back(row_right3);
          break;
        }
      }
    }
  }

  return out_aux;
}

void PositionFile::stream_pos_impl(std::string& out, const std::vector<OutRowAux>& out_aux,
                                   bool asym_region) const {
  // Head
  out += std::format("{}\n", POS_WATERMARK);
  out += std::format("##PARAM=<{}>\n", param_list);
  out += std::format("##INFO=<Ref={},RefSize={},Tar={},TarSize={}>\n", info->ref, info->ref_size,
                     info->tar, info->tar_size);

  // Body
  out += "#RBeg\tREnd\tRRelRdn\tRRdn\tTBeg\tTEnd\tTRelRdn\tTRdn\tInv\n";
  for (auto row : out_aux) {
    uint64_t left_beg = 0, left_end = 0, right_beg = 0, right_end = 0;
    prc_t left_ent = 0.0, left_self_ent = 0.0, right_ent = 0.0, right_self_ent = 0.0;

    // Left hand side
    const bool has_left = !(row.pos2.beg_pos == 0 && row.pos2.end_pos == 0 && row.pos2.ent == 0 &&
                            row.pos2.self_ent == 0);
    if (has_left) {
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

    const auto left_beg_out = has_left ? std::format("{}", left_beg) : std::format("{}", DBLANK);
    const auto left_end_out = has_left ? std::format("{}", left_end) : std::format("{}", DBLANK);
    const auto left_ent_out =
        has_left ? fixed_precision(PREC_POS, left_ent) : std::format("{}", DBLANK);
    const auto left_self_ent_out =
        has_left ? fixed_precision(PREC_POS, left_self_ent) : std::format("{}", DBLANK);
    auto publish = [&]() -> std::string {
      return std::format(
          "{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\n", left_beg_out, left_end_out, left_ent_out,
          left_self_ent_out, right_beg, right_end, fixed_precision(PREC_POS, right_ent),
          fixed_precision(PREC_POS, right_self_ent), (right_beg < right_end ? "F" : "T"));
    };
    const auto abs_diff = [](uint64_t lhs, uint64_t rhs) {
      return (lhs > rhs) ? lhs - rhs : rhs - lhs;
    };
    const auto right_span = abs_diff(right_end, right_beg);
    const auto left_span = abs_diff(left_end, left_beg);

    if (!has_left) {
      out += publish();
    } else if (!asym_region) {
      if (right_span < left_span * 1.5 && right_span > left_span * 0.5) {
        out += publish();
      }
    } else {
      if (left_end != 0) {
        out += publish();
      }
    }
  }
}
