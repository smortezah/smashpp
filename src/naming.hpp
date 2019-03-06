// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.

#ifndef PROJECT_NAMING_HPP
#define PROJECT_NAMING_HPP

#include "def.hpp"

namespace smashpp {
inline static std::string gen_name(uint32_t ID, std::string ref,
                                   std::string tar, const Format& frmt) {
  const auto refName{file_name(ref)};
  const auto tarName{file_name(tar)};
  switch (frmt) {
    case Format::PROFILE:
      return std::to_string(ID) + "-" + refName + "_" + tarName + "." + FMT_PRF;
    case Format::FILTER:
      return std::to_string(ID) + "-" + refName + "_" + tarName + "." + FMT_FIL;
    case Format::POSITION:
      return std::to_string(ID) + "-" + refName + "_" + tarName + "." + FMT_POS;
    case Format::SEGMENT:
      return std::to_string(ID) + "-" + refName + "_" + tarName + "-" + LBL_SEG;
    default:
      return "";
  }
}

inline static std::string gen_name(std::string ref, std::string tar,
                                   const Format& frmt) {
  const auto refName{file_name(ref)};
  const auto tarName{file_name(tar)};
  switch (frmt) {
    case Format::PROFILE:
      return refName + "-" + tarName + "." + FMT_PRF;
    case Format::FILTER:
      return refName + "-" + tarName + "." + FMT_FIL;
    case Format::POSITION:
      return refName + "-" + tarName + "." + FMT_POS;
    case Format::SEGMENT:
      return refName + "-" + tarName + "-" + LBL_SEG;
    default:
      return "";
  }
}
}  // namespace smashpp

#endif  // PROJECT_NAMING_HPP