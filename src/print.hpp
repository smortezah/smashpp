// Smash++
// Morteza Hosseini    mhosayny@gmail.com

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
  explicit Column(std::string text_) : Column(text_.size() + 1, text_) {}
};

class Row {
 public:
  void add_col(std::string);
  void add_col(const Column&);
  void print();

 private:
  std::vector<Column> vec_col;
};

void Row::add_col(std::string text) { vec_col.push_back(Column(text)); }

void Row::add_col(const Column& column) { vec_col.push_back(column); }

void Row::print() {
  for (const auto& column : vec_col) {
    std::cerr << ((column.align == Align::left)    ? std::left
                  : (column.align == Align::right) ? std::right
                                                   : std::internal)
              << std::setw(column.width) << column.pre_post.first << column.text
              << column.pre_post.second;
  }
  std::cerr << '\n';
}
}  // namespace smashpp

#endif