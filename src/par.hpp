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
#include "vizdef.hpp"

namespace smashpp {
class Param {   // Parameters
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
  bool     compress, filter, segment;

  // Define Param::Param(){} in *.hpp => compile error
  Param () : level(DEF_LVL), verbose(false), nthr(DEF_THR), wsize(DEF_WS),
             wtype(DEF_WT), sampleStep(1ull), thresh(DEF_THRESH),
             saveSeq(false), saveProfile(false), saveFilter(false),
             saveSegment(false), saveAll(false), refType(FileType::SEQ),
             tarType(FileType::SEQ), showInfo(true), compress(false),
             filter(false), segment(false) {}

  void parse (int, char**&);
  string print_win_type () const;

 private:
  void help () const;
};

class VizParam {
 public:
  bool   verbose, inverse, regular, showPos, showNRC, showRedun;
  string image;
  u32    link, width, space, mult, start, min;
  string posFile;

  VizParam () : verbose(false), inverse(true), regular(true), showPos(false),
                showNRC(false), showRedun(false), image(DEF_IMAGE),
                link(DEF_LINK), width(DEF_WIDT), space(DEF_SPAC),
                mult(DEF_MULT), start(DEF_BEGI), min(DEF_MINP) {}

  void parse (int, char**&);

 private:
  void help () const;
};

inline void Param::parse (int argc, char**& argv) {
  if (argc < 2) { help();  throw EXIT_SUCCESS; }

  vector<string> vArgs(static_cast<u64>(argc));
  for (int i=0; i!=argc; ++i)
    vArgs.emplace_back(static_cast<string>(argv[i]));

  for (auto i=vArgs.begin(); i!=vArgs.end(); ++i) {
    if      (*i=="-h"  || *i=="--help") { help();  throw EXIT_SUCCESS; }
    else if (*i=="-v"  || *i=="--verbose")         verbose=true;
    else if (*i=="-compress")                      compress=true;
    else if (*i=="-filter")                        filter=true;
    else if (*i=="-segment")                       segment=true;
    else if (*i=="-sb" || *i=="--save_seq")        saveSeq=true;
    else if (*i=="-sp" || *i=="--save_profile")    saveProfile=true;
    else if (*i=="-sf" || *i=="--save_fitler")     saveFilter=true;
    else if (*i=="-ss" || *i=="--save_segment")    saveSegment=true;
    else if (*i=="-sa" || *i=="--save_all")        saveAll=true;
    else if (*i=="-t"  || *i=="--tar") {
      if (i+1 != vArgs.end()) { tar=*++i;  check_file(tar); }
      else error("target file not specified. Use \"-t fileName\".");
    }
    else if (*i=="-r"  || *i=="--ref") {
      if (i+1 != vArgs.end()) { ref=*++i;  check_file(ref); }
      else error("reference file not specified. Use \"-r fileName\".");
    }
    else if ((*i=="-l" || *i=="--level") && i+1!=vArgs.end()) {
      level = static_cast<u8>(stoi(*++i));
      def_if_not_in_range("Level", level, MIN_LVL, MAX_LVL, DEF_LVL);
      level -= 1;
    }
    else if ((*i=="-n" || *i=="--nthr") && i+1!=vArgs.end()) {
      nthr = static_cast<u8>(stoi(*++i));
      def_if_not_in_range("Number of threads", nthr, MIN_THR,MAX_THR,DEF_THR);
    }
    else if ((*i=="-m" || *i=="--model") && i+1!=vArgs.end()) {
      modelsPars = *++i;
      if (modelsPars[0]=='-' || modelsPars.empty())
        error("incorrect model parameters.");
    }
    else if ((*i=="-w" || *i=="--wsize") && i+1!=vArgs.end()) {
      wsize = static_cast<u32>(stoi(*++i));
      def_if_not_in_range("Window size", wsize, MIN_WS, MAX_WS, DEF_WS);
    }
    else if ((*i=="-wt"|| *i=="--wtype") && i+1!=vArgs.end()) {
      wtype = *++i;
      def_if_not_in_range("Window type", wtype, MIN_WT, MAX_WT, DEF_WT);
    }
    else if ((*i=="-d" || *i=="--step") && i+1!=vArgs.end()) {
      sampleStep = stoull(*++i);
      if (sampleStep==0)  sampleStep=1ull;
    }
    else if ((*i=="-th"|| *i=="--thresh") && i+1!=vArgs.end())
      thresh = stof(*++i);
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

  if (!has(vArgs.begin(), vArgs.end(), "-m") &&
      !has(vArgs.begin(), vArgs.end(), "--model"))
    modelsPars = LEVEL[level];

  keep_in_range(wsize, 0ull, min(file_size(ref),file_size(tar)));

  // Fasta/Fastq to Seq
  auto convert_to_seq = [&] (const string& s, const FileType& type) {
    rename(s.c_str(), (s+LBL_BAK).c_str());
    to_seq(s+LBL_BAK, s, type);
  };
  refType = file_type(ref);
  if      (refType==FileType::FASTA)  convert_to_seq(ref, FileType::FASTA);
  else if (refType==FileType::FASTQ)  convert_to_seq(ref, FileType::FASTQ);
  else if (refType!=FileType::SEQ)    error("\""+ref+"\" has unknown format.");

  tarType = file_type(tar);
  if      (tarType==FileType::FASTA)  convert_to_seq(tar, FileType::FASTA);
  else if (tarType==FileType::FASTQ)  convert_to_seq(tar, FileType::FASTQ);
  else if (tarType!=FileType::SEQ)    error("\""+tar+"\" has unknown format.");
}

inline void Param::help () const {
  cerr <<
    "NAME                                                                    \n"
    "  Smash++ v"<<VERSION<< " - rearrangements finder                       \n"
    "                                                                        \n"
    "AUTHOR                                                                  \n"
    "  Morteza Hosseini    seyedmorteza@ua.pt                                \n"
    "                                                                        \n"
    "SYNOPSIS                                                                \n"
    "  ./smashpp [OPTION]...  -r [REF_FILE] -t [TAR_FILE]                    \n"
    "                                                                        \n"
    "SAMPLE                                                                  \n"
//    "  ./smashpp -t TAR -r REF                                             \n"
    "                                                                        \n"
    "DESCRIPTION                                                             \n"
    "  Mandatory arguments:                                                  \n"
    "  -r,  --ref [FILE]       reference file (Seq/Fasta/Fastq)              \n"
    "  -t,  --tar [FILE]       target file    (Seq/Fasta/Fastq)              \n"
    "                                                                        \n"
    "  Options:                                                              \n"
    "  -v,  --verbose         more information                               \n"
    "  -l,  --level [NUM]     levels of compression [1;5]                    \n"
    "  -n,  --nthr  [NUM]     number of threads                              \n"
    "  -w,  --wsize [NUM]     window size                <-- filter          \n"
    "  -wt, --wtype [...]     type of windowing function <-- filter          \n"
    "                         [0 | rectangular] [1 | hamming]                \n"
    "                         [2 | hann]        [3 | blackman]               \n"
    "                         [4 | triangular]  [5 | welch]                  \n"
    "                         [6 | sine]        [7 | nuttall]                \n"
    "  -d,  --step   [NUM]    sampling steps             <-- filter          \n"
    "  -th, --thresh [NUM]    threshold                  <-- filter          \n"
    "  -sb, --save_seq        save sequence (input is Fasta/Fastq)           \n"
    "  -sp, --save_profile    save profile                                   \n"
    "  -sf, --save_filter     save filtered file                             \n"
    "  -ss, --save_segment    save segmented file(s)                         \n"
    "  -sa, --save_all        save profile, filetered and                    \n"
    "                           segmented files                              \n"
    "  -R,  --report          save results in the \"report\" file            \n"
    "  -h,  --help            usage guide                                    \n"
    "  -m,  --model [\U0001D705,[\U0001D464,\U0001D451,]ir,\U0001D6FC,"
                         "\U0001D6FE/\U0001D70F,ir,\U0001D6FC,\U0001D6FE:...]\n"
    "                         parameters of models                           \n"
    "                         \U0001D705:   context size                     \n"
    "                         \U0001D464:   width of sketch in log2 form,    \n"
    "                                e.g., set 10 for w=2^10=1024            \n"
    "                         \U0001D451:   depth of sketch                  \n"
    "                         ir:  inverted repeat (0 or 1)                  \n"
    "                         \U0001D6FC:   estimator                        \n"
    "                         \U0001D6FE:   forgetting factor [0;1)          \n"
    "                         \U0001D70F:   threshold (# substitutions)      \n"
    "                                                                        \n"
    "COPYRIGHT                                                               \n"
    "  Copyright (C) "<< DEV_YEARS <<", IEETA, University of Aveiro.         \n"
    "  You may redistribute copies of this Free software                     \n"
    "  under the terms of the GNU - General Public License                   \n"
    "  v3 <http://www.gnu.org/licenses/gpl.html>. There is                   \n"
    "  NOT ANY WARRANTY, to the extent permitted by law.                "<<endl;
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

inline void VizParam::parse (int argc, char**& argv) {
  if (argc < 3) { help();  throw EXIT_SUCCESS; }

  vector<string> vArgs(static_cast<u64>(argc));
  for (int i=0; i!=argc; ++i)
    vArgs.emplace_back(static_cast<string>(argv[i]));

  for (auto i=vArgs.begin(); i!=vArgs.end(); ++i) {
    if      (*i=="-h"  || *i=="--help") { help();   throw EXIT_SUCCESS; }
    else if (*i=="-v"  || *i=="--verbose")          verbose=true;
    else if (*i=="-sp" || *i=="--show_pos")         showPos=true;
    else if (*i=="-sn" || *i=="--show_nrc")         showNRC=true;
    else if (*i=="-sr" || *i=="--show_redun")       showRedun=true;
    else if (*i=="-i"  || *i=="--dont_show_inv")    inverse=false;
    else if (*i=="-r"  || *i=="--dont_show_reg")    regular=false;
    else if ((*i=="-o" || *i=="--out")   && i+1!=vArgs.end())
      image = *++i;
    else if ((*i=="-l" || *i=="--link")  && i+1!=vArgs.end()) {
      link = static_cast<u32>(stoul(*++i));
      def_if_not_in_range("Link", link, MIN_LINK, MAX_LINK, DEF_LINK);
    }
    else if ((*i=="-w" || *i=="--width") && i+1!=vArgs.end()) {
      width = static_cast<u32>(stoul(*++i));
      def_if_not_in_range("Width", width, MIN_WIDT, MAX_WIDT, DEF_WIDT);
    }
    else if ((*i=="-s" || *i=="--space") && i+1!=vArgs.end()) {
      space = static_cast<u32>(stoul(*++i));
      def_if_not_in_range("Space", space, MIN_SPAC, MAX_SPAC, DEF_SPAC);
    }
    else if ((*i=="-m" || *i=="--mult")  && i+1!=vArgs.end()) {
      mult = static_cast<u32>(stoul(*++i));
      def_if_not_in_range("Mult", mult, MIN_MULT, MAX_MULT, DEF_MULT);
    }
    else if ((*i=="-b" || *i=="--begin") && i+1!=vArgs.end()) {
      start = static_cast<u32>(stoul(*++i));
      def_if_not_in_range("Begin", start, MIN_BEGI, MAX_BEGI, DEF_BEGI);
    }
    else if ((*i=="-c" || *i=="--min")   && i+1!=vArgs.end()) {
      min = static_cast<u32>(stoul(*++i));
      def_if_not_in_range("Min", min, MIN_MINP, MAX_MINP, DEF_MINP);
    }
  }
  posFile = vArgs.back();
}

inline void VizParam::help () const {
  cerr <<
    "NAME                                                                    \n"
    "  Smash++ Visualizer v"<<VERSION<<" - Visualization of Samsh++ output   \n"
    "                                                                        \n"
    "AUTHORS                                                                 \n"
    "  Morteza Hosseini    seyedmorteza@ua.pt                                \n"
    "  Diogo   Pratas      pratas@ua.pt                                      \n"
    "                                                                        \n"
    "SYNOPSIS                                                                \n"
    "  ./smashpp -viz [OPTION]...  -o [SVG_FILE] [POS_FILE]                  \n"
    "                                                                        \n"
    "SAMPLE                                                                  \n"
//    "  ./smashpp -viz -sp -sn -o out.svg ab.pos                            \n"
    "                                                                        \n"
    "DESCRIPTION                                                             \n"
    "  Mandatory arguments:                                                  \n"
    "  [POS_FILE]               positions file, generated by                 \n"
    "                             Smash++ program (*.pos)                    \n"
    "                                                                        \n"
    "  Options:                                                              \n"
    "  -v,  --verbose           more information                             \n"
    "  -o,  --out [SVG_FILE]    output image filename with map               \n"
    "  -sp, --show_pos          show positions                               \n"
    "  -sn, --show_nrc          show normalized relative                     \n"
    "                             compression (NRC)                          \n"
    "  -sr, --show_redun        show self complexity                         \n"
    "  -l,  --link  [NUM]       type of the link between maps [0;5]          \n"
    "  -w,  --width [NUM]       width of the image sequence                  \n"
    "  -s,  --space [NUM]       space between sequences                      \n"
    "  -m,  --mult  [NUM]       multiplication factor for color ID           \n"
    "  -b,  --begin [NUM]       color id beginning of color ID               \n"
    "  -c,  --min   [NUM]       minimum block size to consider               \n"
    "  -i,  --dont_show_inv     do NOT show inverse maps                     \n"
    "  -r,  --dont_show_reg     do NOT show regular maps                     \n"
    "  -h,  --help              usage guide                                  \n"
    "                                                                        \n"
    "COPYRIGHT                                                               \n"
    "  Copyright (C) "<<DEV_YEARS<<", IEETA, University of Aveiro.           \n"
    "  You may redistribute copies of this Free software                     \n"
    "  under the terms of the GNU - General Public License                   \n"
    "  v3 <http://www.gnu.org/licenses/gpl.html>. There is                   \n"
    "  NOT ANY WARRANTY, to the extent permitted by law.                "<<endl;
}
}

#endif //SMASHPP_PAR_HPP