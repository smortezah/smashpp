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


int main (int argc, char* argv[])
{
  try {
    const auto t0{now()};
    Param par;
    par.parse(argc, argv);              // Parse the command
    const auto tempRef=par.ref, tempTar=par.tar;
    auto models = make_shared<FCM>(par);// Equiv to auto* models = new FCM(par);
    models->store(par);                 // Build models
//    models->compress(par);              // Compress
//    auto filter = make_shared<Filter>(par);
//    filter->smooth_seg(par);            // Filter and segment
//    filter->extract_seg(par.tar, par.ref);  // Extract segs from the target file

//    cerr <<"\n===[ Building reference map for each target pattern ]=========\n";
//    // Consider the ref as new tar and the tar segments as new refs
//    const auto newTar  = par.ref;
//    const auto segName = gen_name(par.ref, par.tar, Format::SEGMENT);
//    par.tar = newTar;
//    const auto nSegs = filter->nSegs;
//    for (auto i=0; i!=nSegs; ++i) {
//      par.ref = segName+to_string(i);
//      par.thresh = 1.5;//todo
//      models = make_shared<FCM>(par);
//      models->store(par);
//      models->compress(par);
//      filter = make_shared<Filter>(par);
//      filter->smooth_seg(par);
//    }
//    for (auto i=0; i!=nSegs; ++i)
//      if (!par.saveAll && !par.saveSegment)
//        remove((segName+to_string(i)).c_str());
//
//    // Remove temporary sequences generated from Fasta/Fastq input files
//    if (par.refType==FileType::FASTA || par.refType==FileType::FASTQ) {
//      remove(tempRef.c_str());
//      rename((tempRef+LBL_BAK).c_str(), tempRef.c_str());
//    }
//    if (par.tarType==FileType::FASTA || par.tarType==FileType::FASTQ) {
//      remove(tempTar.c_str());
//      rename((tempTar+LBL_BAK).c_str(), tempTar.c_str());
//    }




    // Report
//    models->report(par); // Without "-R" does nothing

    const auto t1{now()};
    cerr << OUT_SEP << "Total time: " << hms(t1-t0);
  }
  catch (std::exception& e) { cout << e.what(); }
  catch (...) { return EXIT_FAILURE; }
  
  return 0;
}