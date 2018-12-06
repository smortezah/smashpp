#ifndef PROJECT_STRING_HPP
#define PROJECT_STRING_HPP

#include "def.hpp"
#include "number.hpp"
#include "exception.hpp"

namespace smashpp {
inline static string bold (string&& text) {
  return "\033[1m"+text+"\033[0m";
}
inline static string bold (const string& text) {
  return "\033[1m"+text+"\033[0m";
}

inline static string faint (string&& text) {
  return "\033[2m"+text+"\033[0m";
}
inline static string faint (const string& text) {
  return "\033[2m"+text+"\033[0m";
}

inline static string italic (string&& text) {
  return "\033[3m"+text+"\033[0m";
}
inline static string italic (const string& text) {
  return "\033[3m"+text+"\033[0m";
}

inline static string underline (string&& text) {
  return "\033[4m"+text+"\033[0m";
}
inline static string underline (const string& text) {
  return "\033[4m"+text+"\033[0m";
}

inline static string highlight (string&& text) {
  return "\033[7m"+text+"\033[0m";
}
inline static string highlight (const string& text) {
  return "\033[7m"+text+"\033[0m";
}

inline static string bold_red (string&& text) {
  return "\033[1m\033[38;5;1m"+text+"\033[0m";
}
inline static string bold_red (const string& text) {
  return "\033[1m\033[38;5;1m"+text+"\033[0m";
}

#ifdef DEBUG
// Print variadic inputs
// template <class Head, class... Tail>
// static void print (Head const& head, Tail const&... tail) {
//   cerr << head;
//   if (std::is_convertible<Head, u8>::value)
//   (void) initializer_list<int>{((cerr<<", "<<tail), 0)...};
// }

template <typename Integral>
static void print (Integral&& in) {
  cerr << (is_u8(in) ? static_cast<u32>(in) : in);
}
template <typename Integral, typename... Args>
static void print (Integral&& in, Args&&... args) {
  cerr << (is_u8(in) ? static_cast<u32>(in) : in) << '\t';
  print(args...);
}
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

      auto p = word.end();
      while (p!=word.begin() && *--p!=' ');
      if (*p == ' ') 
        word = string(++p, word.end());

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
  if (total>100 && pos%(total/100)==0) {
    cerr << "Progress: [" << static_cast<int>((pos*100) / total) << "%]\r";
  }
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
}

#endif //PROJECT_STRING_HPP