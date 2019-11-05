// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.

#ifndef SMASHPP_OUTPUT_HPP
#define SMASHPP_OUTPUT_HPP

#include "naming.hpp"

namespace smashpp {
static constexpr uint8_t PREC_POS{4};  // Precision

struct OutRowAux {
  PosRow pos2;
  PosRow pos1;
  std::vector<PosRow> pos3;
  OutRowAux() = default;
  OutRowAux(PosRow pos2_, PosRow pos1_) : pos2(pos2_), pos1(pos1_) {}
};

class PositionFile {
 public:
  std::string name;
  std::string watermark;
  std::string param_list;
  struct Info {
    std::string ref;
    uint64_t ref_size;
    std::string tar;
    uint64_t tar_size;
    Info() = default;
    // Info() {};
  };
  std::unique_ptr<Info> info;

  PositionFile()
      : name("out.pos"),
        watermark(POS_WATERMARK),
        info(std::make_unique<Info>()) {}
  void write_pos_file(const std::vector<PosRow>&, bool);

 private:
  void make_write_pos_pair(const std::vector<PosRow>&,
                           const std::vector<PosRow>&,
                           const std::vector<PosRow>&, bool);
  void write_pos_file_impl(const std::vector<OutRowAux>&, bool);
};
}  // namespace smashpp

#endif  // SMASHPP_OUTPUT_HPP