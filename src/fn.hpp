//
// Created by morteza on 03-05-2018.
//

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

template <typename Input>
inline static bool is_u8 (Input&& in) {
  return typeid(in)==typeid(u8);
}

#ifdef DEBUG
// Print variadic inputs
template <typename Integral>
static void print (Integral&& in) /*noexcept*/ {
  cerr << (is_u8(in) ? static_cast<u32>(in) : in) << '\n';
}
template <typename Integral, typename... Args>
static void print (Integral&& in, Args&&... args) /*noexcept*/ {
  cerr << (is_u8(in) ? static_cast<u32>(in) : in) << '\t';
  print(args...);
}
#endif

// "inline" is a MUST -- not to get "multiple definition of `now()'" error
inline static chrono::time_point<std::chrono::high_resolution_clock> now ()
noexcept {
  return chrono::high_resolution_clock::now();
}

template <typename Time>
inline static string hms (Time elapsed) {
//  std::chrono::duration<double, std::milli> ms = elapsed;
  const auto durSec =
    chrono::duration_cast<std::chrono::seconds>(elapsed).count();
  const auto h = durSec / 3600;
  const auto m = (durSec % 3600) / 60;
  const auto s = durSec % 60;

  return to_string(h)+":"+to_string(m)+":"+to_string(s)+" hour:min:sec.\n";
}

inline static void ignore_this_line (ifstream& fs) {
  fs.ignore(numeric_limits<std::streamsize>::max(), '\n');
}

// Split a range by delim and insert the result into an std::vector
template <typename InIter, typename Vec>
inline static void split (InIter first, InIter last, char delim, Vec& vOut) {
  while (true) {
    InIter found = std::find(first, last, delim);
    vOut.emplace_back(string(first,found));
    if (found == last)
      break;
    first = ++found;
  }
}

// "inline" is a MUST -- not to get "multiple definition of `now()'" error
inline static void error (const string& msg) {
  throw std::runtime_error ("Error: " + msg + "\n");
}
inline static void error (string&& msg) {
  throw std::runtime_error ("Error: " + msg + "\n");
}

inline static void err (const string& msg) {
  cerr << "Error: " << msg << '\n';
}

template <typename Iter, typename Element>
inline static bool has (Iter first, Iter last, Element elem) {
  return std::find(first, last, elem) != last;
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
  return std::all_of(first, first+CARDIN,
                     [val](u64 i){ return i==static_cast<u64>(val); });
}

template <typename Iter, typename Value>
inline static bool is_any (Iter first, Value val) {
  return std::any_of(first, first+CARDIN,
                     [val](u64 i){ return i==static_cast<u64>(val); });
}

template <typename Iter>
inline static bool are_all_zero (Iter first) {
  return std::all_of(first, first+CARDIN, [](u64 i){ return i==0; });
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

//template <typename Iter>
//void normalize (Iter first, Iter last) {
//  for (const auto sum=std::accumulate(first,last,static_cast<prec_t>(0));
//       first!=last; ++first)
//    *first /= sum;    // *first = *first / sum;
//}
template <typename OutIter, typename InIter>
inline static void normalize (OutIter oFirst, InIter iFirst, InIter iLast) {
  const auto sumInv =
    static_cast<prec_t>(1)/std::accumulate(iFirst,iLast,static_cast<prec_t>(0));
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
  return static_cast<u64>(
    std::count(istream_iterator<char>(f), istream_iterator<char>(), '\n'));
}

// Must be inline
inline static void extract_subseq (const shared_ptr<SubSeq>& subseq) {
  ifstream fIn(subseq->inName);
  ofstream fOut(subseq->outName);

  fIn.seekg(subseq->begPos);
  vector<char> buffer(static_cast<u64>(subseq->size), 0);
  fIn.read(buffer.data(), subseq->size);
  fOut.write(buffer.data(), subseq->size);

  fIn.close();  fOut.close();

  // Slower
//  char c;
//  for (auto i=subseq->endPos-subseq->begPos; i-- && fIn.get(c);)
//    fOut << c;
}

inline static string gen_name
(const string& ref, const string& tar, const Format& frmt) {
  switch (frmt) {
  case Format::PROFILE:   return ref+"-"+tar+"."+FMT_PRF;
  case Format::FILTER:    return ref+"-"+tar+"."+FMT_FIL;
  case Format::POSITION:  return ref+"-"+tar+"."+FMT_POS;
  case Format::SEGMENT:   return ref+"-"+tar+"-"+LBL_SEG;
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

inline static void to_seq
(const string& inName, const string& outName, const FileType& type) {
  ifstream fIn(inName);
  ofstream fOut(outName);

  if (type == FileType::FASTA) {
    bool isHeader = false;  // MUST be positioned before the following loop
    for (vector<char> buffer(FILE_BUF,0); fIn;) {
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
    for (vector<char> buffer(FILE_BUF,0); fIn;) {
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

template <typename P, typename T>
inline static void show_progress (P pos, T total) {
  if (pos % (total/100) == 0) {
    cerr << "Progress: [" << static_cast<int>((pos*100) / total) << "%]\r";
//    flush(cerr);
  }
}

inline static void remove_progress_trace () {
  cerr << string(21, ' ') << '\r';  // Remove the trace of [...%] progress
}

#endif //PROJECT_FN_HPP