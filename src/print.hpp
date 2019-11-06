// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.

#ifndef SMASHPP_PRINT_HPP
#define SMASHPP_PRINT_HPP

#include <iostream>
#include <string>
#include "string.hpp"

namespace smashpp {
class Column {
 public:
  Align align;
  uint8_t width;
  std::string text;
  std::pair<std::string, std::string> pre_post;

  Column() = default;
  Column(Align align_, uint8_t width_, std::string text_)
      : align(align_), width(width_), text(text_) {}
  Column(uint8_t width_, std::string text_)
      : Column(Align::left, width_, text_) {}
  Column(std::string text_) : Column(text_.size() + 1, text_) {}
};

class Row {
 public:
  void add_col(std::string);
  void add_col(const Column&);
  void print();

 private:
  // uint8_t num_columns;
  std::vector<Column> vec_col;
};

void Row::add_col(std::string text) { vec_col.push_back(Column(text)); }

void Row::add_col(const Column& column) { vec_col.push_back(column); }

void Row::print() {
  for (const auto& column : vec_col) {
    std::cerr << ((column.align == Align::left)
                      ? std::left
                      : (column.align == Align::right) ? std::right
                                                       : std::internal)
              << std::setw(column.width) << column.pre_post.first << column.text
              << column.pre_post.second;

    // switch (column.align) {
    //   case Align::left:
    //     std::cerr << std::left << std::setw(column.width)
    //               << column.pre_post.first << column.text
    //               << column.pre_post.second;
    //     break;
    //   case Align::right:
    //     std::cerr << std::right << std::setw(column.width)
    //               << column.pre_post.first << column.text
    //               << column.pre_post.second;
    //     break;
    //   case Align::internal:
    //     std::cerr << std::internal << std::setw(column.width)
    //               << column.pre_post.first << column.text
    //               << column.pre_post.second;
    //     break;
    // }
  }
  std::cerr << '\n';
}

void print_title(std::string str) { std::cerr << bold(str) << '\n'; }

void print_line(const std::string& str) { std::cerr << "  " << str << '\n'; }

// W/o type & w/o default value
void print_align(std::string arg, std::string delim_description,
                 const std::string& description) {
  std::cerr << "  ";
  std::cerr << std::left << std::setw((arg.substr(0, 4) == "\033[1m") ? 27 : 19)
            << arg;
  std::cerr << std::left << std::setw(2) << delim_description;
  std::cerr << std::left << std::setw(38) << description << '\n';
}

// W/o type & w/ default value
void print_align(std::string arg, std::string delim_description,
                 const std::string& description, std::string delim_default,
                 std::string default_val) {
  std::cerr << "  ";
  std::cerr << std::left << std::setw((arg.substr(0, 4) == "\033[1m") ? 27 : 19)
            << arg;
  std::cerr << std::left << std::setw(2) << delim_description;
  std::cerr << std::left << std::setw(38) << description;
  std::cerr << std::left << std::setw(3) << delim_default;
  std::cerr << default_val << '\n';
}

// W/ type & w/o default value
void print_align(std::string arg, std::string type,
                 std::string delim_description,
                 const std::string& description) {
  std::cerr << "  " << std::left << std::setw(12) << arg;
  std::cerr << std::left << std::setw(15)
            << ((type.empty() || type[0] == '[') ? type : "<" + type + ">");
  std::cerr << std::left << std::setw(2) << delim_description;
  std::cerr << std::left << std::setw(38) << description << '\n';
}

// W/ type & w/ default value
void print_align(std::string arg, std::string type,
                 std::string delim_description, const std::string& description,
                 std::string delim_default, std::string default_val) {
  std::cerr << "  " << std::left << std::setw(12) << arg;
  std::cerr << std::left << std::setw(15)
            << ((type.empty() || type[0] == '[') ? type : "<" + type + ">");
  std::cerr << std::left << std::setw(2) << delim_description;
  std::cerr << std::left << std::setw(38) << description;
  std::cerr << std::left << std::setw(3) << delim_default;
  std::cerr << default_val << '\n';
}

// // 3 columns: arg, desciption, default
// void print_aligned_3(std::string arg, const std::string& description,
// std::string default_val="", std::string delim_description="=",std::string
// delim_default = "") {
//   std::cerr << "  ";
//   std::cerr << std::left << std::setw(19) << arg;
//   std::cerr << std::left << std::setw(2) << delim_description;
//   std::cerr << std::left << std::setw(38) << description;
//   std::cerr << std::left << std::setw(3) << delim_default;
//   std::cerr << default_val << '\n';
// }

// // 4 columns: arg, type, desciption, default
// void print_aligned_4(std::string arg, std::string type, const std::string&
// description, std::string default_val="", std::string
// delim_description="=",std::string delim_default = "") {
//   std::cerr << "  ";
//   std::cerr << std::left << std::setw(12) << arg;
//   std::cerr << std::left << std::setw(15)
//             << ((type.empty() || type[0] == '[') ? type : "<" + type + ">");
//   std::cerr << std::left << std::setw(2) << delim_description;
//   std::cerr << std::left << std::setw(38) << description;
//   std::cerr << std::left << std::setw(3) << delim_default;
//   std::cerr << default_val << '\n';
// }

// // 6 columns
// void print_aligned(std::string arg, std::string type,
//                    std::string delim_description,
//                    const std::string& description,
//                    std::string delim_default = "",
//                    std::string default_val = "") {
//   std::cerr << "  ";
//   std::cerr << std::left << std::setw(13) << bold(arg);
//   std::cerr << std::left << std::setw(15)
//             << ((type.empty() || type[0] == '[') ? type : "<" + type + ">");
//   std::cerr << std::left << std::setw(2) << delim_description;
//   std::cerr << std::left << std::setw(38) << description;
//   std::cerr << std::left << std::setw(3) << delim_default;
//   std::cerr << default_val << '\n';
// }

void print_aligned_right_left(std::string arg, std::string type,
                              std::string delim_description,
                              const std::string& description,
                              std::string delim_default = "",
                              std::string default_val = "") {
  std::cerr << "  ";
  std::cerr << std::left << std::setw(12) << bold(arg);
  std::cerr << std::right << std::setw(15)
            << ((type.empty() || type[0] == '[') ? type : "<" + type + ">");
  std::cerr << std::left << std::setw(2) << delim_description;
  std::cerr << std::left << std::setw(38) << description;
  std::cerr << std::left << std::setw(3) << delim_default;
  std::cerr << default_val << '\n';
}
}  // namespace smashpp

#endif