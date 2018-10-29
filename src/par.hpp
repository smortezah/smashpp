//
// Created by morteza on 02-02-2018.
//

#ifndef SMASHPP_PAR_HPP
#define SMASHPP_PAR_HPP

#include <fstream>
#include <vector>
#include <algorithm>
#include "def.hpp"
#include "fn.hpp"

namespace smashpp {
class Param    // Parameters
{
 public:
  string   tar, ref;
  u8       level /*:BIT_LEVEL*/;
  bool     verbose;
  u8       nthr  /*:BIT_THREAD*/;
  string   modelsPars;
  u32      wsize /*:BIT_WSIZE*/;
  string   wtype;
  u64      sampleStep;
  float    thresh;
  bool     saveSeq, saveProfile, saveFilter, saveSegment, saveAll;
  FileType refType, tarType;
  bool     showInfo;
  string   report;

  // Define Param::Param(){} in *.hpp => compile error
  Param () : level(DEF_LVL), verbose(false), nthr(DEF_THR), wsize(DEF_WS),
             wtype(DEF_WT), sampleStep(1ull), thresh(DEF_THRESH),
             saveSeq(false), saveProfile(false), saveFilter(false),
             saveSegment(false), saveAll(false), showInfo(true) {}
  void parse (int, char**&);
  string print_win_type () const;

 private:
  void help () const;
};

inline void Param::parse (int argc, char**& argv) {
  if (argc < 2) {
    help();  throw EXIT_SUCCESS;
  }
  else {
    vector<string> vArgs(static_cast<u64>(argc));
    for (int i=0; i!=argc; ++i)
      vArgs.emplace_back(static_cast<string>(argv[i]));

    for (auto i=vArgs.begin(); i!=vArgs.end(); ++i) {
      if (*i=="-h" || *i=="--help") {
        help();  throw EXIT_SUCCESS;
      }
      else if (*i=="-t" || *i=="--tar") {
        if (i+1 != vArgs.end()) { tar=*++i;  check_file(tar); }
        else error("target file not specified. Use \"-t fileName\".");
      }
      else if (*i=="-r" || *i=="--ref") {
        if (i+1 != vArgs.end()) { ref=*++i;  check_file(ref); }
        else error("reference file not specified. Use \"-r fileName\".");
      }
      else if ((*i=="-l" || *i=="--level") && i+1!=vArgs.end())
        level = static_cast<u8>(stoi(*++i));
      else if (*i=="-v"  || *i=="--verbose")
        verbose = true;
      else if ((*i=="-n" || *i=="--nthr") && i+1!=vArgs.end())
        nthr = static_cast<u8>(stoi(*++i));
      else if ((*i=="-m" || *i=="--models") && i+1!=vArgs.end()) {
        modelsPars = *++i;
        if (modelsPars[0]=='-' || modelsPars.empty())
          error("incorrect model parameters.");
      }
      else if ((*i=="-w" || *i=="--wsize") && i+1!=vArgs.end()) {
        const auto tmp = stoi(*++i);
        if (tmp <= 0)
          error("The window size must be greater than 0.");
        wsize = static_cast<u32>(tmp);
      }
      else if ((*i=="-wt" || *i=="--wtype") && i+1!=vArgs.end())
        wtype = *++i;
      else if ((*i=="-d" || *i=="--sample_step") && i+1!=vArgs.end()) {
        sampleStep = stoull(*++i);
        if (sampleStep==0)  sampleStep=1ull;
      }
      else if ((*i=="-th" || *i=="--thresh") && i+1!=vArgs.end())
        thresh = stof(*++i);
      else if (*i=="-sb"  || *i=="--save_seq")
        saveSeq = true;
      else if (*i=="-sp"  || *i=="--save_profile")
        saveProfile = true;
      else if (*i=="-sf"  || *i=="--save_fitler")
        saveFilter = true;
      else if (*i=="-ss"  || *i=="--save_segment")
        saveSegment = true;
      else if (*i=="-sa"  || *i=="--save_all")
        saveAll = true;
      else if (*i=="-R"  || *i=="--report")
        report = (i+1!=vArgs.end()) ? *++i : "report.txt";
    }

    // Mandatory args
    const bool has_t   {has(vArgs.begin(), vArgs.end(), "-t")};
    const bool has_tar {has(vArgs.begin(), vArgs.end(), "--tar")};
    const bool has_r   {has(vArgs.begin(), vArgs.end(), "-r")};
    const bool has_ref {has(vArgs.begin(), vArgs.end(), "--ref")};
    if (!has_t && !has_tar)
      error("target file not specified. Use \"-t fileName\".");
    else if (!has_r && !has_ref)
      error("reference file not specified. Use \"-r fileName\".");

    // Fasta/Fastq to bare Seq
    auto convert_to_seq = [&] (const string& s, const FileType& type) {
      rename(s.c_str(), (s+LBL_BAK).c_str());
      to_seq(s+LBL_BAK, s, type);
    };

    refType = file_type(ref);
    if      (refType==FileType::FASTA) convert_to_seq(ref, FileType::FASTA);
    else if (refType==FileType::FASTQ) convert_to_seq(ref, FileType::FASTQ);
    else if (refType!=FileType::SEQ)   error("\""+ref+"\" has unknown format.");

    tarType = file_type(tar);
    if      (tarType==FileType::FASTA) convert_to_seq(tar, FileType::FASTA);
    else if (tarType==FileType::FASTQ) convert_to_seq(tar, FileType::FASTQ);
    else if (tarType!=FileType::SEQ)   error("\""+tar+"\" has unknown format.");
  }
}

inline void Param::help () const {
  cerr <<                                                                   "\n"
    "NAME                                                                    \n"
    "    Smash++ v" << VERSION << " - rearrangements finder                  \n"
    "                                                                        \n"
    "AUTHOR                                                                  \n"
    "    Morteza Hosseini    seyedmorteza@ua.pt                              \n"
    "                                                                        \n"
    "SYNOPSIS                                                                \n"
    "    ./smashpp [OPTION]...  -r [REF_FILE] -t [TAR_FILE]                  \n"
    "                                                                        \n"
    "SAMPLE                                                                  \n"
//    "    ./smashpp -t TAR -r REF                                           \n"
    "                                                                        \n"
    "DESCRIPTION                                                             \n"
    "    Find rearrangements.                                                \n"
    "                                                                        \n"
    "    -r [FILE],  --ref [FILE]                                            \n"
    "        reference file. Can be bare Seq/Fasta/Fastq -> MANDATORY        \n"
    "                                                                        \n"
    "    -t [FILE],  --tar [FILE]                                            \n"
    "        target file. Can be bare Seq/Fasta/Fastq -> MANDATORY           \n"
    "                                                                        \n"
    "    -l [NUM],  --level [NUM]                                            \n"
    "        level                                                           \n"
    "                                                                        \n"
    "    -v,  --verbose                                                      \n"
    "        more information                                                \n"
    "                                                                        \n"
    "    -n [NUM],  --nthr [NUM]                                             \n"
    "        number of threads                                               \n"
    "                                                                        \n"
    "    -m [k,[w,d,]ir,alpha,gamma[,log2(w),d]/thresh,ir,alpha,gamma:...],  \n"
    "    --models [...]                                                      \n"
    "        parameters of models                                            \n"
    "        k:  context size                                                \n"
    "        w:  width of sketch in log2 format, e.g., set 10 for w=2^10=1024\n"
    "        d:  depth of sketch                                             \n"
    "        ir: inverted repeat                                             \n"
    "        thresh: threshold of substitutional tolerant Markov model       \n"
    "                                                                        \n"
    "    -w [NUM],  --wsize [NUM]                                            \n"
    "        window size -- for filtering                                    \n"
    "                                                                        \n"
    "    -wt [0 | 1 | 2 | 3 | 4 | 5 | 6 | 7], --wtype [...]                  \n"
    "    -wt [rectangular | hamming | hann | blackman | triangular           \n"
    "                     | welch   | sine | nuttall]                        \n"
    "        type of windowing function -- for filtering                     \n"
    "        0 | rectangular:  ractangular window                            \n"
    "        1 | hamming:      Hamming window                                \n"
    "        2 | hann:         Hann window                                   \n"
    "        3 | blackman:     Blackman window                               \n"
    "        4 | triangular:   triangular window                             \n"
    "        5 | welch:        Welch window                                  \n"
    "        6 | sine:         sine window                                   \n"
    "        7 | nuttall:      Nuttall window                                \n"
    "                                                                        \n"
    "    -d [NUM],  --sample_step [NUM]                                      \n"
    "        sampling steps -- for filtering                                 \n"
    "                                                                        \n"
    "    -th [NUM],  --thresh [NUM]                                          \n"
    "        threshold -- for filtering                                      \n"
    "                                                                        \n"
    "    -sb,  --save_seq                                                    \n"
    "        save bare sequence, in case the input file is Fasta/Fastq       \n"
    "                                                                        \n"
    "    -sp,  --save_profile                                                \n"
    "        save profile                                                    \n"
    "                                                                        \n"
    "    -sf,  --save_filter                                                 \n"
    "        save filtered file                                              \n"
    "                                                                        \n"
    "    -ss,  --save_segment                                                \n"
    "        save segmented file(s)                                          \n"
    "                                                                        \n"
    "    -sa,  --save_all                                                    \n"
    "        save profile, filetered and segmented files                     \n"
    "                                                                        \n"
    "    -R,  --report                                                       \n"
    "        save results in the \"report\" file                             \n"
    "                                                                        \n"
    "    -h,  --help                                                         \n"
    "        usage guide                                                     \n"
    "                                                                        \n"
    "COPYRIGHT                                                               \n"
    "    Copyright (C) "<< DEV_YEARS <<", IEETA, University of Aveiro.       \n"
    "    You may redistribute copies of this Free software                   \n"
    "    under the terms of the GNU - General Public License                 \n"
    "    v3 <http://www.gnu.org/licenses/gpl.html>. There                    \n"
    "    is NOT ANY WARRANTY, to the extent permitted by law.           "<<endl;
}

inline string Param::print_win_type () const {
  if      (wtype=="0" || wtype=="rectangular")    return "Rectangular";
  else if (wtype=="1" || wtype=="hamming")        return "Hamming";
  else if (wtype=="2" || wtype=="hann")           return "Hann";
  else if (wtype=="3" || wtype=="blackman")       return "Blackman";
  else if (wtype=="4" || wtype=="triangular")     return "Triangular";
  else if (wtype=="5" || wtype=="welch")          return "Welch";
  else if (wtype=="6" || wtype=="sine")           return "Sine";
  else if (wtype=="7" || wtype=="nuttall")        return "Nuttall";

  return "Rectangular";
}
}

#endif //SMASHPP_PAR_HPP