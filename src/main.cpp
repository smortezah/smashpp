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

#define __STDC_FORMAT_MACROS
#if defined(_MSC_VER)
#include <io.h>
#else
#include <unistd.h>
#endif

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

uint64_t run_round(std::unique_ptr<Param>& par, uint8_t round,
                   uint8_t run_num) {
  par->ID = run_num;
  par->refName = file_name(par->ref);
  par->tarName = file_name(par->tar);

  auto models = std::make_unique<FCM>(par);
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
  if (!par->manThresh) par->thresh = static_cast<float>(models->aveEnt);
  auto filter = std::make_unique<Filter>(par);
  filter->smooth_seg(par, round);
  if (filter->nSegs == 0) {
    std::cerr << '\n';
    return 0;  // continue;
  }
  filter->merge_extract_seg(par->ID, par->ref, par->tar);
  const auto seg_name{gen_name(par->ID, par->ref, par->tar, Format::segment)};

  // Ref-free compress
  if (!par->noRedun) {
    std::cerr << ". . . . . . . . . . . . . . . . . . . "
                 ". . . . . . . . . .\n>>> "
              << italic("Reference-free compression of the segment")
              << italic(filter->nSegs == 1 ? "" : "s") << '\n';

    models->selfEnt.reserve(filter->nSegs);
    for (uint64_t i = 0; i != filter->nSegs; ++i) {
      par->seq = seg_name + std::to_string(i);
      models->self_compress(par, i);
    }
  }
  models->aggregate_slf(par);

  return filter->nSegs;
}

void remove_temp_seg(std::unique_ptr<Param>& par, uint64_t seg_num) {
  const auto seg{gen_name(par->ID, par->ref, par->tar, Format::segment)};

  for (uint64_t i = 0; i != seg_num; ++i)
    if (!par->saveAll && !par->saveSegment)
      remove((seg + std::to_string(i)).c_str());
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

  for (uint8_t run_num = 0; run_num != 2; ++run_num) {
    if (run_num == 0)
      std::cerr << bold(
          "====[ REGULAR MODE ]==================================\n");
    else if (run_num == 1)
      std::cerr << bold(
          "====[ INVERTED MODE ]=================================\n");

    auto num_seg_round1 = run_round(par, 1, run_num);
    const auto name_seg_round1{
        gen_name(par->ID, ref_round1, tar_round1, Format::segment)};

    // Round 2: old ref = new tar & old tar segments = new refs
    std::cerr << bold(
        underline("\nBuilding reference map for each target pattern\n"));

    par->tar = par->ref;
    for (uint64_t i = 0; i != num_seg_round1; ++i) {
      par->ref = name_seg_round1 + std::to_string(i);
      auto num_seg_round2 = run_round(par, 2, run_num);
      remove_temp_seg(par, num_seg_round2);
      std::cerr << '\n';

      // // Round 3
      // std::cerr << bold(underline("\nRound 3\n"));

      // par3->tar = name_seg_round1 + std::to_string(i);
      // par3->tarName = file_name(par3->tar);

      // const auto seg_ref3_num{filter->nSegs};
      // for (uint64_t seg_ref3_idx = 0; seg_ref3_idx != seg_ref3_num;
      //      ++seg_ref3_idx) {
      //   par3->ref = seg_tar2_name + std::to_string(seg_ref3_idx);
      //   par3->refName = file_name(par3->ref);
      //   // Make all IRs consistent
      //   models = std::make_unique<FCM>(par3);
      //   for (auto& e : models->rMs) {
      //     e.ir = run_num;
      //     if (e.child) e.child->ir = run_num;
      //   }
      //   for (auto& e : models->tMs) {
      //     e.ir = run_num;
      //     if (e.child) e.child->ir = run_num;
      //   }

      //   // Build models and Compress
      //   models->tarSegMsg = par3->ref + "-segment-";
      //   models->tarSegID = seg_ref3_idx + 1;
      //   models->store(par3);
      //   models->compress(par3);

      //   // Filter and segment
      //   if (!par3->manThresh) par3->thresh = static_cast<float>(models->aveEnt);
      //   filter = std::make_unique<Filter>(par3);
      //   filter->smooth_seg(par3, 3);
      //   if (filter->nSegs == 0) {
      //     std::cerr << '\n';
      //     continue;
      //   }
      //   filter->merge_extract_seg(par3->ID, par3->ref, par3->tar);

      //   const auto seg_tar3_name{
      //       gen_name(par3->ID, par3->ref, par3->tar, Format::segment)};

      //   // Ref-free compress
      //   if (!par3->noRedun) {
      //     std::cerr << ". . . . . . . . . . . . . . . . . . . "
      //                  ". . . . . . . . . .\n>>> "
      //               << italic("Reference-free compression of the
      //               segment")
      //               << italic(filter->nSegs > 1 ? "s" : "") << '\n';

      //     models->selfEnt.reserve(filter->nSegs);
      //     for (uint64_t j = 0; j != filter->nSegs; ++j) {
      //       par3->seq = seg_tar3_name + std::to_string(j);
      //       models->self_compress(par3, j);
      //       if (!par3->saveAll && !par3->saveSegment) remove(par3->seq.c_str());
      //     }
      //   }
      //   models->aggregate_slf(par3);
      //   std::cerr << '\n';
      // }
      // std::cerr << "*************\n\n";
    }

    par->ref = ref_round1;
    par->tar = tar_round1;
    filter->aggregate_mid_pos(par->ID, ref_round1, tar_round1);

    remove_temp_seg(par, num_seg_round1);
    remove_temp_seq(par);
  }  // for

  filter->aggregate_final_pos(ref_round1, tar_round1);
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
      par->parse(argc, argv);  // Parse the command

      if (par->compress) {
        auto models = std::make_unique<FCM>(par);
        models->store(par);
        models->compress(par);
      } else if (par->filter) {
        auto filter = std::make_unique<Filter>(par);
        filter->smooth_seg(par, 1);
      } else if (par->segment) {
        auto filter = std::make_unique<Filter>(par);
        filter->smooth_seg(par, 1);
        filter->merge_extract_seg(par->ID, par->ref, par->tar);
      } else {
        run(par);
      }
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