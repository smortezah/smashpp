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
    case Format::profile:
      return std::to_string(ID) + "." + refName + "." + tarName + ".prf";
      // return std::to_string(ID) + "-" + refName + "_" + tarName + ".prf";
    case Format::filter:
      return std::to_string(ID) + "." + refName + "." + tarName + ".fil";
      // return std::to_string(ID) + "-" + refName + "_" + tarName + ".fil";
    case Format::position:
      return std::to_string(ID) + "." + refName + "." + tarName + ".pos";
      // return std::to_string(ID) + "-" + refName + "_" + tarName + ".pos";
    case Format::segment:
      return std::to_string(ID) + "." + refName + "." + tarName + ".s";
      // return std::to_string(ID) + "-" + refName + "_" + tarName + "-s";
    default:
      return "";
  }
}

inline static std::string gen_name(std::string ref, std::string tar,
                                   const Format& frmt) {
  const auto refName{file_name(ref)};
  const auto tarName{file_name(tar)};
  switch (frmt) {
    case Format::profile:
      return refName + "." + tarName + ".prf";
      // return refName + "-" + tarName + ".prf";
    case Format::filter:
      return refName + "." + tarName + ".fil";
      // return refName + "-" + tarName + ".fil";
    case Format::position:
      return refName + "." + tarName + ".pos";
      // return refName + "-" + tarName + ".pos";
    case Format::segment:
      return refName + "." + tarName + "-s";
      // return refName + "-" + tarName + "-s";
    default:
      return "";
  }
}
}  // namespace smashpp

#endif  // PROJECT_NAMING_HPP