// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2020, IEETA, University of Aveiro, Portugal.

#include "output.hpp"
using namespace smashpp;

void PositionFile::write_pos_file_impl(const std::vector<OutRowAux>& out_aux,
                                       bool asym_region) {
  std::ofstream pos_file(name);

  // Head
  pos_file << POS_WATERMARK << '\n';
  pos_file << "##PARAM=<" << param_list << ">\n";
  pos_file << "##INFO=<"
           << "Ref=" << info->ref
           << ",RefSize=" << std::to_string(info->ref_size)
           << ",Tar=" << info->tar
           << ",TarSize=" << std::to_string(info->tar_size) << ">\n";

  // Body
  pos_file << "#RBeg\tREnd\tRRelRdn\tRRdn\tTBeg\tTEnd\tTRelRdn\tTRdn\tInv\n";
  for (auto row : out_aux) {
    uint64_t left_beg = 0, left_end = 0, right_beg = 0, right_end = 0;
    prc_t left_ent = 0.0, left_self_ent = 0.0, right_ent = 0.0,
          right_self_ent = 0.0;

    // Left hand side
    if (row.pos2.beg_pos == 0 && row.pos2.end_pos == 0 && row.pos2.ent == 0 &&
        row.pos2.self_ent == 0) {
      pos_file << DBLANK << '\t' << DBLANK << '\t' << DBLANK << '\t' << DBLANK
               << '\t' << "F" << '\n';
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

    auto publish = [](auto left_beg, auto left_end, auto left_ent,
                      auto left_self_ent, auto right_beg, auto right_end,
                      auto right_ent, auto right_self_ent) -> std::string {
      std::ostringstream oss;
      oss << left_beg << '\t' << left_end << '\t'
          << fixed_precision(PREC_POS, left_ent) << '\t'
          << fixed_precision(PREC_POS, left_self_ent) << '\t' << right_beg
          << '\t' << right_end << '\t' << fixed_precision(PREC_POS, right_ent)
          << '\t' << fixed_precision(PREC_POS, right_self_ent) << '\t'
          << (right_beg < right_end ? "F" : "T") << '\n';
      return oss.str();
    };

    if (!asym_region) {
      if (uint64_t(std::llabs(right_end - right_beg)) <
              (left_end - left_beg) * 1.5 &&
          uint64_t(std::llabs(right_end - right_beg)) >
              (left_end - left_beg) * 0.5)
        pos_file << publish(left_beg, left_end, left_ent, left_self_ent,
                            right_beg, right_end, right_ent, right_self_ent);
    } else {
      if (left_end != 0)
        pos_file << publish(left_beg, left_end, left_ent, left_self_ent,
                            right_beg, right_end, right_ent, right_self_ent);
    }
  }

  pos_file.close();
}

inline void PositionFile::make_write_pos_pair(const std::vector<PosRow>& left,
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

void PositionFile::write_pos_file(const std::vector<PosRow>& pos_out,
                                  bool asym_region) {
  std::vector<PosRow> left, right1, right3;
  for (const auto& row : pos_out) {
    if (row.round == 2) left.push_back(PosRow(row));
    if (row.round == 1) right1.push_back(PosRow(row));
    if (row.round == 3) right3.push_back(PosRow(row));
  }

  make_write_pos_pair(left, right1, right3, asym_region);
}