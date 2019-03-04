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

#include <chrono>
#include <iomanip>  // setw, setprecision
#include <iostream>
#include "container.hpp"
#include "exception.hpp"
#include "fcm.hpp"
#include "filter.hpp"
#include "naming.hpp"
#include "par.hpp"
#include "segment.hpp"
#include "time.hpp"
#include "vizpaint.hpp"
using namespace smashpp;

int main(int argc, char* argv[]) {
  try {
    const auto t0{now()};

    if (has(argv, argv + argc, std::string("-viz"))) {
      VizParam vizpar;
      vizpar.parse(argc, argv);
      auto paint = std::make_unique<VizPaint>();
      paint->plot(vizpar);
    } else {
      Param par;
      par.parse(argc, argv);  // Parse the command

      if (par.compress) {
        auto models = std::make_unique<FCM>(par);
        models->store(par);
        models->compress(par);
      } else if (par.filter) {
        auto filter = std::make_unique<Filter>(par);
        filter->smooth_seg(par);
      } else if (par.segment) {
        auto filter = std::make_unique<Filter>(par);
        filter->smooth_seg(par);
        filter->merge_extract_seg(par.ID, par.ref, par.tar);
      } else {
        const auto origRef{par.ref};
        const auto origTar{par.tar};
        for (uint8_t timesRunning = 0; timesRunning != 2; ++timesRunning) {
          if (timesRunning == 0)
            std::cerr << bold(
                "====[ REGULAR MODE ]==================================\n");
          else if (timesRunning == 1)
            std::cerr << bold(
                "====[ INVERTED MODE ]=================================\n");

          par.ID = timesRunning;
          par.ref = origRef;
          par.refName = file_name(par.ref);
          par.tar = origTar;
          par.tarName = file_name(par.tar);

          auto models =
              std::make_unique<FCM>(par);  // auto* models=new FCM(par);
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

          // Filter and segment
          if (!par.manThresh) par.thresh = static_cast<float>(models->aveEnt);
          auto filter = std::make_unique<Filter>(par);
          filter->smooth_seg(par);
          if (filter->nSegs == 0) {
            std::cerr << '\n';
            continue;
          }
          filter->merge_extract_seg(par.ID, par.ref, par.tar);
          const auto segName{
              gen_name(par.ID, par.ref, par.tar, Format::SEGMENT)};

          // Ref-free compress
          if (!par.noRedun) {
            std::cerr << TERM_SEP << ">>> "
                      << italic("Reference-free compression of the segment")
                      << italic(filter->nSegs == 1 ? "" : "s") << '\n';
            models->selfEnt.reserve(filter->nSegs);
            for (uint64_t i = 0; i != filter->nSegs; ++i) {
              par.seq = segName + std::to_string(i);
              models->self_compress(par, i);
            }
          }
          models->aggregate_slf(par);

          // Consider the ref as new tar and segments of the tar as new refs
          std::cerr << bold(
              underline("\nBuilding reference map for each "
                        "target pattern\n"));
          const auto newTar{par.ref};
          par.tar = newTar;
          par.tarName = file_name(par.tar);
          const auto tarSegs{filter->nSegs};
          for (uint64_t i = 0; i != tarSegs; ++i) {
            par.ref = segName + std::to_string(i);
            par.refName = file_name(par.ref);
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
            models->tarSegMsg = origTar + "-segment-";
            models->tarSegID = i + 1;
            models->store(par);
            models->compress(par);

            // Filter and segment
            if (!par.manThresh) par.thresh = static_cast<float>(models->aveEnt);
            filter = std::make_unique<Filter>(par);
            filter->smooth_seg(par);
            if (filter->nSegs == 0) {
              std::cerr << '\n';
              continue;
            }

            // Ref-free compress
            if (!par.noRedun) {
              filter->merge_extract_seg(par.ID, par.ref, par.tar);
              std::cerr << TERM_SEP;
              std::cerr << ">>> "
                        << italic(
                               "Reference-free compression of the "
                               "segment")
                        << italic(filter->nSegs == 1 ? "" : "s") << '\n';
              models->selfEnt.reserve(filter->nSegs);
              const auto selfSegName{
                  gen_name(par.ID, par.ref, par.tar, Format::SEGMENT)};
              for (uint64_t j = 0; j != filter->nSegs; ++j) {
                par.seq = selfSegName + std::to_string(j);
                models->self_compress(par, j);
                if (!par.saveAll && !par.saveSegment) remove(par.seq.c_str());
              }
            }
            models->aggregate_slf(par);
            std::cerr << '\n';
          }
          models->tarSegMsg.clear();
          filter->aggregate_mid_pos(par.ID, origRef, origTar);

          // Remove temporary files
          for (uint64_t i = 0; i != tarSegs; ++i)
            if (!par.saveAll && !par.saveSegment)
              remove((segName + std::to_string(i)).c_str());
          if (!par.saveSeq) {
            if (par.refType == FileType::FASTA ||
                par.refType == FileType::FASTQ) {
              remove(origRef.c_str());
              rename((origRef + LBL_BAK).c_str(), origRef.c_str());
            }
            if (par.tarType == FileType::FASTA ||
                par.tarType == FileType::FASTQ) {
              remove(origTar.c_str());
              rename((origTar + LBL_BAK).c_str(), origTar.c_str());
            }
          }
        }  // for

        // Aggregate final positions
        par.ref = origRef;
        par.refName = file_name(par.ref);
        par.tar = origTar;
        par.tarName = file_name(par.tar);
        auto filter = std::make_unique<Filter>(par);
        filter->aggregate_final_pos(origRef, origTar);

        //        // Report
        //        models->report(par); // Without "-R" does nothing
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