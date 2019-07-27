// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.

#ifndef SMASHPP_STRING_HPP
#define SMASHPP_STRING_HPP

#include "par.hpp"
#include "number.hpp"

namespace smashpp {
static constexpr uint8_t TEXTWIDTH{65};

inline std::string bold(const std::string& text) {
#ifdef _WIN32
  return text;
#else
  return "\033[1m" + text + "\033[0m";
#endif
}

inline std::string faint(const std::string& text) {
#ifdef _WIN32
  return text;
#else
  return "\033[2m" + text + "\033[0m";
#endif
}

inline std::string italic(const std::string& text) {
#ifdef _WIN32
  return text;
#else
  return "\033[3m" + text + "\033[0m";
#endif
}

inline std::string underline(const std::string& text) {
#ifdef _WIN32
  return text;
#else
  return "\033[4m" + text + "\033[0m";
#endif
}

inline std::string highlight(const std::string& text) {
#ifdef _WIN32
  return text;
#else
  return "\033[7m" + text + "\033[0m";
#endif
}

inline std::string bold_red(const std::string& text) {
#ifdef _WIN32
  return text;
#else
  return "\033[1m\033[38;5;1m" + text + "\033[0m";
#endif
}

#ifdef DEBUG
// Print variadic inputs
// template <class Head, class... Tail>
// inline static void print (Head const& head, Tail const&... tail) {
//   std::cerr << head;
//   if (std::is_convertible<Head, uint8_t>::value)
//   (void) initializer_list<int>{((std::cerr<<", "<<tail), 0)...};
// }
#endif

inline
//  static 
void wrap_text(std::string& text) {
  constexpr auto width{TEXTWIDTH};
  std::string out;
  std::string word;
  char last{'\0'};
  uint64_t pos{0};

  for (auto c : text) {
    if (++pos == width) {
      if (word.empty()) return;

      auto p = std::end(word);
      while (p != std::begin(word) && *--p != ' ')
        ;
      if (*p == ' ') word = std::string(++p, std::end(word));

      out += "\n" + word;
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
  if (total > 100 && pos % (total / 100) == 0)
    std::cerr << "Progress: [" << static_cast<int>((pos * 100) / total)
              << "%]\r";
}

template <typename ValuePos, typename Value>
inline static void show_progress(ValuePos pos, Value total,
                                 const std::string& msg) {
  if (total > 100 && pos % (total / 100) == 0)
    std::cerr << msg << "[" << static_cast<int>((pos * 100) / total) << "%]\r";
}

template <typename... Args>
inline static std::string string_format(const std::string& format,
                                        Args... args) {
  // Extra space for '\0'
  auto size{size_t(snprintf(nullptr, 0, format.c_str(), args...) + 1)};
  std::unique_ptr<char[]> buf(new char[size]);
  std::snprintf(buf.get(), size, format.c_str(), args...);
  return std::string(buf.get(),
                     buf.get() + size - 1);  // Don't want the '\0' inside
}

inline static std::string conv_to_string(FilterType val) {
  switch (val) {
    case FilterType::rectangular:
      return "0|rectangular";
      break;
    case FilterType::hamming:
      return "1|hamming";
      break;
    case FilterType::hann:
      return "2|hann";
      break;
    case FilterType::blackman:
      return "3|blackman";
      break;
    case FilterType::triangular:
      return "4|triangular";
      break;
    case FilterType::welch:
      return "5|welch";
      break;
    case FilterType::sine:
      return "6|sine";
      break;
    case FilterType::nuttall:
      return "7|nuttall";
      break;
    default:
      return "2|hann";
  }
}

inline static std::string conv_to_string(FilterScale val) {
  switch (val) {
    case FilterScale::s:
      return "S|small";
      break;
    case FilterScale::m:
      return "M|medium";
      break;
    case FilterScale::l:
      return "L|large";
      break;
    default:
      return "L|large";
  }
}

inline static std::string human_readable(uint64_t bytes,
                                         uint8_t precision = 0) {
  const uint64_t KB_div{POW2[10]};
  const uint64_t MB_div{POW2[20]};
  const uint64_t GB_div{POW2[30]};
  const uint64_t TB_div{POW2[40]};
  const std::string precFormat{"%." + std::to_string(precision) + "f"};

  if (bytes >= TB_div)
    return string_format(precFormat + " T", static_cast<float>(bytes) / TB_div);
  else if (bytes >= GB_div && bytes < TB_div)
    return string_format(precFormat + " G", static_cast<float>(bytes) / GB_div);
  else if (bytes >= MB_div && bytes < GB_div)
    return string_format(precFormat + " M", static_cast<float>(bytes) / MB_div);
  else if (bytes >= KB_div && bytes < MB_div)
    return string_format(precFormat + " K", static_cast<float>(bytes) / KB_div);
  else if (bytes < KB_div)
    return string_format(precFormat, static_cast<float>(bytes));

  return "";
}

// Non-computer science
inline static std::string human_readable_non_cs(uint64_t bytes,
                                                uint8_t precision = 0) {
  const uint64_t Kb_div{POW10[3]};
  const uint64_t Mb_div{POW10[6]};
  const uint64_t Gb_div{POW10[9]};
  const uint64_t Tb_div{POW10[12]};

  const auto out = [=](std::string unit, uint64_t div) {
    if (static_cast<float>(bytes / div) == static_cast<float>(bytes) / div)
      return string_format("%.0f " + unit, static_cast<float>(bytes) / div);
    else
      return string_format("%." + std::to_string(precision) + "f " + unit,
                           static_cast<float>(bytes) / div);
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