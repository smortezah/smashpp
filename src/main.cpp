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
        filter->extract_seg(par.ref, par.tar);
      }
      else {
        const auto tempRef=par.ref, tempTar=par.tar;
        auto models = make_unique<FCM>(par);    // == auto* models=new FCM(par);
        models->store(par);                     // Build models
        models->compress(par);                  // Compress
        if (!par.manThresh)  par.thresh=static_cast<float>(models->aveEnt);
        auto filter = make_unique<Filter>(par);
        filter->smooth_seg(par);                // Filter and segment
//        filter->extract_seg(par.ref, par.tar);  // Extract segs from target

//        //todo ref-free compress all extracted regions
//        const auto segName = gen_name(par.ref, par.tar, Format::SEGMENT);
//        for (u64 i=0; i!=filter->nSegs; ++i) {
//          par.seq = segName+to_string(i);
//          models->self_compress(par);
//        }

//        cerr <<"\n===[ Building reference map for each target pattern ]=======";
//        // Consider the ref as new tar and the tar segments as new refs
//        const auto newTar  = par.ref;
//        const auto segName = gen_name(par.ref, par.tar, Format::SEGMENT);
//        par.tar = newTar;
//        for (u64 i=0; i!=filter->nSegs; ++i) {
//          par.ref = segName+to_string(i);
//          models = make_unique<FCM>(par);
//          models->store(par);
//          models->compress(par);
//          if (!par.manThresh)  par.thresh=static_cast<float>(models->aveEnt);
//          filter = make_unique<Filter>(par);
//          filter->smooth_seg(par);
//          //todo extract and ref-free compress all extracted regions
//        }
//        filter->aggregate_pos(tempRef, tempTar);
//
//        for (u64 i=0; i!=filter->nSegs; ++i)
//          if (!par.saveAll && !par.saveSegment)
//            remove((segName+to_string(i)).c_str());
//
//        // Remove temporary sequences generated from Fasta/Fastq input files
//        if (!par.saveSeq) {
//          if (par.refType==FileType::FASTA || par.refType==FileType::FASTQ) {
//            remove(tempRef.c_str());
//            rename((tempRef+LBL_BAK).c_str(), tempRef.c_str());
//          }
//          if (par.tarType==FileType::FASTA || par.tarType==FileType::FASTQ) {
//            remove(tempTar.c_str());
//            rename((tempTar+LBL_BAK).c_str(), tempTar.c_str());
//          }
//        }


//        // Report
//        models->report(par); // Without "-R" does nothing
      }
    }

    const auto t1{now()};
    cerr << OUT_SEP << "Total time: " << hms(t1-t0);
  }
  catch (std::exception& e) { cout << e.what(); }
  catch (...) { return EXIT_FAILURE; }
  
  return 0;
}