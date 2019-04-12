// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.

#ifndef SMASHPP_SEGMENT_HPP
#define SMASHPP_SEGMENT_HPP

#include <fstream>
// #include "def.hpp"
#include "par.hpp"
#include "number.hpp"

namespace smashpp {
struct PosRow {
  int64_t beg_ref;
  int64_t end_ref;
  double ent_ref;
  double self_ent_ref;
  int64_t beg_tar;
  int64_t end_tar;
  double ent_tar;
  double self_ent_tar;

  PosRow()
      : beg_ref(DBLANK),
        end_ref(DBLANK),
        ent_ref(0.0),
        self_ent_ref(0.0),
        beg_tar(DBLANK),
        end_tar(DBLANK),
        ent_tar(0.0),
        self_ent_tar(0.0) {}
  PosRow(uint64_t beg_ref_, uint64_t end_ref_, double ent_ref_,
         double self_ent_ref_, uint64_t beg_tar_=DBLANK, uint64_t end_tar_=DBLANK,
         double ent_tar_=0.0, double self_ent_tar_=0.0)
      : beg_ref(beg_ref_),
        end_ref(end_ref_),
        ent_ref(ent_ref_),
        self_ent_ref(self_ent_ref_),
        beg_tar(beg_tar_),
        end_tar(end_tar_),
        ent_tar(ent_tar_),
        self_ent_tar(self_ent_tar_) {}
  PosRow(uint64_t beg_ref_, uint64_t end_ref_, double ent_ref_,
         double self_ent_ref_, uint64_t beg_tar_, uint64_t end_tar_,
         double ent_tar_, double self_ent_tar_)
      : beg_ref(beg_ref_),
        end_ref(end_ref_),
        ent_ref(ent_ref_),
        self_ent_ref(self_ent_ref_),
        beg_tar(beg_tar_),
        end_tar(end_tar_),
        ent_tar(ent_tar_),
        self_ent_tar(self_ent_tar_) {}
};

// struct PosOut {
//   struct Row {
//     int64_t beg_ref;
//     int64_t end_ref;
//     double ent_ref;
//     double self_ent_ref;
//     int64_t beg_tar;
//     int64_t end_tar;
//     double ent_tar;
//     double self_ent_tar;

//     Row()
//         : beg_ref(DBLANK),
//           end_ref(DBLANK),
//           ent_ref(0.0),
//           self_ent_ref(0.0),
//           beg_tar(DBLANK),
//           end_tar(DBLANK),
//           ent_tar(0.0),
//           self_ent_tar(0.0) {}
//     Row(uint64_t beg_ref_, uint64_t end_ref_, double ent_ref_,
//         double self_ent_ref_, uint64_t beg_tar_, uint64_t end_tar_,
//         double ent_tar_, double self_ent_tar_)
//         : beg_ref(beg_ref_),
//           end_ref(end_ref_),
//           ent_ref(ent_ref_),
//           self_ent_ref(self_ent_ref_),
//           beg_tar(beg_tar_),
//           end_tar(end_tar_),
//           ent_tar(ent_tar_),
//           self_ent_tar(self_ent_tar_) {}
//   };

//   std::vector<Row> row;
//   int current_row;

//   PosOut() : current_row(0) { row.push_back(Row()); };
//   void add_row(uint64_t beg_ref, uint64_t end_ref, double ent_ref,
//                double self_ent_ref, uint64_t beg_tar, uint64_t end_tar,
//                double ent_tar, double self_ent_tar) {
//     row.push_back(Row(beg_ref, end_ref, ent_ref, self_ent_ref, beg_tar, end_tar,
//                       ent_tar, self_ent_tar));
//   }
// };

class Segment {
 public:
  bool begun;
  uint64_t pos;
  uint64_t begPos;
  uint64_t endPos;
  uint64_t nSegs;
  float thresh;
  prc_t sumEnt;
  uint64_t numEnt;
  uint32_t minSize;
  uint64_t totalSize;
  int16_t beg_guard;
  int16_t end_guard;
  // std::unique_ptr<PosOut> pos_out;
  std::vector<PosRow> pos_out;
  // int current_row;

  Segment()
      : begun(false),
        pos(0),
        begPos(0),
        endPos(0),
        nSegs(0),
        thresh(0),
        sumEnt(0),
        numEnt(0),
        minSize(MIN_SSIZE)
        // ,
        // pos_out(std::make_unique<PosOut>()) ,
        // current_row(0)
        {}

  void partition(std::ofstream&, float);
  void partition_last(std::ofstream&);
  void partition(float);
  void partition_last();
  void set_guards(uint8_t, int16_t, int16_t);
};
}  // namespace smashpp

#endif  // SMASHPP_SEGMENT_HPP