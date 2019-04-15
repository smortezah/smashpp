// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.
//    ____                      _
//   / ___| _ __ ___   __ _ ___| |__    _     _
//   \___ \| '_ ` _ \ / _` / __| '_ \ _| |_ _| |_
//    ___) | | | | | | (_| \__ \ | | |_   _|_   _|
//   |____/|_| |_| |_|\__,_|___/_| |_| |_|   |_|

//                 ```.```
//           `-/oydmmmmmmmmdho:`
//          `smNNMMMMMMMMMNNMMNd+`
//          :hNNNmmdhhmmddddmmNNNh`
//         sNNmy+/::--:::--::/ohNN/
//        .NMNs/:----.-..-----:/hNy
//        :NMm+::---........--:/sNd`
//        -NNd/://+oo+/-:/+o+///sNh
//        `dNh/+yyhdddy:/dmmhhyooNo
//        `sds:+syydhho.-yddhys++h-
//        `+y+:---:::::.-:///:::/s-
//         :+/:--...-:-.--:-..-:/+-
//         .//::----:+oooo/:---:/:`
//          .//:::://+yyys//::///`
//           .++//+sssyyyssyo+++-
//            /+++//:/osso//+++/
//            :ooo+/::://///+oo.
//          `:/ossssoo+oooosssoso.
//         `sm++ossyyyyyyyyyso+smh`
//      ``.smdo++ossssyyyysoo++hNNh/.`
//  `.-:/+odmNmysoooooossoooshmNNNmsso/.`
// /+++++++odNNNNmdhhyssyhdmNNNNNdyooo+++/:
// ++++++++++ymNNNNNNmddmmmNNNNmhssooo+++++
// +++++++++++ohmmmmmmmNmddmmmdyysoooo+++++
// +++++++++++oossyyyhmmhhhhhhyysoooo++++++
// o+++++++++++++oosyhddyhhhyyssooo++++++++

// MMMMNMMNNMNNNNNNNNNMNNNNmmmmmNNNNNNNMNNN
// MMMMMMMNNMNNNNNNmmdddmmNNNmmmNNNMNNNNNNN
// MMMMMMMNNMMNmhso+////++shdNNNNNNMNmNNNNN
// MMMMMMMMNMMds+:-..``..-:/sdNNNNNNmNNMNNN
// MMMMMMMMMMmy+:.```  ``.-:+sdNMNMMMNMMMMN
// MMMMMMMMMMh+/:-.``````.-:/oymMMMMMNMMMMN
// MMMMMMMMMNsohhyo:``` `/yhhdddMMMMMMMMMMN
// MMMMMMMMMNyy/...`.```````.:sdNMMMMMMMMMM
// MMMMMMMMMNo/ooso/::://::+ssysmMMMMMMMMMM
// MMMMMMMMmdo+/.``.:/:+o+-..:osddMMMMMMMMM
// MMMMMMMMNyo/.```-/:-:oo:---:odhMMMMMMMMM
// MMMMMMMMM+y:.```.-:..//-```-+dyMMMMMMMMM
// MMMMMMMMMmm+-.`.://--+syo:-:oNMMMMMMMMMM
// MMMMMMMMMMNs/::+o:.```-o+//+yNMMMMMMMMMM
// MMMMMMMMMMMh//:://-..-/o//+sdMMMMMMMMMMM
// MMMMMMMMMMMNs+//-..``..-+sshmMMMMMMMMMMM
// MMMMMMMMMMMMsooo-```.`.-shhydMMMMMMMMMMM
// MMMMMMMMMMMMy::oo:---::+hs+odmddmMMMMMMN
// MMMMMMMMms+-o:..-/+//+//:::+yy:-::/sdmNN
// MMMMNhs:````.:.````````...-/o-...-.-:-:s

// +++/+:-://+/:---.---:-.-:::hyyyyyyyy..-/
// oo+/:::-:-/+:/++ooooo+++/:/ysosyhsss/:-.
// so+oo+o/:-:oyhhhhhyyyhhysyyys+/+oso//:o+
// sso///+sosyyyhhhhhyyyyyssssssss+::::/:/o
// /+/////oyssyhhhhdmmNNNNNmmmmmmdhs+/-----
// ossooooyhydNmdhhdddddhddddmmdddmds/:::-/
// /++/oohddmNdo+///++++++++++//::oddyooo+/
// ++o+/shdmmh/::-::///////::::----:ddsoo++
// o++//syhdho/:://++++++/////:::---ody+//+
// sooo/shhys+///+++++++++////////::/ho:/os
// ys/-/yhhs/////////++///////::::://s+so+o
// oo+o:sys+://///++oooooo/+++//::-:/o:oooo
// ++//s+++::/+osyhmmmmmhyo+ydddho////-/+++
// osso+sy/::/+sssyhhdddhs//ydmdhyy+/os::``
// ssso:++//::///+ossssso/:-/oyyo+//+osoo/-
// ++/://o+////++///+os+/-::-://////--::++:
// yssyy+++//++++++osyy+//+/:/+///////+oooo
// ssyyyso///+ooosyhyyyhdyhyo+os+++/+sssoo+
// ooooooo/++osssyhhhhyhhhhysossssoooos+ooo
// o+oooso++ossyyhhdddddhdhhhhhyssoooo+//+o
// ::+shyds+osyhhhhyyyyyyysoooyyyddhy+:.-:-
// ::+ydNm+osyyhhhhyyyyhdhysssyhmdmmmmmh+//
// hhyhNNd+osyhhhddhhyyyyyssyyodmmmmmNNmNmd
// hsymNNmsosyhhhyhhddddddhyssyNNdNmmmNNNNN
// yydmmmmhssyhddhhhdddhhyyssyNNNmdNNNNNNNN
// hhdmmmmmhhyyhhhhhdddhhyyyhNNNNNmmNNNNNNN
// hddmmNNmmddhhhhhhddhhhhhmNNNNNMNNNNNNNNN

// #define __STDC_FORMAT_MACROS
// #if defined(_MSC_VER)
// #include <io.h>
// #else
// #include <unistd.h>
// #endif

#include <iostream>
#include <chrono>
#include <iomanip>      // setw, setprecision
#include <thread>
#include "par.hpp"
#include "fcm.hpp"
#include "filter.hpp"
#include "container.hpp"
#include "time.hpp"
#include "naming.hpp"
#include "string.hpp"
#include "segment.hpp"
#include "vizpaint.hpp"
using namespace smashpp;

// template <typename Iterator>
// void make_pos_pair(Iterator left_first_iter, Iterator left_last_iter,
//                    Iterator right_first_iter, Iterator right_last_iter) {
//   for (auto right_begin = right_first_iter; left_first_iter != left_last_iter;
//        ++left_first_iter) {
//     left_first_iter->print();
//     // left_first_iter->show();
//     std::cerr << '\t';

//     for (; right_first_iter != right_last_iter; ++right_first_iter) {
//       auto seg_name{
//           gen_name(right_first_iter->run_num, right_first_iter->ref,
//                    right_first_iter->tar, Format::segment) +
//           std::to_string(std::distance(right_begin, right_first_iter))};
//       if (left_first_iter->ref == seg_name) {
//         right_first_iter->print();
//         // right_first_iter->show();
//         break;
//       }
//     }
//     std::cerr << '\n';
//   }
// }

void make_pos_pair(const std::vector<PosRow>& left,
                   const std::vector<PosRow>& right1) {
  for (auto right_first_iter = std::begin(right1),
            right_begin = right_first_iter, left_first_iter = std::begin(left);
       left_first_iter != std::end(left); ++left_first_iter) {
    left_first_iter->print();
    // left_first_iter->show();
    std::cerr << '\t';

    for (; right_first_iter != std::end(right1); ++right_first_iter) {
      auto seg_name{
          gen_name(right_first_iter->run_num, right_first_iter->ref,
                   right_first_iter->tar, Format::segment) +
          std::to_string(std::distance(right_begin, right_first_iter))};
      if (left_first_iter->ref == seg_name) {
        right_first_iter->print();
        // right_first_iter->show();
        break;
      }
    }
    std::cerr << '\n';
  }
}

void make_pos_pair(const std::vector<PosRow>& left,
                   const std::vector<PosRow>& right1,
                   const std::vector<PosRow>& right3) {
  struct Out {
    PosRow pos2;
    PosRow pos1;
    std::vector<PosRow> pos3;
  };
  std::vector<Out> out{};
  out.resize(1);

  uint64_t line = 0;
  for (auto right1_iter = std::begin(right1), right1_begin = right1_iter,
            left_iter = std::begin(left);
       left_iter != std::end(left); ++left_iter) {
    // left_iter->print();
    // left_iter->show();
    // Out tmp_out;
    // tmp_out.pos2=*left_iter;
    // out.push_back(tmp_out);
// out[line].pos2=*left_iter;
    std::cerr << '\t';

    for (; right1_iter != std::end(right1); ++right1_iter) {
      auto seg_name{gen_name(right1_iter->run_num, right1_iter->ref,
                             right1_iter->tar, Format::segment) +
                    std::to_string(std::distance(right1_begin, right1_iter))};
      if (left_iter->ref == seg_name) {
        // right_first_iter->print();
        // right1_iter->show();
        // out[line].pos1 = *right1_iter;
        break;
      }
    }
    std::cerr << '\n';

    ++line;
  }

  // todo
  // for (auto e : out) {
  //   e.pos2.show();
  //   e.pos1.show();
  //   for (auto ee : e.pos3) ee.show();
  // }
}

void write_pos_file(const std::vector<PosRow>& pos_out) {
  std::vector<PosRow> left_reg;
  std::vector<PosRow> left_ir;
  std::vector<PosRow> right1_reg;
  std::vector<PosRow> right1_ir;
  std::vector<PosRow> right3_reg;
  std::vector<PosRow> right3_ir;

  for (auto& row : pos_out) {
    if (row.round == 1 && row.run_num==0) right1_reg.push_back(PosRow(row));
    if (row.round == 1 && row.run_num==1) right1_ir.push_back(PosRow(row));
    if (row.round == 2 && row.run_num==0) left_reg.push_back(PosRow(row));
    if (row.round == 2 && row.run_num==1) left_ir.push_back(PosRow(row));
    if (row.round == 3 && row.run_num==0) right3_reg.push_back(PosRow(row));
    if (row.round == 3 && row.run_num==1) right3_ir.push_back(PosRow(row));
  }

  if (right3_reg.empty() && right3_ir.empty()) {
    make_pos_pair(left_reg, right1_reg);
    make_pos_pair(left_ir, right1_ir);
    // make_pos_pair(std::begin(left_reg), std::end(left_reg),
    //               std::begin(right1_reg), std::end(right1_reg));
    // make_pos_pair(std::begin(left_ir), std::end(left_ir),
    // std::begin(right1_ir),
    //               std::end(right1_ir));
  } else if (!right3_reg.empty() && right3_ir.empty()) {
    make_pos_pair(left_reg, right1_reg, right3_reg);
  } else if (right3_reg.empty() && !right3_ir.empty()) {
    make_pos_pair(left_ir, right1_ir, right3_ir);
  } else if (!right3_reg.empty() && !right3_ir.empty()) {
    make_pos_pair(left_reg, right1_reg, right3_reg);
    make_pos_pair(left_ir, right1_ir, right3_ir);
  }

  // todo
  std::cerr << '\n';
  for (auto row : right1_reg) {
    row.show();
    std::cerr << '\n';
  }
  std::cerr << '\n';
  for (auto row : right1_ir) {
    row.show();
    std::cerr << '\n';
  }
  std::cerr << '\n';
  for (auto row : left_reg) {
    row.show();
    std::cerr << '\n';
  }
  std::cerr << '\n';
  for (auto row : left_ir) {
    row.show();
    std::cerr << '\n';
  }
  std::cerr << '\n';
  for (auto row : right3_reg) {
    row.show();
    std::cerr << '\n';
  }
  std::cerr << '\n';
  for (auto row : right3_ir) {
    row.show();
    std::cerr << '\n';
  }
}

uint64_t run_round(std::unique_ptr<Param>& par, uint8_t round, uint8_t run_num,
                   std::vector<PosRow>& pos_out, uint64_t& current_pos_row) {
  par->ID = run_num;
  par->refName = file_name(par->ref);
  par->tarName = file_name(par->tar);
  auto models = std::make_unique<FCM>(par);

  if (round == 1 && run_num == 0)
    std::cerr << bold(
        "====[ REGULAR MODE ]==================================\n");
  else if (round == 1 && run_num == 1)
    std::cerr << bold(
        "====[ INVERTED MODE ]=================================\n");

  // Make all IRs consistent
  for (auto& ref_model : models->rMs) {
    ref_model.ir = run_num;
    if (ref_model.child) ref_model.child->ir = run_num;
  }
  for (auto& tar_model : models->tMs) {
    tar_model.ir = run_num;
    if (tar_model.child) tar_model.child->ir = run_num;
  }

  // Build models and Compress
  models->store(par);
  models->compress(par);

  // Filter and segment
  auto filter = std::make_unique<Filter>(par);
  if (!par->manThresh) par->thresh = static_cast<float>(models->aveEnt);
  filter->smooth_seg(pos_out, par, round, current_pos_row);

  if (filter->nSegs == 0) {
    if (round == 2) {
      pos_out.push_back(PosRow(0, 0, 0.0, 0.0, 0, par->ref, par->tar, 2));
    }
    std::cerr << '\n';
    return 0;  // continue;
  }
  filter->extract_seg(pos_out, round, run_num, par->ref);

  // Ref-free compress
  if (!par->noRedun) {
    if (par->verbose)
      std::cerr << ". . . . . . . . . . . . . . . . . . . "
                   ". . . . . . . . . .\n>>> "
                << italic("Reference-free compression of the segment")
                << italic(filter->nSegs == 1 ? "" : "s") << '\n';

    const auto seg{gen_name(par->ID, par->ref, par->tar, Format::segment)};
    models->selfEnt.reserve(filter->nSegs);
    for (uint64_t i = 0; i != filter->nSegs; ++i) {
      par->seq = seg + std::to_string(i);
      models->self_compress(par, i);
    }

    models->aggregate_slf_ent(pos_out, round, run_num,  par->ref,par->noRedun);
  }
  
  current_pos_row += filter->nSegs;
  return filter->nSegs;
}

void remove_temp_seg(std::unique_ptr<Param>& par, uint64_t seg_num) {
  const auto seg{gen_name(par->ID, par->ref, par->tar, Format::segment)};

  for (uint64_t i = 0; i != seg_num; ++i)
    if (!par->saveAll && !par->saveSegment) {
      // std::cerr << "\n++++++removed " << seg + std::to_string(i) <<
      // "+++++++\n";
      remove((seg + std::to_string(i)).c_str());
    }
}

void remove_temp_seq(std::unique_ptr<Param>& par) {
  if (!par->saveSeq) {
    if (par->refType == FileType::fasta || par->refType == FileType::fastq) {
      remove(par->ref.c_str());
      rename((par->ref + LBL_BAK).c_str(), par->ref.c_str());
    }
    if (par->tarType == FileType::fasta || par->tarType == FileType::fastq) {
      remove(par->tar.c_str());
      rename((par->tar + LBL_BAK).c_str(), par->tar.c_str());
    }
  }
}

void run(std::unique_ptr<Param>& par) {
  std::string ref_round1 = par->ref;
  std::string tar_round1 = par->tar;
  auto filter = std::make_unique<Filter>();

  std::vector<PosRow> pos_out;
  uint64_t current_pos_row = 0;

  for (uint8_t run_num = 0; run_num != 2/*1*/; ++run_num) {
    // Round 1
    auto num_seg_round1 = run_round(par, 1, run_num, pos_out, current_pos_row);
    const auto name_seg_round1{
        gen_name(par->ID, ref_round1, tar_round1, Format::segment)};

    // Round 2: old ref = new tar & old tar segments = new refs
    std::cerr << bold(
        underline("\nBuilding reference map for each target pattern\n"));

    std::string tar_round2 = par->tar = par->ref;
    for (uint64_t i = 0; i != num_seg_round1; ++i) {
      std::string ref_round2 = par->ref = name_seg_round1 + std::to_string(i);
      auto num_seg_round2 =
          run_round(par, 2, run_num, pos_out, current_pos_row);

      std::cerr << '\n';

      // Round 3
      std::cerr << bold(underline("\nRound 3\n"));
      const auto name_seg_round2{
          gen_name(par->ID, ref_round2, tar_round2, Format::segment)};
      par->tar = ref_round2;
      for (uint64_t j = 0; j != num_seg_round2; ++j) {
        par->ref = name_seg_round2 + std::to_string(j);
        auto num_seg_round3 =
            run_round(par, 3, run_num, pos_out, current_pos_row);
        std::cerr << "*************\n\n";

        remove_temp_seg(par, num_seg_round3);
      }  // Round 3

      par->ref = ref_round2;
      par->tar = tar_round2;
      // filter->aggregate_mid_pos(par->ID, ref_round2, tar_round2);

      remove_temp_seg(par, num_seg_round2);
    }  // Round 2

    par->ref = ref_round1;
    par->tar = tar_round1;
    // filter->aggregate_mid_pos(par->ID, ref_round1, tar_round1);

    remove_temp_seg(par, num_seg_round1);
    remove_temp_seq(par);
  }  // Round 1

  // filter->aggregate_final_pos(ref_round1, tar_round1);

  write_pos_file(pos_out);

  // // todo
  // for (auto row : pos_out) 
  //   row.show();
}

int main(int argc, char* argv[]) {
  try {
    const auto t0{now()};

    if (has(argv, argv + argc, std::string("-viz"))) {
      auto vizpar = std::make_unique<VizParam>();
      vizpar->parse(argc, argv);
      auto paint = std::make_unique<VizPaint>();
      paint->plot(vizpar);
    } else {
      auto par = std::make_unique<Param>();
      par->parse(argc, argv);
      run(par);
    }

    const auto t1{now()};
    std::cerr << "Total time: " << hms(t1 - t0);
  } catch (std::exception& e) {
    std::cerr << e.what();
  } catch (...) {
    return EXIT_FAILURE;
  }

  return 0;
}