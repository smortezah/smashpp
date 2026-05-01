// SPDX-FileCopyrightText: 2018-2026 Morteza Hosseini
// SPDX-License-Identifier: GPL-3.0-only

#ifndef SMASHPP_NAMING_HPP
#define SMASHPP_NAMING_HPP

#include <format>
#include <string_view>

#include "def.hpp"
#include "file.hpp"

namespace smashpp {
[[nodiscard]] inline static std::string gen_name(uint32_t ID, std::string_view ref,
                                                 std::string_view tar, const Format& frmt) {
  const auto refName{file_name(ref)};
  const auto tarName{file_name(tar)};
  switch (frmt) {
    case Format::profile:
      return std::format("{}.{}.{}.prf", ID, refName, tarName);
    case Format::filter:
      return std::format("{}.{}.{}.fil", ID, refName, tarName);
    case Format::position:
      return std::format("{}.{}.{}.pos", ID, refName, tarName);
    case Format::midposition:
      return std::format("mid.{}.{}.{}.pos", ID, refName, tarName);
    case Format::segment:
      return std::format("{}.{}.{}.s", ID, refName, tarName);
    default:
      return "";
  }
}

[[nodiscard]] inline static std::string gen_name(std::string_view ref, std::string_view tar,
                                                 const Format& frmt) {
  const auto refName{file_name(ref)};
  const auto tarName{file_name(tar)};
  switch (frmt) {
    case Format::profile:
      return std::format("{}.{}.prf", refName, tarName);
    case Format::filter:
      return std::format("{}.{}.fil", refName, tarName);
    case Format::position:
      return std::format("{}.{}.pos", refName, tarName);
    case Format::midposition:
      return std::format("mid.{}.{}.pos", refName, tarName);
    case Format::json:
      return std::format("{}.{}.json", refName, tarName);
    case Format::segment:
      return std::format("{}.{}-s", refName, tarName);
    default:
      return "";
  }
}
}  // namespace smashpp

#endif  // SMASHPP_NAMING_HPP
