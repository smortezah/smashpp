// SPDX-FileCopyrightText: 2018-2026 Morteza Hosseini
// SPDX-License-Identifier: GPL-3.0-only

#ifndef SMASHPP_STRING_HPP
#define SMASHPP_STRING_HPP

#include <format>

#include "number.hpp"
#include "par.hpp"

namespace smashpp {
static constexpr uint8_t TEXTWIDTH{65};

inline std::string bold(const std::string& text) {
#ifdef _WIN32
  return text;
#else
  return std::format("\033[1m{}\033[0m", text);
#endif
}

inline std::string faint(const std::string& text) {
#ifdef _WIN32
  return text;
#else
  return std::format("\033[2m{}\033[0m", text);
#endif
}

inline std::string italic(const std::string& text) {
#ifdef _WIN32
  return text;
#else
  return std::format("\033[3m{}\033[0m", text);
#endif
}

inline std::string underline(const std::string& text) {
#ifdef _WIN32
  return text;
#else
  return std::format("\033[4m{}\033[0m", text);
#endif
}

inline std::string highlight(const std::string& text) {
#ifdef _WIN32
  return text;
#else
  return std::format("\033[7m{}\033[0m", text);
#endif
}

inline std::string bold_red(const std::string& text) {
#ifdef _WIN32
  return text;
#else
  return std::format("\033[1m\033[38;5;1m{}\033[0m", text);
#endif
}

inline void wrap_text(std::string& text) {
  constexpr auto width{TEXTWIDTH};
  std::string out;
  std::string word;
  char last{'\0'};
  uint64_t pos{0};

  for (auto c : text) {
    if (++pos == width) {
      if (word.empty()) {
        return;
      }

      auto p = std::end(word);
      while (p != std::begin(word) && *--p != ' ');
      if (*p == ' ') {
        word = std::string(++p, std::end(word));
      }

      out += std::format("\n{}", word);
      pos = word.length();
      word.clear();
    } else if (c == ' ' && last != ' ') {
      out += word;
      word.clear();
    }

    word += c;
    last = c;
  }
  out += word;

  text = std::move(out);
}

template <typename ValuePos, typename Value>
inline static void show_progress(ValuePos pos, Value total) {
  if (total > 100 && pos % (total / 100) == 0) {
    std::cerr << std::format("Progress: [{}%]\r", static_cast<int>((pos * 100) / total));
  }
}

template <typename ValuePos, typename Value>
inline static void show_progress(ValuePos pos, Value total, const std::string& msg) {
  if (total > 100 && pos % (total / 100) == 0) {
    std::cerr << std::format("{}[{}%]\r", msg, static_cast<int>((pos * 100) / total));
  }
}

inline static std::string conv_to_string(FilterType val) {
  switch (val) {
    case FilterType::rectangular:
      return "0|rectangular";
    case FilterType::hamming:
      return "1|hamming";
    case FilterType::hann:
      return "2|hann";
    case FilterType::blackman:
      return "3|blackman";
    case FilterType::triangular:
      return "4|triangular";
    case FilterType::welch:
      return "5|welch";
    case FilterType::sine:
      return "6|sine";
    case FilterType::nuttall:
      return "7|nuttall";
    default:
      return "2|hann";
  }
}

inline static std::string conv_to_string(FilterScale val) {
  switch (val) {
    case FilterScale::s:
      return "S|small";
    case FilterScale::m:
      return "M|medium";
    case FilterScale::l:
      return "L|large";
    default:
      return "L|large";
  }
}

inline static std::string human_readable(uint64_t bytes, uint8_t precision = 0) {
  const uint64_t KB_div = pow2(10);
  const uint64_t MB_div = pow2(20);
  const uint64_t GB_div = pow2(30);
  const uint64_t TB_div = pow2(40);
  const auto prec{static_cast<int>(precision)};

  if (bytes >= TB_div) {
    return std::format("{:.{}f} T", static_cast<float>(bytes) / TB_div, prec);
  } else if (bytes >= GB_div && bytes < TB_div) {
    return std::format("{:.{}f} G", static_cast<float>(bytes) / GB_div, prec);
  } else if (bytes >= MB_div && bytes < GB_div) {
    return std::format("{:.{}f} M", static_cast<float>(bytes) / MB_div, prec);
  } else if (bytes >= KB_div && bytes < MB_div) {
    return std::format("{:.{}f} K", static_cast<float>(bytes) / KB_div, prec);
  } else if (bytes < KB_div) {
    return std::format("{:.{}f}", static_cast<float>(bytes), prec);
  }

  return "";
}

// Outside-computer science
inline static std::string human_readable_non_cs(uint64_t bytes, uint8_t precision = 0) {
  const uint64_t Kb_div{POW10[3]};
  const uint64_t Mb_div{POW10[6]};
  const uint64_t Gb_div{POW10[9]};
  const uint64_t Tb_div{POW10[12]};
  const auto prec{static_cast<int>(precision)};

  const auto out = [=](std::string unit, uint64_t div) {
    if (static_cast<float>(bytes / div) == static_cast<float>(bytes) / div) {
      return std::format("{:.0f} {}", static_cast<float>(bytes) / div, unit);
    } else {
      return std::format("{:.{}f} {}", static_cast<float>(bytes) / div, prec, unit);
    }
  };

  if (bytes >= Tb_div) {
    return out("Tb", Tb_div);
  } else if (bytes >= Gb_div && bytes < Tb_div) {
    return out("Gb", Gb_div);
  } else if (bytes >= Mb_div && bytes < Gb_div) {
    return out("Mb", Mb_div);
  } else if (bytes >= Kb_div && bytes < Mb_div) {
    return out("Kb", Kb_div);
  } else if (bytes < Kb_div) {
    return out("b", 1);
  }

  return "";
}
}  // namespace smashpp

#endif  // SMASHPP_STRING_HPP
