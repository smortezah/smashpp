// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.

#ifndef SMASHPP_CLEAN_HPP
#define SMASHPP_CLEAN_HPP

#include "naming.hpp"

namespace smashpp {
inline static void remove_temp_seg(std::unique_ptr<Param>& par,
                                   uint64_t seg_num) {
  const auto seg{gen_name(par->ID, par->ref, par->tar, Format::segment)};

  for (uint64_t i = 0; i != seg_num; ++i)
    if (!par->saveAll && !par->saveSegment)
      remove((seg + std::to_string(i)).c_str());
}

inline static void remove_temp_seq(std::unique_ptr<Param>& par) {
  if (par->refType == FileType::fasta || par->refType == FileType::fastq) {
    if (!par->saveSeq) {
      remove(par->ref.c_str());
    } else {
      const std::string seq_name = file_name_no_ext(par->ref) + ".seq";
      rename(par->ref.c_str(), seq_name.c_str());
    }
    rename((par->ref + LBL_BAK).c_str(), par->ref.c_str());
  }

  if (par->tarType == FileType::fasta || par->tarType == FileType::fastq) {
    if (!par->saveSeq) {
      remove(par->tar.c_str());
    } else {
      const std::string seq_name = file_name_no_ext(par->tar) + ".seq";
      rename(par->tar.c_str(), seq_name.c_str());
    }
    rename((par->tar + LBL_BAK).c_str(), par->tar.c_str());
  }
}
}  // namespace smashpp

#endif  // SMASHPP_CLEAN_HPP