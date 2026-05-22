// SPDX-FileCopyrightText: 2018-2026 Morteza Hosseini
// SPDX-License-Identifier: GPL-3.0-only

#ifndef SMASHPP_FILE_HPP
#define SMASHPP_FILE_HPP

#include <algorithm>
#include <array>
#include <format>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string_view>

#include "exception.hpp"
#include "par.hpp"

#define JSON_ASSERT(x)                                        \
  if (!(x)) {                                                 \
    fprintf(stderr, "assertion error in %s\n", __FUNCTION__); \
    std::abort();                                             \
  }

#include <nlohmann/json.hpp>
using json = nlohmann::ordered_json;

namespace smashpp {
static constexpr float PI{3.14159265f};
extern void error(std::string&&);
static constexpr char INVALID_BASE{'\1'};

/// Builds the byte normalization lookup used while scanning sequence files.
///
/// Canonical bases are uppercased, IUPAC ambiguity bases are treated as `N`, whitespace returns
/// `\0` so callers can skip it, and every other byte is marked invalid for a later diagnostic that
/// includes the source file name.
[[nodiscard]] constexpr auto make_base_normalization_lookup() {
  std::array<char, 256> lookup{};

  for (auto& value : lookup) {
    value = INVALID_BASE;
  }

  lookup[static_cast<unsigned char>('A')] = 'A';
  lookup[static_cast<unsigned char>('a')] = 'A';
  lookup[static_cast<unsigned char>('C')] = 'C';
  lookup[static_cast<unsigned char>('c')] = 'C';
  lookup[static_cast<unsigned char>('G')] = 'G';
  lookup[static_cast<unsigned char>('g')] = 'G';
  lookup[static_cast<unsigned char>('T')] = 'T';
  lookup[static_cast<unsigned char>('t')] = 'T';

  for (const auto base : {'N', 'n', 'R', 'r', 'Y', 'y', 'S', 's', 'W', 'w', 'K',
                          'k', 'M', 'm', 'B', 'b', 'D', 'd', 'H', 'h', 'V', 'v'}) {
    lookup[static_cast<unsigned char>(base)] = 'N';
  }

  for (const auto space : {'\n', '\r', ' ', '\t'}) {
    lookup[static_cast<unsigned char>(space)] = '\0';
  }

  return lookup;
}

inline constexpr auto BASE_NORMALIZATION_LOOKUP = make_base_normalization_lookup();

struct SubSeq {
  std::string inName;
  std::string outName;
  uint64_t begPos;
  std::streamsize size;
};

inline static void ignore_this_line(std::ifstream& fs) {
  fs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

[[nodiscard]] inline static bool is_sequence_space(char c) {
  return c == '\n' || c == '\r' || c == ' ' || c == '\t';
}

[[nodiscard]] inline static std::string char_label(char c) {
  switch (c) {
    case '\n':
      return "\\n";
    case '\r':
      return "\\r";
    case '\t':
      return "\\t";
    default:
      return std::format("{}", c);
  }
}

/// Normalizes a sequence byte for compression and model construction.
///
/// \return `A`, `C`, `G`, `T`, or `N` for sequence symbols, `\0` for ignored whitespace. Invalid
/// bytes report an error using `source` for context.
[[nodiscard]] inline static char normalize_base(char c, std::string_view source) {
  const auto normalized = BASE_NORMALIZATION_LOOKUP[static_cast<unsigned char>(c)];
  if (normalized != INVALID_BASE) {
    return normalized;
  }

  error(std::format("invalid base \"{}\" in \"{}\".", char_label(c), source));
  return '\0';
}

inline static void check_file(const std::string& name) {  // Must be inline
  std::ifstream f(name);
  if (!f) {
    f.close();
    error(std::format("the file \"{}\" cannot be opened or is empty.", name));
  } else {
    bool foundChar{false};
    for (char c; f.get(c) && !foundChar;) {
      if (c != ' ' && c != '\n' && c != '\t') {
        foundChar = true;
      }
    }
    if (!foundChar) {
      error(std::format("the file \"{}\" is empty.", name));
    }
    f.close();
  }
}

[[nodiscard]] inline static bool file_is_empty(const std::string& name) {
  std::ifstream f(name);
  bool foundChar{false};

  for (char c; f.get(c) && !foundChar;) {
    if (c != ' ' && c != '\n' && c != '\t') {
      foundChar = true;
    }
  }

  f.close();
  return !foundChar;
}

[[nodiscard]] inline static std::string file_name(std::string_view str) {
  const auto found = str.find_last_of("/\\");
  return std::string{str.substr(found + 1)};
}

[[nodiscard]] inline static std::string file_name_no_ext(std::string_view str) {
  auto found = str.find_last_of("/\\");
  const auto file_name = str.substr(found + 1);
  found = file_name.find_last_of(".");
  return std::string{file_name.substr(0, found)};
}

[[nodiscard]] inline static uint64_t file_size(const std::string& name) {
  check_file(name);
  std::ifstream f(name, std::ifstream::ate | std::ifstream::binary);
  return static_cast<uint64_t>(f.tellg());
}

[[nodiscard]] inline static uint64_t file_lines(const std::string& name) {
  std::ifstream f(name);
  f.unsetf(std::ios_base::skipws);  // New lines will be skipped unless we stop it
  return static_cast<uint64_t>(
      std::count(std::istream_iterator<char>(f), std::istream_iterator<char>(), '\n'));
}

// Must be inline
inline static void extract_subseq(const SubSeq& subseq) {
  if (subseq.size <= 0) {
    return;
  }

  std::ifstream in_file(subseq.inName);
  std::ofstream out_file(subseq.outName);

  in_file.seekg(subseq.begPos);
  std::vector<char> buffer(static_cast<uint64_t>(subseq.size), 0);
  in_file.read(buffer.data(), subseq.size);
  out_file.write(buffer.data(), subseq.size);

  in_file.close();
  out_file.close();
}

inline static FileType file_type(std::string name) {
  check_file(name);
  std::ifstream f(name);
  char c;
  while (f.peek() == '\n' || f.peek() == ' ') {  // Skip leading blank spaces
    f.get(c);
  }

  // Fastq
  while (f.peek() == '@') {
    ignore_this_line(f);
  }
  while (f.get(c) && c != '\n') {
  }
  if (f.peek() == '+') {
    f.close();
    return FileType::fastq;
  }

  // Fasta or bare Seq
  f.clear();
  f.seekg(0, std::ios::beg);
  while (f.peek() != '>' && f.peek() != EOF) {
    ignore_this_line(f);
  }
  if (f.peek() == '>') {
    f.close();
    return FileType::fasta;
  } else {
    f.close();
    return FileType::seq;
  }
}

inline static void to_seq(std::string inName, std::string outName, const FileType& type) {
  std::ifstream in_file(inName);
  std::ofstream out_file(outName);
  std::string out;
  out.reserve(FILE_READ_BUF);

  const auto append_base = [&](char c) {
    const auto base = normalize_base(c, inName);
    if (base != '\0') {
      out.push_back(base);
    }
  };
  const auto flush_out = [&]() {
    out_file.write(out.data(), static_cast<std::streamsize>(out.size()));
    out.clear();
  };

  if (type == FileType::fasta) {
    bool is_header{false};
    bool line_start{true};

    for (std::vector<char> buffer(FILE_READ_BUF, 0); in_file.peek() != EOF;) {
      in_file.read(buffer.data(), FILE_READ_BUF);
      for (auto it = std::begin(buffer); it != std::begin(buffer) + in_file.gcount(); ++it) {
        const auto c = *it;
        if (line_start && c == '>') {
          is_header = true;
          line_start = false;
          continue;
        }
        if (c == '\n') {
          is_header = false;
          line_start = true;
          continue;
        }
        if (c == '\r') {
          continue;
        }
        if (is_header) {
          continue;
        }
        line_start = false;
        append_base(c);
      }
      flush_out();
    }
  } else if (type == FileType::fastq) {
    uint8_t line{0};
    bool is_dna{false};

    for (std::vector<char> buffer(FILE_READ_BUF, 0); in_file.peek() != EOF;) {
      in_file.read(buffer.data(), FILE_READ_BUF);
      for (auto it = std::begin(buffer); it != std::begin(buffer) + in_file.gcount(); ++it) {
        const auto c{*it};
        if (c == '\r') {
          continue;
        }
        switch (line) {
          case 0:
            if (c == '\n') {
              line = 1;
              is_dna = true;
            }
            break;
          case 1:
            if (c == '\n') {
              line = 2;
              is_dna = false;
            }
            break;
          case 2:
            if (c == '\n') {
              line = 3;
              is_dna = false;
            }
            break;
          case 3:
            if (c == '\n') {
              line = 0;
              is_dna = false;
            }
            break;
          default:
            break;
        }
        if (!is_dna || c == '\n') {
          continue;
        }
        append_base(c);
      }
      flush_out();
    }
  }

  in_file.close();
  out_file.close();
}

inline static json parse_pos_metadata(const std::string& pos) {
  json j;
  std::istringstream iss(pos);
  std::string line;

  // Watermark
  std::getline(iss, line);
  j["watermark"] = line;

  // Parameters
  std::getline(iss, line);
  line = line.substr(line.find("<") + 1);
  line.pop_back();
  j["parameters"] = line;

  // Info
  std::getline(iss, line);
  std::string ref, refsize, tar, tarsize;
  std::istringstream info_ss(line);
  std::getline(info_ss, ref, ',');
  std::getline(info_ss, refsize, ',');
  std::getline(info_ss, tar, ',');
  std::getline(info_ss, tarsize, ',');
  ref = ref.substr(ref.find("<") + 1);
  if (!tarsize.empty() && tarsize.back() == '>') {
    tarsize.pop_back();
  }
  j["reference"] = ref.substr(ref.find("=") + 1);
  j["reference_size"] = refsize.substr(refsize.find("=") + 1);
  j["target"] = tar.substr(tar.find("=") + 1);
  j["target_size"] = tarsize.substr(tarsize.find("=") + 1);

  return j;
}

inline static std::string pos2json(const std::string& pos_content) {
  json j = parse_pos_metadata(pos_content);
  j["positions"] = json::array();
  std::istringstream iss(pos_content);

  for (std::string line; std::getline(iss, line);) {
    if (line[0] == '#') {
      continue;
    }
    std::istringstream line_ss(line);
    std::string rbeg, rend, rrelrdn, rrdn, tbeg, tend, trelrdn, trdn, inv;
    line_ss >> rbeg >> rend >> rrelrdn >> rrdn >> tbeg >> tend >> trelrdn >> trdn >> inv;
    j["positions"].push_back({{"reference_begin", rbeg},
                              {"reference_end", rend},
                              {"reference_relative_redundancy", rrelrdn},
                              {"reference_redundancy", rrdn},
                              {"target_begin", tbeg},
                              {"target_end", tend},
                              {"target_relative_redundancy", trelrdn},
                              {"target_redundancy", trdn},
                              {"inverted", inv}});
  }

  return j.dump(4);
}
}  // namespace smashpp

#endif  // SMASHPP_FILE_HPP
