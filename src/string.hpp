#ifndef PROJECT_STRING_HPP
#define PROJECT_STRING_HPP

#include "def.hpp"
#include "number.hpp"
#include "exception.hpp"

namespace smashpp {
inline static std::string bold (std::string&& text) {
  return "\033[1m"+text+"\033[0m";
}

inline static std::string bold (const std::string& text) {
  return "\033[1m"+text+"\033[0m";
}

inline static std::string faint (std::string&& text) {
  return "\033[2m"+text+"\033[0m";
}

inline static std::string faint (const std::string& text) {
  return "\033[2m"+text+"\033[0m";
}

inline static std::string italic (std::string&& text) {
  return "\033[3m"+text+"\033[0m";
}

inline static std::string italic (const std::string& text) {
  return "\033[3m"+text+"\033[0m";
}

inline static std::string underline (std::string&& text) {
  return "\033[4m"+text+"\033[0m";
}

inline static std::string underline (const std::string& text) {
  return "\033[4m"+text+"\033[0m";
}

inline static std::string highlight (std::string&& text) {
  return "\033[7m"+text+"\033[0m";
}

inline static std::string highlight (const std::string& text) {
  return "\033[7m"+text+"\033[0m";
}

inline static std::string bold_red (std::string&& text) {
  return "\033[1m\033[38;5;1m"+text+"\033[0m";
}

inline static std::string bold_red (const std::string& text) {
  return "\033[1m\033[38;5;1m"+text+"\033[0m";
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

inline static void wrap_text (string& text) {
  constexpr auto width = TEXTWIDTH;
  string out, word;
  char last = '\0';
  u64 pos = 0;

  for (auto c : text) {
    if (++pos == width) {
      if (word.empty())  
        return;

      auto p = end(word);
      while (p!=begin(word) && *--p!=' ');
      if (*p == ' ') 
        word = string(++p, end(word));

      out += "\n" + word;
      pos  = word.length();
      word.clear();
    }
    else if (c==' ' && last!=' ') {
      out += word;
      word.clear();
    }

    word += c;
    last  = c;
  }
  out += word;
  
  text = move(out);
}

template <typename ValuePos, typename Value>
inline static void show_progress (ValuePos pos, Value total) {
  if (total>100 && pos%(total/100)==0)
    cerr << "Progress: [" << static_cast<int>((pos*100) / total) << "%]\r";
}
template <typename ValuePos, typename Value>
inline static void show_progress (ValuePos pos, Value total, const string& msg){
  if (total>100 && pos%(total/100)==0)
    cerr << msg << "[" << static_cast<int>((pos*100) / total) << "%]\r";
}

template <typename ...Args>
inline static string string_format (const string& format, Args... args) {
  // Extra space for '\0'
  auto size = size_t(snprintf(nullptr, 0, format.c_str(), args...) + 1);
  unique_ptr<char[]> buf(new char[size]);
  snprintf(buf.get(), size, format.c_str(), args...);
  return string(buf.get(), buf.get()+size-1);  // We don't want the '\0' inside
}

inline static string conv_to_string (WType val) {
  switch (val) {
  case WType::RECTANGULAR:  return "0|rectangular";  break;
  case WType::HAMMING:      return "1|hamming";      break;
  case WType::HANN:         return "2|hann";         break;
  case WType::BLACKMAN:     return "3|blackman";     break;
  case WType::TRIANGULAR:   return "4|triangular";   break;
  case WType::WELCH:        return "5|welch";        break;
  case WType::SINE:         return "6|sine";         break;
  case WType::NUTTALL:      return "7|nuttall";      break;
  default:                  return "2|hann";
  }
}

inline static string conv_to_string (FilterScale val) {
  switch (val) {
  case FilterScale::S:      return "S|small";        break;
  case FilterScale::M:      return "M|medium";       break;
  case FilterScale::L:      return "L|large";        break;
  default:                  return "L|large";
  }
}

inline static string human_readable (u64 bytes, u8 precision=0) {
  const u64 KB_div = POW2[10],
            MB_div = POW2[20],
            GB_div = POW2[30],
            TB_div = POW2[40];
  const string precFormat = "%." + to_string(precision) + "f";

  if (bytes >= TB_div)
    return string_format(precFormat+" T", float(bytes) / TB_div);
  else if (bytes >= GB_div && bytes < TB_div)
    return string_format(precFormat+" G", float(bytes) / GB_div);
  else if (bytes >= MB_div && bytes < GB_div)
    return string_format(precFormat+" M", float(bytes) / MB_div);
  else if (bytes >= KB_div && bytes < MB_div)
    return string_format(precFormat+" K", float(bytes) / KB_div);
  else if (bytes < KB_div)
    return string_format(precFormat, float(bytes));

  return "";
}

// Non-computer science
inline static string human_readable_non_cs (u64 bytes, u8 precision=0) {
  const u64 KB_div = POW10[3],
            MB_div = POW10[6],
            GB_div = POW10[9],
            TB_div = POW10[12];

  const auto out = [=](const string& unit, u64 div) {
    if (float(bytes / div) == float(bytes) / div)
      return string_format("%.0f "+unit, float(bytes) / div);
    else
      return 
        string_format("%."+to_string(precision)+"f "+unit, float(bytes) / div);
  };

  if      (bytes >= TB_div)                   { return out("T", TB_div); }
  else if (bytes >= GB_div && bytes < TB_div) { return out("G", GB_div); }
  else if (bytes >= MB_div && bytes < GB_div) { return out("M", MB_div); }
  else if (bytes >= KB_div && bytes < MB_div) { return out("K", KB_div); }
  else if (bytes < KB_div)                    { return out("",  1);      }

  return "";
}
}

#endif //PROJECT_STRING_HPP