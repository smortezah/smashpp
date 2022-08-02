// Smash++
// Morteza Hosseini    mhosayny@gmail.com

#include "output.hpp"
#include "file.hpp"
using namespace smashpp;

void PositionFile::dump(const std::vector<PosRow>& pos_out, bool asym_region,
                        Format format) {
  std::ostringstream oss;
  auto pos_content = stream_pos(oss, pos_out, asym_region).str();
  auto file_name = gen_name(info->ref, info->tar, format);
  std::ofstream out_file(file_name);

  if (format == Format::json) {
    out_file << pos2json(pos_content);
  } else {
    out_file << pos_content;
  }

  out_file.close();
}

std::ostringstream& PositionFile::stream_pos(std::ostringstream& out,
                                             const std::vector<PosRow>& pos_out,
                                             bool asym_region) const {
  auto out_aux = pos_pairs(pos_out);
  if (!out_aux.empty()) stream_pos_impl(out, out_aux, asym_region);
  return out;
}

std::vector<OutRowAux> PositionFile::pos_pairs(
    const std::vector<PosRow>& pos_out) const {
  std::vector<PosRow> left, right1, right3;
  for (const auto& row : pos_out) {
    if (row.round == 2) left.push_back(PosRow(row));
    if (row.round == 1) right1.push_back(PosRow(row));
    if (row.round == 3) right3.push_back(PosRow(row));
  }

  // Make position pairs
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

  return out_aux;
}

void PositionFile::stream_pos_impl(std::ostringstream& out,
                                   const std::vector<OutRowAux>& out_aux,
                                   bool asym_region) const {
  // Head
  out << POS_WATERMARK << '\n';
  out << "##PARAM=<" << param_list << ">\n";
  out << "##INFO=<"
      << "Ref=" << info->ref << ",RefSize=" << std::to_string(info->ref_size)
      << ",Tar=" << info->tar << ",TarSize=" << std::to_string(info->tar_size)
      << ">\n";

  // Body
  out << "#RBeg\tREnd\tRRelRdn\tRRdn\tTBeg\tTEnd\tTRelRdn\tTRdn\tInv\n";
  for (auto row : out_aux) {
    uint64_t left_beg = 0, left_end = 0, right_beg = 0, right_end = 0;
    prc_t left_ent = 0.0, left_self_ent = 0.0, right_ent = 0.0,
          right_self_ent = 0.0;

    // Left hand side
    if (row.pos2.beg_pos == 0 && row.pos2.end_pos == 0 && row.pos2.ent == 0 &&
        row.pos2.self_ent == 0) {
      out << DBLANK << '\t' << DBLANK << '\t' << DBLANK << '\t' << DBLANK
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
        out << publish(left_beg, left_end, left_ent, left_self_ent, right_beg,
                       right_end, right_ent, right_self_ent);
    } else {
      if (left_end != 0)
        out << publish(left_beg, left_end, left_ent, left_self_ent, right_beg,
                       right_end, right_ent, right_self_ent);
    }
  }
}