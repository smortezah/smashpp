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
#include "exception.hpp"
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
      auto par1 = std::make_unique<Param>();
      par1->parse(argc, argv);  // Parse the command

      if (par1->compress) {
        auto models = std::make_unique<FCM>(par1);
        models->store(par1);
        models->compress(par1);
      } else if (par1->filter) {
        auto filter = std::make_unique<Filter>(par1);
        filter->smooth_seg(par1);
      } else if (par1->segment) {
        auto filter = std::make_unique<Filter>(par1);
        filter->smooth_seg(par1);
        filter->merge_extract_seg(par1->ID, par1->ref, par1->tar);
      } else {
        // unique_ptr cannot be copied
        auto par2 = std::make_unique<Param>();  // For round 2
        par2->parse(argc, argv);
        auto par3 = std::make_unique<Param>();  // For round 3
        par3->parse(argc, argv);

auto mori=2;//todo
        for (uint8_t timesRunning = 0; timesRunning != mori; ++timesRunning) {
          if (timesRunning == 0)
            std::cerr << bold(
                "====[ REGULAR MODE ]==================================\n");
          else if (timesRunning == 1)
            std::cerr << bold(
                "====[ INVERTED MODE ]=================================\n");

          par3->ID = par2->ID = par1->ID = timesRunning;

          // Round 1
          par1->refName = file_name(par1->ref);
          par1->tarName = file_name(par1->tar);

          auto models = std::make_unique<FCM>(par1);
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
          models->store(par1);
          models->compress(par1);

          // Filter and segment
          if (!par1->manThresh)
            par1->thresh = static_cast<float>(models->aveEnt);
          auto filter = std::make_unique<Filter>(par1);
          filter->smooth_seg(par1);
          if (filter->nSegs == 0) {
            std::cerr << '\n';
            continue;
          }
          filter->merge_extract_seg(par1->ID, par1->ref, par1->tar);
          const auto seg_tar1_name{
              gen_name(par1->ID, par1->ref, par1->tar, Format::segment)};

          // Ref-free compress
          if (!par1->noRedun) {
            std::cerr << ". . . . . . . . . . . . . . . . . . . "
                         ". . . . . . . . . .\n>>> "
                      << italic("Reference-free compression of the segment")
                      << italic(filter->nSegs == 1 ? "" : "s") << '\n';

            models->selfEnt.reserve(filter->nSegs);
            for (uint64_t i = 0; i != filter->nSegs; ++i) {
              par1->seq = seg_tar1_name + std::to_string(i);
              models->self_compress(par1, i);
            }
          }
          models->aggregate_slf(par1);

          // Round 2: ref (of round 1) is new tar and tar segments (of round 1)
          // are new refs
          std::cerr << bold(
              underline("\nBuilding reference map for each target pattern\n"));

          par2->tar = par1->ref;
          par2->tarName = file_name(par2->tar);
          const auto seg_ref2_num{filter->nSegs};

          for (uint64_t seg_ref2_idx = 0; seg_ref2_idx != seg_ref2_num;
               ++seg_ref2_idx) {
            par2->ref = seg_tar1_name + std::to_string(seg_ref2_idx);
            par2->refName = file_name(par2->ref);
            // Make all IRs consistent
            models = std::make_unique<FCM>(par2);
            for (auto& e : models->rMs) {
              e.ir = timesRunning;
              if (e.child) e.child->ir = timesRunning;
            }
            for (auto& e : models->tMs) {
              e.ir = timesRunning;
              if (e.child) e.child->ir = timesRunning;
            }

            // Build models and Compress
            models->tarSegMsg = par2->ref + "-segment-";
            models->tarSegID = seg_ref2_idx + 1;
            models->store(par2);
            models->compress(par2);

            // Filter and segment
            if (!par2->manThresh)
              par2->thresh = static_cast<float>(models->aveEnt);
            filter = std::make_unique<Filter>(par2);
            filter->smooth_seg(par2);
            if (filter->nSegs == 0) {
              std::cerr << '\n';
              continue;
            }
            filter->merge_extract_seg(par2->ID, par2->ref, par2->tar);
            const auto seg_tar2_name{
                gen_name(par2->ID, par2->ref, par2->tar, Format::segment)};

            // Ref-free compress
            if (!par2->noRedun) {
              std::cerr << ". . . . . . . . . . . . . . . . . . . "
                           ". . . . . . . . . .\n>>> "
                        << italic("Reference-free compression of the segment")
                        << italic(filter->nSegs > 1 ? "s" : "") << '\n';

              models->selfEnt.reserve(filter->nSegs);
              for (uint64_t j = 0; j != filter->nSegs; ++j) {
                par2->seq = seg_tar2_name + std::to_string(j);
                models->self_compress(par2, j);
              }
            }
            models->aggregate_slf(par2);
            std::cerr << '\n';

            // Round 3
            std::cerr << bold(underline("\nRound 3\n"));

            par3->tar = seg_tar1_name + std::to_string(seg_ref2_idx);
            par3->tarName = file_name(par3->tar);

            const auto seg_ref3_num{filter->nSegs};
            for (uint64_t seg_ref3_idx = 0; seg_ref3_idx != seg_ref3_num;
                 ++seg_ref3_idx) {

              par3->ref = seg_tar2_name + std::to_string(seg_ref3_idx);
              par3->refName = file_name(par3->ref);
              // Make all IRs consistent
              models = std::make_unique<FCM>(par3);
              for (auto& e : models->rMs) {
                e.ir = timesRunning;
                if (e.child) e.child->ir = timesRunning;
              }
              for (auto& e : models->tMs) {
                e.ir = timesRunning;
                if (e.child) e.child->ir = timesRunning;
              }

              // Build models and Compress
              models->tarSegMsg = par3->ref + "-segment-";
              models->tarSegID = seg_ref3_idx + 1;
              models->store(par3);
              models->compress(par3);

              // Filter and segment
              if (!par3->manThresh)
                par3->thresh = static_cast<float>(models->aveEnt);
              filter = std::make_unique<Filter>(par3);
              filter->smooth_seg(par3);
              if (filter->nSegs == 0) {
                std::cerr << '\n';
                continue;
              }
              filter->merge_extract_seg(par3->ID, par3->ref, par3->tar);

              const auto seg_tar3_name{
                  gen_name(par3->ID, par3->ref, par3->tar, Format::segment)};

              // Ref-free compress
              if (!par3->noRedun) {
                std::cerr << ". . . . . . . . . . . . . . . . . . . "
                             ". . . . . . . . . .\n>>> "
                          << italic("Reference-free compression of the segment")
                          << italic(filter->nSegs > 1 ? "s" : "") << '\n';

                models->selfEnt.reserve(filter->nSegs);
                for (uint64_t j = 0; j != filter->nSegs; ++j) {
                  par3->seq = seg_tar3_name + std::to_string(j);
                  models->self_compress(par3, j);
                  if (!par3->saveAll && !par3->saveSegment)
                    remove(par3->seq.c_str());
                }
              }
              models->aggregate_slf(par3);
              std::cerr << '\n';
            }
            std::cerr<<"*************\n\n";
          }
          models->tarSegMsg.clear();
          filter->aggregate_mid_pos(par1->ID, par1->ref, par1->tar);

          // Remove temporary files
          for (uint64_t seg_ref2_idx = 0; seg_ref2_idx != seg_ref2_num;
               ++seg_ref2_idx)
            if (!par2->saveAll && !par2->saveSegment)
              remove((seg_tar1_name + std::to_string(seg_ref2_idx)).c_str());
          if (!par2->saveSeq) {
            if (par2->refType == FileType::fasta ||
                par2->refType == FileType::fastq) {
              remove(par1->ref.c_str());
              rename((par1->ref + LBL_BAK).c_str(), par1->ref.c_str());
            }
            if (par1->tarType == FileType::fasta ||
                par1->tarType == FileType::fastq) {
              remove(par1->tar.c_str());
              rename((par1->tar + LBL_BAK).c_str(), par1->tar.c_str());
            }
          }
        }  // for

        // Aggregate final positions
        auto filter = std::make_unique<Filter>(par1);
        filter->aggregate_final_pos(par1->ref, par1->tar);
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