#ifndef PROJECT_FN_HPP
#define PROJECT_FN_HPP

#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <memory>
#include <iterator>
#include "def.hpp"

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

template <typename Input>
inline static bool is_u8 (Input&& in) {
  return typeid(in)==typeid(u8);
}

#ifdef DEBUG
// Print variadic inputs
template <typename Integral>
static void print (Integral&& in) {
  cerr << (is_u8(in) ? static_cast<u32>(in) : in) << '\n';
}
template <typename Integral, typename... Args>
static void print (Integral&& in, Args&&... args) {
  cerr << (is_u8(in) ? static_cast<u32>(in) : in) << '\t';
  print(args...);
}
#endif

// "inline" is a MUST -- not to get "multiple definition of `now()'" error
inline static chrono::time_point<chrono::high_resolution_clock> now () noexcept{
  return chrono::high_resolution_clock::now();
}

template <typename Time>
inline static string hms (Time elapsed) {
//  std::chrono::duration<double, std::milli> ms = elapsed;
  const auto durSec = chrono::duration_cast<chrono::seconds>(elapsed).count();
  const auto h = durSec / 3600;
  const auto m = (durSec % 3600) / 60;
  const auto s = durSec % 60;

  if (m < 1)
    return to_string(s)+" sec.\n";
  else if (h < 1)
    return to_string(m)+":"+to_string(s)+" min:sec.\n";
  else
    return to_string(h)+":"+to_string(m)+":"+to_string(s)+" hour:min:sec.\n";
}

inline static void ignore_this_line (ifstream& fs) {
  fs.ignore(numeric_limits<std::streamsize>::max(), '\n');
}

// Split a range by delim and insert the result into an std::vector
template <typename InIter, typename Vec>
inline static void split (InIter first, InIter last, char delim, Vec& vOut) {
  while (true) {
    InIter found = find(first, last, delim);
    vOut.emplace_back(string(first,found));
    if (found == last)
      break;
    first = ++found;
  }
}

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

// "inline" is a MUST -- not to get "multiple definition of `now()'" error
inline static void error (string&& msg) {
  string message = "Error: " + std::move(msg);
  wrap_text(message);
  throw runtime_error (bold_red(message.substr(0,6)) + message.substr(6) +"\n");
}

inline static void err (string&& msg) {
  string message = "Error: " + std::move(msg);
  wrap_text(message);
  cerr << bold_red(message.substr(0,6)) << message.substr(6) << '\n';
}

inline static void warning (string&& msg) {
  string message = "Warning: " + std::move(msg);
  wrap_text(message);
  cerr << bold(message.substr(0,8)) << message.substr(8) << '\n';
}

template <typename Iter, typename Element>
inline static bool has (Iter first, Iter last, Element elem) {
  return find(first, last, elem) != last;
}

#ifdef ARRAY_HISTORY
template <typename VecIter>
static u8 pop_count (VecIter first, u8 len) {  // Number of ones in a bool array
  u8 n = 0;
  for (auto last=first+len; last--!=first;) {
    if (*last == true)  // 1u instead of "true"
      ++n;
  }
  return n;
}
#else
template <typename Digit>
inline static u8 pop_count (Digit d) {  // Number of ones in a digit
  u8 n = 0;
  for (; d; ++n)  d &= d-1;  // First impl. Kernighan. Clear the LSB set
//  do { if (d & 1) ++n; } while (d >>= 1); // Second impl. Negative nums?
  return n;
}
#endif

template <typename Iter, typename Value>
inline static bool are_all (Iter first, Value val) {
  return all_of(first, first+CARDIN, 
    [val](u64 i) { return i==static_cast<u64>(val); });
}

template <typename Iter>
inline static bool has_multi_max (Iter first) {
  auto last = first + CARDIN;
  for (const auto max_pos=std::max_element(first,last); last-- != first;)
    if (*last==*max_pos && last!=max_pos)
      return true;
  return false;
}

template <typename Iter, typename PosIter>
inline static bool has_multi_max (Iter first, PosIter maxPos) {
  for (auto last=first+CARDIN; last-- != first;)
    if (*last==*maxPos && last!=maxPos)
      return true;
  return false;
}

template <typename Iter>
inline static u8 best_sym (Iter first) {
  return static_cast<u8>(*std::max_element(first, first+CARDIN));
}

template <typename Iter>
inline static u8 best_sym_abs (Iter first) {
  const auto max_pos = std::max_element(first, first+CARDIN);
  return static_cast<u8>(has_multi_max(first, max_pos) ? 255 : max_pos-first);
}

template <typename OutIter, typename InIter>
inline static void normalize (OutIter oFirst, InIter iFirst, InIter iLast) {
  const auto sumInv = prc_t(1) / accumulate(iFirst, iLast, prc_t(0));
  for (; iFirst!=iLast; ++iFirst,++oFirst)
    *oFirst = *iFirst * sumInv;
}

template <typename Value>
inline static bool is_odd (Value val) {
  if (val < 0)
    error("\"" + to_string(val) + "\" is a negative number.");
  return (val & 1ull);
}

template <typename T>
inline static auto pow2 (T base) {  // Must be inline
  return std::pow(base, static_cast<T>(2));
}

// From http://martin.ankerl.com/2007/10/04/
// optimized-pow-approximation-for-java-and-c-c/
inline static double Power (double base, double exponent) {
  union {
    double d;
    int x[2];
  } u = { base };
  u.x[1] = int(exponent * (u.x[1] - 1072632447) + 1072632447);
  u.x[0] = 0;
  return u.d;
}
// inline static auto Power (double base, double exponent) {
//   int tmp  = (*(1 + (int*) &base)),
//       tmp2 = int(exponent * (tmp-1072632447) + 1072632447);
//   double p = 0.0;
//   *(1 + (int*) &p) = tmp2;
//   return p;
// }
// inline double Power(const double a, const double b) {
//   union {
//     double d;
//     struct {
//       int a;
//       int b;
//     } s;
//   } u = { a };
//   u.s.b = (int)(b * (u.s.b - 1072632447) + 1072632447);
//   u.s.a = 0;
//   return u.d;
// }

inline static void check_file (const string& name) {  // Must be inline
  ifstream f(name);
  if (!f) {
    f.close();
    error("the file \"" + name + "\" cannot be opened or is empty.");
  }
  else {
    bool foundChar {false};
    for (char c; f.get(c) && !foundChar;)
      if (c!=' ' && c!='\n' && c!='\t')
        foundChar = true;
    if (!foundChar)
      error("the file \"" + name + "\" is empty.");
    f.close();
  }
}

inline static u64 file_size (const string& name) {
  check_file(name);
  ifstream f(name, ifstream::ate | ifstream::binary);
  return static_cast<u64>(f.tellg());
}

inline static u64 file_lines (const string& name) {
  ifstream f(name);
  f.unsetf(ios_base::skipws);  // New lines will be skipped unless we stop it
  return u64(count(istream_iterator<char>(f), istream_iterator<char>(), '\n'));
}

// Must be inline
inline static void extract_subseq (const unique_ptr<SubSeq>& subseq) {
  ifstream fIn(subseq->inName);
  ofstream fOut(subseq->outName);

  fIn.seekg(subseq->begPos);
  vector<char> buffer(static_cast<u64>(subseq->size), 0);
  fIn.read(buffer.data(), subseq->size);
  fOut.write(buffer.data(), subseq->size);

  fIn.close();  fOut.close();
}

inline static string gen_name (u32 ID, const string& ref, const string& tar,
const Format& frmt) {
  switch (frmt) {
  case Format::PROFILE:   return to_string(ID)+"-"+ref+"_"+tar+"."+FMT_PRF;
  case Format::FILTER:    return to_string(ID)+"-"+ref+"_"+tar+"."+FMT_FIL;
  case Format::POSITION:  return to_string(ID)+"-"+ref+"_"+tar+"."+FMT_POS;
  case Format::SEGMENT:   return to_string(ID)+"-"+ref+"_"+tar+"-"+LBL_SEG;
  default:                return "";
  }
}

inline static FileType file_type (const string& name) {
  check_file(name);
  ifstream f(name);
  char c;
  while (f.peek()=='\n' || f.peek()==' ')  f.get(c); //Skip leading blank spaces

  // Fastq
  while (f.peek()=='@')  ignore_this_line(f);
  for (u8 nTabs=0; f.get(c) && c!='\n';)  if (c=='\t') ++nTabs;
  if (f.peek()=='+') { f.close();  return FileType::FASTQ; }

  // Fasta or bare Seq
  f.clear();  f.seekg(0, std::ios::beg);
  while (f.peek()!='>' && f.peek()!=EOF)  ignore_this_line(f);
  if (f.peek()=='>') { f.close();  return FileType::FASTA; }
  else               { f.close();  return FileType::SEQ;   }
}

inline static void to_seq (const string& inName, const string& outName, 
const FileType& type) {
  ifstream fIn(inName);
  ofstream fOut(outName);

  if (type == FileType::FASTA) {
    bool isHeader = false;  // MUST be positioned before the following loop
    for (vector<char> buffer(FILE_BUF,0); fIn.peek()!=EOF;) {
      fIn.read(buffer.data(), FILE_BUF);
      string out;
      for (auto it=buffer.begin(); it!=buffer.begin()+fIn.gcount(); ++it) {
        const auto c = *it;
        if      (c=='>')  {               isHeader=true;   continue; }
        else if (c=='\n') { if (isHeader) isHeader=false;  continue; }
        else if (isHeader)                                 continue;
        else if (c>64 && c<123)
          out += c;
      }
      fOut.write(out.data(), out.size());
    }
  }
  else if (type == FileType::FASTQ) {
    u8   line  = 0;      // MUST be positioned before the following loop
    bool isDNA = false;  // MUST be positioned before the following loop
    for (vector<char> buffer(FILE_BUF,0); fIn.peek()!=EOF;) {
      fIn.read(buffer.data(), FILE_BUF);
      string out;
      for (auto it=buffer.begin(); it!=buffer.begin()+fIn.gcount(); ++it) {
        const auto c = *it;
        switch (line) {
        case 0:  if (c=='\n') { line=1;  isDNA=true;  }  break;
        case 1:  if (c=='\n') { line=2;  isDNA=false; }  break;
        case 2:  if (c=='\n') { line=3;  isDNA=false; }  break;
        case 3:  if (c=='\n') { line=0;  isDNA=false; }  break;
        default:                                         break;
        }
        if (!isDNA || c=='\n')  continue;
        if (c>64 && c<123)
          out += c;
      }
      fOut.write(out.data(), out.size());
    }
  }

  fIn.close();  fOut.close();
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
inline static string string_format(const string& format, Args... args) {
  // Extra space for '\0'
  auto size = size_t(snprintf(nullptr, 0, format.c_str(), args...) + 1);
  unique_ptr<char[]> buf(new char[size]);
  snprintf(buf.get(), size, format.c_str(), args...);
  return string(buf.get(), buf.get()+size-1);  // We don't want the '\0' inside
}

template <typename Val, typename MinVal, typename MaxVal>
inline static void keep_in_range (MinVal min, Val& val, MaxVal max) {
  if      (val < min)    val = min;
  else if (val > max)    val = max;
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

inline static void save_n_pos (const string& fileName) {
  ifstream inFile(fileName);
  ofstream NFile(fileName+".n");
  u64 pos=0, beg=0, num=0;
  bool begun = false;

  for (char c; inFile.get(c); ++pos) {
    if (c=='N' || c=='n') {
      if (!begun) {
        begun = true;
        beg = pos;
      }
      ++num;
    }
    else {
      begun = false;
      if (num != 0)
        NFile << beg << '\t' << beg+num-1 << '\n';
      num = 0;
      beg = 0;
    }
  }

  inFile.close();  NFile.close();
}

}

#endif //PROJECT_FN_HPP