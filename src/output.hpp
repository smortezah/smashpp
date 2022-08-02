// Smash++
// Morteza Hosseini    mhosayny@gmail.com

#ifndef SMASHPP_OUTPUT_HPP
#define SMASHPP_OUTPUT_HPP

#include <sstream>
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
  std::string param_list;
  struct Info {
    std::string ref;
    uint64_t ref_size;
    std::string tar;
    uint64_t tar_size;
    Info() = default;
  };
  std::unique_ptr<Info> info;

  PositionFile() : info(std::make_unique<Info>()) {}
  void dump(const std::vector<PosRow>&, bool, Format);

 private:
  auto stream_pos(std::ostringstream&, const std::vector<PosRow>&, bool) const
      -> std::ostringstream&;
  auto pos_pairs(const std::vector<PosRow>&) const -> std::vector<OutRowAux>;
  void stream_pos_impl(std::ostringstream&, const std::vector<OutRowAux>&,
                       bool) const;
};
}  // namespace smashpp

#endif  // SMASHPP_OUTPUT_HPP