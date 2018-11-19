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

//
// Created by morteza on 02-02-2018.
//

#define __STDC_FORMAT_MACROS
#if defined(_MSC_VER)
#  include <io.h>
#else
#  include <unistd.h>
#endif

#include <iostream>
#include <chrono>
#include <iomanip>      // setw, setprecision
#include "par.hpp"
#include "fcm.hpp"
#include "filter.hpp"
#include "fn.hpp"
#include "segment.hpp"
#include "vizpaint.hpp"
using namespace smashpp;


int main (int argc, char* argv[]) {
  try {
    const auto t0{now()};

    if (has(argv, argv+argc, string("-viz"))) {
      VizParam vizpar;
      vizpar.parse(argc, argv);
      auto paint = make_unique<VizPaint>();
      paint->print_plot(vizpar);
    }
    else {
      Param par;
      par.parse(argc, argv);                    // Parse the command

      if (par.compress) {
        auto models = make_unique<FCM>(par);
        models->store(par);
        models->compress(par);
      }
      else if (par.filter) {
        auto filter = make_unique<Filter>(par);
        filter->smooth_seg(par);
      }
      else if (par.segment) {
        auto filter = make_unique<Filter>(par);
        filter->smooth_seg(par);
        filter->extract_seg(par.ID, par.ref, par.tar);
      }
      else {
        const auto origRef=par.ref, origTar=par.tar;
        for (u8 timesRunning=0; timesRunning!=2; ++timesRunning) {
          if (timesRunning == 0) 
            cerr << bold("========[ REGULAR MODE ]========\n");
          else if (timesRunning == 1)
            cerr << bold("========[ INVERTED MODE ]========\n");
          par.ID = timesRunning;    par.ref=origRef;    par.tar=origTar;
          auto models = make_unique<FCM>(par);  // == auto* models=new FCM(par);
          for (auto& e : models->rMs)   e.ir=timesRunning;
          for (auto& e : models->rTMs)  e.ir=timesRunning;
          for (auto& e : models->tMs)   e.ir=timesRunning;
          for (auto& e : models->tTMs)  e.ir=timesRunning;
          models->store(par);                   // Build models
          models->compress(par);                // Compress
          if (!par.manThresh)  par.thresh=static_cast<float>(models->aveEnt);
          auto filter = make_unique<Filter>(par);
          filter->smooth_seg(par);              // Filter and segment
          filter->extract_seg(par.ID, par.ref, par.tar);  // Extract from tar
          cerr << TERM_SEP;
          // Ref-free compress
          models->selfEnt.reserve(filter->nSegs);
          const auto segName = 
            gen_name(par.ID, par.ref, par.tar, Format::SEGMENT);
          for (u64 i=0; i!=filter->nSegs; ++i) {
            par.seq = segName+to_string(i);
            models->self_compress(par, i);
          }
          models->aggregate_slf(par);
          
          // Consider the ref as new tar and segments of the tar as new refs
          cerr << "\n" 
               << underline("Building reference map for each target pattern")
               << "\n";
          const auto newTar = par.ref;
          par.tar = newTar;
          const auto tarSegs = filter->nSegs;
          for (u64 i=0; i!=tarSegs; ++i) {
            par.ref = segName+to_string(i);
            models = make_unique<FCM>(par);
            models->store(par);
            models->compress(par);
            if (!par.manThresh)  par.thresh=static_cast<float>(models->aveEnt);
            filter = make_unique<Filter>(par);
            filter->smooth_seg(par);
            filter->extract_seg(par.ID, par.ref, par.tar);
            cerr << TERM_SEP;
            // Ref-free compress
            models->selfEnt.reserve(filter->nSegs);
            const auto selfSegName = 
              gen_name(par.ID, par.ref, par.tar, Format::SEGMENT);
            for (u64 j=0; j!=filter->nSegs; ++j) {
              par.seq = selfSegName+to_string(j);
              models->self_compress(par, j);
              if (!par.saveAll && !par.saveSegment)
                remove(par.seq.c_str());
            }
            models->aggregate_slf(par);
            cerr << '\n';
          }
          filter->aggregate_mid_pos(par.ID, origRef, origTar);

          for (u64 i=0; i!=tarSegs; ++i)
            if (!par.saveAll && !par.saveSegment)
              remove((segName+to_string(i)).c_str());
          // Remove temporary sequences generated from Fasta/Fastq input files
          if (!par.saveSeq) {
            if (par.refType==FileType::FASTA || par.refType==FileType::FASTQ) {
              remove(origRef.c_str());
              rename((origRef+LBL_BAK).c_str(), origRef.c_str());
            }
            if (par.tarType==FileType::FASTA || par.tarType==FileType::FASTQ) {
              remove(origTar.c_str());
              rename((origTar+LBL_BAK).c_str(), origTar.c_str());
            }
          }
        }

        auto filter = make_unique<Filter>(par);
        filter->aggregate_final_pos(origRef, origTar);
        

// //        // Report
// //        models->report(par); // Without "-R" does nothing
      }
    }



    const auto t1{now()};
    cerr << "Total time: " << hms(t1-t0);
    // cerr << OUT_SEP << "Total time: " << hms(t1-t0);
  }
  catch (std::exception& e) { cout << e.what(); }
  catch (...) { return EXIT_FAILURE; }
  
  return 0;
}