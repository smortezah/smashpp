// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.

#ifndef PROJECT_STRING_HPP
#define PROJECT_STRING_HPP

#include "def.hpp"
#include "number.hpp"
#include "exception.hpp"

namespace smashpp {
inline static std::string bold(std::string&& text) {
  return "\033[1m" + text + "\033[0m";
}

inline static std::string bold(const std::string& text) {
  return "\033[1m" + text + "\033[0m";
}

inline static std::string faint(std::string&& text) {
  return "\033[2m" + text + "\033[0m";
}

inline static std::string faint(const std::string& text) {
  return "\033[2m" + text + "\033[0m";
}

inline static std::string italic(std::string&& text) {
  return "\033[3m" + text + "\033[0m";
}

inline static std::string italic(const std::string& text) {
  return "\033[3m" + text + "\033[0m";
}

inline static std::string underline(std::string&& text) {
  return "\033[4m" + text + "\033[0m";
}

inline static std::string underline(const std::string& text) {
  return "\033[4m" + text + "\033[0m";
}

inline static std::string highlight(std::string&& text) {
  return "\033[7m" + text + "\033[0m";
}

inline static std::string highlight(const std::string& text) {
  return "\033[7m" + text + "\033[0m";
}

inline static std::string bold_red(std::string&& text) {
  return "\033[1m\033[38;5;1m" + text + "\033[0m";
}

inline static std::string bold_red(const std::string& text) {
  return "\033[1m\033[38;5;1m" + text + "\033[0m";
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

inline static void wrap_text(std::string& text) {
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

inline static std::string conv_to_string(WType val) {
  switch (val) {
    case WType::rectangular:
      return "0|rectangular";
      break;
    case WType::hamming:
      return "1|hamming";
      break;
    case WType::hann:
      return "2|hann";
      break;
    case WType::blackman:
      return "3|blackman";
      break;
    case WType::triangular:
      return "4|triangular";
      break;
    case WType::welch:
      return "5|welch";
      break;
    case WType::sine:
      return "6|sine";
      break;
    case WType::nuttall:
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
  const uint64_t KB_div{POW10[3]};
  const uint64_t MB_div{POW10[6]};
  const uint64_t GB_div{POW10[9]};
  const uint64_t TB_div{POW10[12]};

  const auto out = [=](std::string unit, uint64_t div) {
    if (static_cast<float>(bytes / div) == static_cast<float>(bytes) / div)
      return string_format("%.0f " + unit, static_cast<float>(bytes) / div);
    else
      return string_format("%." + std::to_string(precision) + "f " + unit,
                           static_cast<float>(bytes) / div);
  };

  if (bytes >= TB_div) {
    return out("T", TB_div);
  } else if (bytes >= GB_div && bytes < TB_div) {
    return out("G", GB_div);
  } else if (bytes >= MB_div && bytes < GB_div) {
    return out("M", MB_div);
  } else if (bytes >= KB_div && bytes < MB_div) {
    return out("K", KB_div);
  } else if (bytes < KB_div) {
    return out("", 1);
  }

  return "";
}
}  // namespace smashpp

#endif  // PROJECT_STRING_HPP