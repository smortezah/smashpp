// SPDX-FileCopyrightText: 2018-2026 Morteza Hosseini
// SPDX-License-Identifier: GPL-3.0-only

#ifndef SMASHPP_MEMORY_HPP
#define SMASHPP_MEMORY_HPP

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <format>
#include <limits>
#include <string>

#if defined(__APPLE__)
#include <sys/sysctl.h>
#elif defined(__linux__)
#include <sys/sysinfo.h>
#endif

#include "exception.hpp"
#include "file.hpp"
#include "par.hpp"

namespace smashpp {
static constexpr uint8_t DEFAULT_MEMORY_LIMIT_PERCENT{80};

struct MemoryBudget {
  uint64_t ref_models;
  uint64_t tar_models;
  uint64_t profile;
  uint64_t filtered;
  uint64_t per_mode_peak;
  uint64_t concurrent_modes;
  uint64_t peak;
  uint64_t limit;
};

inline auto saturating_add(uint64_t lhs, uint64_t rhs) -> uint64_t {
  const auto max = std::numeric_limits<uint64_t>::max();
  if (max - lhs < rhs) {
    return max;
  }
  return lhs + rhs;
}

inline auto saturating_mul(uint64_t lhs, uint64_t rhs) -> uint64_t {
  const auto max = std::numeric_limits<uint64_t>::max();
  if (lhs != 0 && rhs > max / lhs) {
    return max;
  }
  return lhs * rhs;
}

inline auto saturating_shift_left(uint64_t value, uint32_t shift) -> uint64_t {
  if (shift >= 64) {
    return std::numeric_limits<uint64_t>::max();
  }
  return value << shift;
}

inline auto bytes_to_readable(uint64_t bytes) -> std::string {
  constexpr std::array units{"B", "KiB", "MiB", "GiB", "TiB"};
  auto value = static_cast<double>(bytes);
  size_t unit_idx = 0;

  while (value >= 1024.0 && unit_idx + 1 < units.size()) {
    value /= 1024.0;
    ++unit_idx;
  }

  if (unit_idx == 0) {
    return std::format("{} {}", bytes, units[unit_idx]);
  }
  return std::format("{:.1f} {}", value, units[unit_idx]);
}

inline auto parse_memory_size(std::string value) -> uint64_t {
  value.erase(std::remove_if(std::begin(value), std::end(value),
                             [](unsigned char c) { return std::isspace(c); }),
              std::end(value));
  if (value.empty()) {
    error("memory budget not specified. Use \"--max-memory <SIZE>\".");
  }

  size_t unit_pos = 0;
  bool seen_decimal = false;
  while (unit_pos < value.size()) {
    const auto c = static_cast<unsigned char>(value[unit_pos]);
    if (std::isdigit(c)) {
      ++unit_pos;
      continue;
    }
    if (value[unit_pos] == '.' && !seen_decimal) {
      seen_decimal = true;
      ++unit_pos;
      continue;
    }
    break;
  }

  if (unit_pos == 0 || (unit_pos < value.size() && value[unit_pos] == '.')) {
    error(std::format("invalid memory budget \"{}\".", value));
  }

  const auto number = value.substr(0, unit_pos);
  size_t parsed = 0;
  long double amount = 0;
  try {
    amount = std::stold(number, &parsed);
  } catch (const std::exception&) {
    error(std::format("invalid memory budget \"{}\".", value));
  }

  if (parsed != number.size()) {
    error(std::format("invalid memory budget \"{}\".", value));
  }

  if (amount < 0) {
    error(std::format("invalid memory budget \"{}\".", value));
  }
  if (amount == 0) {
    return 0;
  }

  auto unit = value.substr(unit_pos);
  std::transform(std::begin(unit), std::end(unit), std::begin(unit),
                 [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

  uint64_t multiplier = 1;
  if (unit.empty() || unit == "b") {
    multiplier = 1;
  } else if (unit == "k" || unit == "kb" || unit == "kib") {
    multiplier = 1024ull;
  } else if (unit == "m" || unit == "mb" || unit == "mib") {
    multiplier = 1024ull * 1024ull;
  } else if (unit == "g" || unit == "gb" || unit == "gib") {
    multiplier = 1024ull * 1024ull * 1024ull;
  } else if (unit == "t" || unit == "tb" || unit == "tib") {
    multiplier = 1024ull * 1024ull * 1024ull * 1024ull;
  } else {
    error(std::format("invalid memory budget unit \"{}\".", unit));
  }

  const auto bytes = amount * static_cast<long double>(multiplier);
  if (bytes > static_cast<long double>(std::numeric_limits<uint64_t>::max())) {
    error(std::format("memory budget \"{}\" is too large.", value));
  }

  return static_cast<uint64_t>(std::ceil(bytes));
}

inline auto physical_memory_bytes() -> uint64_t {
#if defined(__APPLE__)
  uint64_t memory = 0;
  size_t size = sizeof(memory);
  if (sysctlbyname("hw.memsize", &memory, &size, nullptr, 0) == 0) {
    return memory;
  }
#elif defined(__linux__)
  struct sysinfo info{};
  if (sysinfo(&info) == 0) {
    return saturating_mul(info.totalram, info.mem_unit);
  }
#endif

  return 0;
}

inline auto default_memory_limit() -> uint64_t {
  const auto physical = physical_memory_bytes();
  if (physical == 0) {
    return 0;
  }
  return saturating_mul(physical / 100, DEFAULT_MEMORY_LIMIT_PERCENT);
}

inline auto configured_memory_limit(const Param& par) -> uint64_t {
  if (par.manMaxMemory) {
    return par.maxMemory;
  }
  return default_memory_limit();
}

inline auto table_entries(uint8_t k) -> uint64_t {
  return saturating_shift_left(1ull, static_cast<uint32_t>(k) * 2u + 2u);
}

inline auto model_memory_bytes(const MMPar& model) -> uint64_t {
  switch (model_container(model)) {
    case Container::table_64:
      return saturating_mul(table_entries(model.k), sizeof(uint64_t));
    case Container::table_32:
      return saturating_mul(table_entries(model.k), sizeof(uint32_t));
    case Container::log_table_8:
      return table_entries(model.k);
    case Container::sketch_8: {
      const auto cells = saturating_mul(model.w, model.d);
      const auto sketch_bytes =
          cells == std::numeric_limits<uint64_t>::max() ? cells : (cells + 1) / 2;
      const auto hash_bytes = saturating_mul(saturating_mul(model.d, 2), sizeof(uint64_t));
      return saturating_add(sketch_bytes, hash_bytes);
    }
  }

  return 0;
}

inline auto model_set_memory_bytes(const std::vector<MMPar>& models) -> uint64_t {
  uint64_t total = 0;
  for (const auto& model : models) {
    total = saturating_add(total, model_memory_bytes(model));
  }
  return total;
}

inline auto profile_memory_bytes(const Param& par) -> uint64_t {
  const auto samples = file_size(par.tar) / par.sampleStep + 1;
  return saturating_mul(samples, sizeof(prc_t));
}

inline auto filtered_memory_bytes(const Param& par) -> uint64_t {
  if (!par.saveAll && !par.saveFilter) {
    return 0;
  }
  if (par.filt_type == FilterType::rectangular) {
    return 0;
  }

  const auto samples = file_size(par.tar) / par.sampleStep + 1;
  return saturating_mul(samples, sizeof(float));
}

inline auto estimate_memory_budget(const Param& par, uint64_t concurrent_modes) -> MemoryBudget {
  const auto ref_models = model_set_memory_bytes(par.refMs);
  const auto tar_models = model_set_memory_bytes(par.tarMs);
  const auto profile = profile_memory_bytes(par);
  const auto filtered = filtered_memory_bytes(par);
  const auto per_mode_peak =
      saturating_add(std::max(ref_models, tar_models), saturating_add(profile, filtered));
  const auto peak = saturating_mul(per_mode_peak, concurrent_modes);

  return {ref_models,    tar_models,       profile, filtered,
          per_mode_peak, concurrent_modes, peak,    configured_memory_limit(par)};
}

inline void enforce_memory_budget(const Param& par, uint64_t concurrent_modes) {
  const auto budget = estimate_memory_budget(par, concurrent_modes);
  if (budget.limit == 0 || budget.peak <= budget.limit) {
    return;
  }

  error(std::format(
      "estimated peak memory {} exceeds the configured limit {}. Estimate: {} concurrent mode{}, "
      "{} per mode (reference models {}, target models {}, profile {}, filtered {}). Lower "
      "--num-threads, use smaller model contexts/sketches, or raise --max-memory.",
      bytes_to_readable(budget.peak), bytes_to_readable(budget.limit), budget.concurrent_modes,
      budget.concurrent_modes == 1 ? "" : "s", bytes_to_readable(budget.per_mode_peak),
      bytes_to_readable(budget.ref_models), bytes_to_readable(budget.tar_models),
      bytes_to_readable(budget.profile), bytes_to_readable(budget.filtered)));
}
}  // namespace smashpp

#endif  // SMASHPP_MEMORY_HPP
