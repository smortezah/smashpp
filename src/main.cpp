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

int main(int argc, char* argv[]) {
  try {
    const auto t0{now()};

    if (has(argv, argv + argc, std::string("-viz"))) {
      auto vizpar = std::make_unique<VizParam>();
      vizpar->parse(argc, argv);
      auto paint = std::make_unique<VizPaint>();
      paint->plot(vizpar);
    } else {
      auto par = std::make_shared<Param>();
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
        std::string ref_round1 = par->ref;
        std::string tar_round1 = par->tar;

        for (uint8_t timesRunning = 0; timesRunning != 2; ++timesRunning) {
          if (timesRunning == 0)
            std::cerr << bold(
                "====[ REGULAR MODE ]==================================\n");
          else if (timesRunning == 1)
            std::cerr << bold(
                "====[ INVERTED MODE ]=================================\n");

          par->ID = timesRunning;

          // Round 1
          par->ref = ref_round1;
          par->tar = tar_round1;
          par->refName = file_name(par->ref);
          par->tarName = file_name(par->tar);

          auto models = std::make_unique<FCM>(par);
          // Make all IRs consistent
          for (auto& e : models->rMs) {
            e.ir = timesRunning;
            if (e.child) e.child->ir = timesRunning;
          }
          for (auto& e : models->tMs) {
            e.ir = timesRunning;
            if (e.child) e.child->ir = timesRunning;
          }

          // Build models and Compress
          models->store(par);
          models->compress(par);

          if (!par->manThresh)
            par->thresh = static_cast<float>(models->aveEnt);
          auto filter = std::make_unique<Filter>(par);
          filter->smooth_seg(par, 1);
          uint64_t seg_num_round1 = filter->nSegs;
          if (seg_num_round1 == 0) {
            std::cerr << '\n';
            continue;
          }
          filter->merge_extract_seg(par->ID, ref_round1, tar_round1);
          const auto seg_tar1_name{
              gen_name(par->ID, ref_round1, tar_round1, Format::segment)};

          // Ref-free compress
          if (!par->noRedun) {
            std::cerr << ". . . . . . . . . . . . . . . . . . . "
                         ". . . . . . . . . .\n>>> "
                      << italic("Reference-free compression of the segment")
                      << italic(seg_num_round1 == 1 ? "" : "s") << '\n';

            models->selfEnt.reserve(seg_num_round1);
            for (uint64_t i = 0; i != seg_num_round1; ++i) {
              par->seq = seg_tar1_name + std::to_string(i);
              models->self_compress(par, i);
            }
          }
          models->aggregate_slf(par);

          // Round 2: ref (of round 1) is new tar and tar segments (of round 1)
          // are new refs
          std::cerr << bold(
              underline("\nBuilding reference map for each target pattern\n"));
          
          std::string ref_round2, tar_round2;

          par->tar = tar_round2 = ref_round1;
          par->tarName = file_name(par->tar);
          const auto seg_ref2_num{seg_num_round1};

          for (uint64_t seg_ref2_idx = 0; seg_ref2_idx != seg_ref2_num;
               ++seg_ref2_idx) {
            par->ref = ref_round2 =
                seg_tar1_name + std::to_string(seg_ref2_idx);
            par->refName = file_name(par->ref);
            // Make all IRs consistent
            models = std::make_unique<FCM>(par);
            for (auto& e : models->rMs) {
              e.ir = timesRunning;
              if (e.child) e.child->ir = timesRunning;
            }
            for (auto& e : models->tMs) {
              e.ir = timesRunning;
              if (e.child) e.child->ir = timesRunning;
            }

            // Build models and Compress
            models->tarSegMsg = ref_round2 + "-segment-";
            models->tarSegID = seg_ref2_idx + 1;
            models->store(par);
            models->compress(par);

            // Filter and segment
            if (!par->manThresh)
              par->thresh = static_cast<float>(models->aveEnt);
            filter = std::make_unique<Filter>(par);
            filter->smooth_seg(par, 2);
            uint64_t seg_num_round2 = filter->nSegs;
            if (seg_num_round2 == 0) {
              std::cerr << '\n';
              continue;
            }
            filter->merge_extract_seg(par->ID, ref_round2, tar_round2);
            const auto seg_tar2_name{
                gen_name(par->ID, ref_round2, tar_round2, Format::segment)};

            // Ref-free compress
            if (!par->noRedun) {
              std::cerr << ". . . . . . . . . . . . . . . . . . . . . . . . . "
                           ". . . .\n>>> "
                        << italic("Reference-free compression of the segment")
                        << italic(seg_num_round2 > 1 ? "s" : "") << '\n';

              models->selfEnt.reserve(seg_num_round2);
              for (uint64_t j = 0; j != seg_num_round2; ++j) {
                par->seq = seg_tar2_name + std::to_string(j);
                models->self_compress(par, j);
                
                //todo remove
                  if (!par->saveAll && !par->saveSegment)
                    remove(par->seq.c_str());
              }
            }
            models->aggregate_slf(par);
            std::cerr << '\n';

            // // Round 3
            // std::cerr << bold(underline("\nRound 3\n"));

            // par3->tar = seg_tar1_name + std::to_string(seg_ref2_idx);
            // par3->tarName = file_name(par3->tar);

            // const auto seg_ref3_num{filter->nSegs};
            // for (uint64_t seg_ref3_idx = 0; seg_ref3_idx != seg_ref3_num;
            //      ++seg_ref3_idx) {

            //   par3->ref = seg_tar2_name + std::to_string(seg_ref3_idx);
            //   par3->refName = file_name(par3->ref);
            //   // Make all IRs consistent
            //   models = std::make_unique<FCM>(par3);
            //   for (auto& e : models->rMs) {
            //     e.ir = timesRunning;
            //     if (e.child) e.child->ir = timesRunning;
            //   }
            //   for (auto& e : models->tMs) {
            //     e.ir = timesRunning;
            //     if (e.child) e.child->ir = timesRunning;
            //   }

            //   // Build models and Compress
            //   models->tarSegMsg = par3->ref + "-segment-";
            //   models->tarSegID = seg_ref3_idx + 1;
            //   models->store(par3);
            //   models->compress(par3);

            //   // Filter and segment
            //   if (!par3->manThresh)
            //     par3->thresh = static_cast<float>(models->aveEnt);
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
            //               << italic("Reference-free compression of the segment")
            //               << italic(filter->nSegs > 1 ? "s" : "") << '\n';

            //     models->selfEnt.reserve(filter->nSegs);
            //     for (uint64_t j = 0; j != filter->nSegs; ++j) {
            //       par3->seq = seg_tar3_name + std::to_string(j);
            //       models->self_compress(par3, j);
            //       if (!par3->saveAll && !par3->saveSegment)
            //         remove(par3->seq.c_str());
            //     }
            //   }
            //   models->aggregate_slf(par3);
            //   std::cerr << '\n';
            // }
            // std::cerr<<"*************\n\n";
          }
          models->tarSegMsg.clear();
          filter->aggregate_mid_pos(par->ID, ref_round1, tar_round1);

          // Remove temporary files
          for (uint64_t seg_ref2_idx = 0; seg_ref2_idx != seg_ref2_num;
               ++seg_ref2_idx)
            if (!par->saveAll && !par->saveSegment)
              remove((seg_tar1_name + std::to_string(seg_ref2_idx)).c_str());
          if (!par->saveSeq) {
            if (par->refType == FileType::fasta ||
                par->refType == FileType::fastq) {
              remove(ref_round1.c_str());
              rename((ref_round1 + LBL_BAK).c_str(), ref_round1.c_str());
            }
            if (par->tarType == FileType::fasta ||
                par->tarType == FileType::fastq) {
              remove(tar_round1.c_str());
              rename((tar_round1 + LBL_BAK).c_str(), tar_round1.c_str());
            }
          }
        }  // for

        // Aggregate final positions
        auto filter = std::make_unique<Filter>(par);
        filter->aggregate_final_pos(ref_round1, tar_round1);
      } // else
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