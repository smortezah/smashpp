// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.

#ifndef SMASHPP_FILE_HPP
#define SMASHPP_FILE_HPP

#include <iterator>
#include <algorithm>
#include <fstream>
#include "par.hpp"
#include "exception.hpp"

namespace smashpp {
static constexpr float PI{3.14159265f};
extern void error(std::string&&);

struct SubSeq {
  std::string inName;
  std::string outName;
  uint64_t begPos;
  std::streamsize size;
};

inline static void ignore_this_line(std::ifstream& fs) {
  fs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

inline static void check_file(std::string name) {  // Must be inline
  std::ifstream f(name);
  if (!f) {
    f.close();
    error("the file \"" + name + "\" cannot be opened or is empty.");
  } else {
    bool foundChar{false};
    for (char c; f.get(c) && !foundChar;)
      if (c != ' ' && c != '\n' && c != '\t') foundChar = true;
    if (!foundChar) error("the file \"" + name + "\" is empty.");
    f.close();
  }
}

inline static bool file_is_empty(std::string name) {
  std::ifstream f(name);
  bool foundChar{false};

  for (char c; f.get(c) && !foundChar;)
    if (c != ' ' && c != '\n' && c != '\t') foundChar = true;
    
  f.close();
  return !foundChar;
}

inline static std::string file_name(std::string str) {
  const auto found = str.find_last_of("/\\");
  return str.substr(found + 1);
}

inline static uint64_t file_size(std::string name) {
  check_file(name);
  std::ifstream f(name, std::ifstream::ate | std::ifstream::binary);
  return static_cast<uint64_t>(f.tellg());
}

inline static uint64_t file_lines(std::string name) {
  std::ifstream f(name);
  f.unsetf(
      std::ios_base::skipws);  // New lines will be skipped unless we stop it
  return static_cast<uint64_t>(std::count(std::istream_iterator<char>(f),
                                     std::istream_iterator<char>(), '\n'));
}

// Must be inline
inline static void extract_subseq(std::unique_ptr<SubSeq>& subseq) {
  if (subseq->size <= 0) return;

  std::ifstream in_file(subseq->inName);
  std::ofstream out_file(subseq->outName);

  in_file.seekg(subseq->begPos);
  std::vector<char> buffer(static_cast<uint64_t>(subseq->size), 0);
  in_file.read(buffer.data(), subseq->size);
  out_file.write(buffer.data(), subseq->size);

  in_file.close();
  out_file.close();
}

inline static FileType file_type(std::string name) {
  check_file(name);
  std::ifstream f(name);
  char c;
  while (f.peek() == '\n' || f.peek() == ' ')
    f.get(c);  // Skip leading blank spaces

  // Fastq
  while (f.peek() == '@') ignore_this_line(f);
  for (uint8_t nTabs = 0; f.get(c) && c != '\n';)
    if (c == '\t') ++nTabs;
  if (f.peek() == '+') {
    f.close();
    return FileType::fastq;
  }

  // Fasta or bare Seq
  f.clear();
  f.seekg(0, std::ios::beg);
  while (f.peek() != '>' && f.peek() != EOF) ignore_this_line(f);
  if (f.peek() == '>') {
    f.close();
    return FileType::fasta;
  } else {
    f.close();
    return FileType::seq;
  }
}

inline static void to_seq(std::string inName, std::string outName,
                          const FileType& type) {
  std::ifstream in_file(inName);
  std::ofstream out_file(outName);

  if (type == FileType::fasta) {
    bool isHeader{false};  // MUST be positioned before the following loop
    for (std::vector<char> buffer(FILE_BUF, 0); in_file.peek() != EOF;) {
      in_file.read(buffer.data(), FILE_BUF);
      std::string out;
      for (auto it = std::begin(buffer);
           it != std::begin(buffer) + in_file.gcount(); ++it) {
        const auto c = *it;
        if (c == '>') {
          isHeader = true;
          continue;
        } else if (c == '\n') {
          if (isHeader) isHeader = false;
          continue;
        } else if (isHeader) {
          continue;
        } else if (c > 64 && c < 123) {
          out += c;
        }
      }
      out_file.write(out.data(), out.size());
    }
  } else if (type == FileType::fastq) {
    uint8_t line{0};    // MUST be positioned before the following loop
    bool isDNA{false};  // MUST be positioned before the following loop
    for (std::vector<char> buffer(FILE_BUF, 0); in_file.peek() != EOF;) {
      in_file.read(buffer.data(), FILE_BUF);
      std::string out;
      for (auto it = std::begin(buffer);
           it != std::begin(buffer) + in_file.gcount(); ++it) {
        const auto c{*it};
        switch (line) {
          case 0:
            if (c == '\n') {
              line = 1;
              isDNA = true;
            }
            break;
          case 1:
            if (c == '\n') {
              line = 2;
              isDNA = false;
            }
            break;
          case 2:
            if (c == '\n') {
              line = 3;
              isDNA = false;
            }
            break;
          case 3:
            if (c == '\n') {
              line = 0;
              isDNA = false;
            }
            break;
          default:
            break;
        }
        if (!isDNA || c == '\n') continue;
        if (c > 64 && c < 123) out += c;
      }
      out_file.write(out.data(), out.size());
    }
  }

  in_file.close();
  out_file.close();
}
}  // namespace smashpp

#endif  // SMASHPP_FILE_HPP