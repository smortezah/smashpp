//
// Created by morteza on 02-02-2018.
//

#ifndef SMASHPP_PAR_HPP
#define SMASHPP_PAR_HPP

#include <fstream>
#include <vector>
#include <algorithm>
#include "assert.hpp"
#include "def.hpp"
#include "fn.hpp"
#include "vizdef.hpp"

namespace smashpp {
class Param {   // Parameters
 public:
  string      ref, tar, seq;
  u8          level;
  bool        verbose;
  u8          nthr;
  string      rmodelsPars, tmodelsPars;
  u32         wsize;
  WType       wtype;
  u64         sampleStep;
  float       thresh;
  bool        manWSize, manThresh, manFilterScale;
  FilterScale filterScale;
  bool        saveSeq, saveProfile, saveFilter, saveSegment, saveAll;
  FileType    refType, tarType;
  bool        showInfo;
  string      report;
  bool        compress, filter, segment;

  // Define Param::Param(){} in *.hpp => compile error
  //todo clean
  Param () : level(DEF_LVL), verbose(false), nthr(DEF_THR), wsize(DEF_WS),
             wtype(DEF_WT), sampleStep(1ull), thresh(DEF_THRESH),
             manWSize(false), manThresh(false), manFilterScale(false), 
             filterScale(DEF_FS),
             saveSeq(false), 
             saveProfile(false), saveFilter(false), saveSegment(false), 
             saveAll(false), refType(FileType::SEQ), tarType(FileType::SEQ), 
             showInfo(true), compress(false), filter(false), segment(false) {}

  void parse (int, char**&);
  WType win_type (const string&) const;
  string print_win_type () const;
  FilterScale filter_scale (const string&) const;
  string print_filter_scale () const;

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
    else if (*i=="-sb" || *i=="--save-seq")        saveSeq=true;
    else if (*i=="-sp" || *i=="--save-profile")    saveProfile=true;
    else if (*i=="-sf" || *i=="--save-fitler")     saveFilter=true;
    else if (*i=="-ss" || *i=="--save-segment")    saveSegment=true;
    else if (*i=="-sa" || *i=="--save-all")        saveAll=true;
    else if (*i=="-t"  || *i=="--tar") {
      if (i+1 != vArgs.end()) { tar=*++i;  check_file(tar); }
      else error("target file not specified. Use \"-t <fileName>\".");
    }
    else if (*i=="-r"  || *i=="--ref") {
      if (i+1 != vArgs.end()) { ref=*++i;  check_file(ref); }
      else error("reference file not specified. Use \"-r <fileName>\".");
    }
    else if ((*i=="-l" || *i=="--level") && i+1!=vArgs.end()) {
      level = static_cast<u8>(stoi(*++i));
      auto range = make_unique<ValRange<u8>>(MIN_LVL, MAX_LVL, DEF_LVL, 
        "Level", "[]", "default", Problem::WARNING);
      range->assert(level);
    }
    else if ((*i=="-n" || *i=="--nthr") && i+1!=vArgs.end()) {
      nthr = static_cast<u8>(stoi(*++i));
      auto range = make_unique<ValRange<u8>>(MIN_THR, MAX_THR, DEF_THR, 
        "Number of threads", "[]", "default", Problem::WARNING);
      range->assert(nthr);
    }
    else if ((*i=="-rm" || *i=="--ref-model") && i+1!=vArgs.end()) {
      rmodelsPars = *++i;
      if (rmodelsPars[0]=='-' || rmodelsPars.empty())
        error("incorrect reference model parameters.");
    }
    else if ((*i=="-tm" || *i=="--tar-model") && i+1!=vArgs.end()) {
      tmodelsPars = *++i;
      if (tmodelsPars[0]=='-' || tmodelsPars.empty())
        error("incorrect target model parameters.");
    }
    else if ((*i=="-w" || *i=="--wsize") && i+1!=vArgs.end()) {
      manWSize = true;
      wsize = static_cast<u32>(stoi(*++i));
      auto range = make_unique<ValRange<u32>>(MIN_WS, MAX_WS, DEF_WS, 
        "Window size", "[]", "default", Problem::WARNING);
      range->assert(wsize);
    }
    else if ((*i=="-wt"|| *i=="--wtype") && i+1!=vArgs.end()) {
      const auto is_win_type = [] (const string& t) {
        if (t=="0" || t=="rectangular" || t=="1" || t=="hamming"  ||
            t=="2" || t=="hann"        || t=="3" || t=="blackman" ||
            t=="4" || t=="triangular"  || t=="5" || t=="welch"    ||
            t=="6" || t=="sine"        || t=="7" || t=="nuttall")
          return true;
        return false;
      };
      const string cmd {*++i};
      auto set = make_unique<ValSet<WType>>(SET_WTYPE, DEF_WT, "Window type",
        "default", Problem::WARNING, win_type(cmd), is_win_type(cmd));
      set->assert(wtype);
    }
    else if ((*i=="-d" || *i=="--step") && i+1!=vArgs.end()) {
      sampleStep = stoull(*++i);
      if (sampleStep==0)  sampleStep=1ull;
    }
    else if ((*i=="-th"|| *i=="--thresh") && i+1!=vArgs.end()) {
      manThresh = true;
      thresh = stof(*++i);
      auto range = make_unique<ValRange<float>>(MIN_THRESH, MAX_THRESH, 
        DEF_THRESH, "Threshold", "(]", "default", Problem::WARNING);
      range->assert(thresh);
    }
    else if ((*i=="-fs"|| *i=="--filter-scale") && i+1!=vArgs.end()) {
      manFilterScale = true;
      const auto is_filter_scale = [] (const string& s) {
        if (s=="S" || s=="small" || s=="M" || s=="medium" || 
            s=="L" || s=="large")
          return true;
        return false;
      };
      const string cmd {*++i};
      auto set = make_unique<ValSet<FilterScale>>(SET_FSCALE, DEF_FS,
        "Filter scale", "default", Problem::WARNING, filter_scale(cmd),
        is_filter_scale(cmd));
      set->assert(filterScale);
    }
    else if (*i=="-R"  || *i=="--report")
      report = (i+1!=vArgs.end()) ? *++i : "report.txt";
  }

  // Mandatory args
  const bool has_t   {has(vArgs.begin(), vArgs.end(), "-t")};
  const bool has_tar {has(vArgs.begin(), vArgs.end(), "--tar")};
  const bool has_r   {has(vArgs.begin(), vArgs.end(), "-r")};
  const bool has_ref {has(vArgs.begin(), vArgs.end(), "--ref")};
  if (!has_t && !has_tar)
    error("target file not specified. Use \"-t <fileName>\".");
  else if (!has_r && !has_ref)
    error("reference file not specified. Use \"-r <fileName>\".");

  if (!has(vArgs.begin(), vArgs.end(), "-rm") &&
      !has(vArgs.begin(), vArgs.end(), "--ref-model"))
    rmodelsPars = LEVEL[level];
  if (!has(vArgs.begin(), vArgs.end(), "-tm") &&
      !has(vArgs.begin(), vArgs.end(), "--tar-model"))
    tmodelsPars = REFFREE_LEVEL[level];

  manFilterScale = !manThresh;
  manFilterScale = !manWSize;
  
  keep_in_range(wsize, 0ull, min(file_size(ref),file_size(tar)));

  // Fasta/Fastq to Seq
  auto convert_to_seq = [&] (const string& f, const FileType& type) {
    rename(f.c_str(), (f+LBL_BAK).c_str());
    to_seq(f+LBL_BAK, f, type);
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
  const auto b  = [] (string&& s) { return bold(std::move(s));      };
  const auto it = [] (string&& s) { return italic(std::move(s));    };
  const auto ul = [] (string&& s) { return underline(std::move(s)); };
  const auto hl = [] (string&& s) { return highlight(std::move(s)); };
  const auto br = [] (string&& s) { return bold_red(std::move(s));  };

  cerr <<
    b("NAME")+                                                              "\n"
    "  Smash++ v"<<VERSION<< " - rearrangements finder                       \n"
    "                                                                        \n"
    +b("SYNOPSIS")+                                                         "\n"
    "  ./smashpp ["+ul("OPTIONS")+"]... -r ["+ul("REF-FILE")+"] "
                                       "-t ["+ul("TAR-FILE")+"]"            "\n"
    "                                                                        \n"
    +b("SAMPLE")+                                                           "\n"
//    "  ./smashpp -t TAR -r REF                                             \n"
    "                                                                        \n"
    +b("DESCRIPTION")+                                                      "\n"
    "  "+it("Mandatory arguments")+":"                                      "\n"
    "  "+b("-r")+",  "+b("--ref")+" "+ul("FILE")+"            "
                                     "reference file (Seq/asta/Fastq)"      "\n"
    "  "+b("-t")+",  "+b("--tar")+" "+ul("FILE")+"            "
                                     "target file    (Seq/asta/Fastq)"      "\n"
    "                                                                        \n"
    "  "+it("Options")+":"                                                  "\n"
    "  "+b("-v")+",  "+b("--verbose")+"             more information"       "\n"
    "  "+b("-l")+",  "+b("--level")+" "+ul("INT")+"           "
                                                "level of compression  [0;4] \n"
    "  "+b("-n")+",  "+b("--nthr")+"  "+ul("INT")+"           "
                                                   "number of threads  [1;8] \n"
    "  "+b("-fs")+", "+b("--filter-scale")+" S|M|L  "
                             "scale of the filter {S|small,  "+hl("filter")+"\n"
    "                             M|medium, L|large}"             "\n"
    "  "+b("-w")+",  "+b("--wsize")+" "+ul("INT")+"           "
                             "window size                    "+hl("filter")+"\n"
    "  "+b("-wt")+", "+b("--wtype")+" [0;7]         type of windowing function"
                                                       "     "+hl("filter")+"\n"
    "                               {0|rectangular, 1|hamming, 2|hann,"     "\n"
    "                               3|blackman, 4|triangular, 5|welch,"     "\n"
    "                               [6|sine]        | [7|nuttall]            \n"
    "  -d,  --step [INT]          sampling steps             <-- filter      \n"
    "  -th, --thresh [FLOAT]      threshold                  <-- filter      \n"
    "  -sp, --save-profile        save profile                               \n"
    "  -sf, --save-filter         save filtered file                         \n"
    "  -sb, --save-seq            save sequence (input is Fasta/Fastq)       \n"
    "  -ss, --save-segment        save segmented file(f)                     \n"
    "  -sa, --save-all            save profile, filetered and                \n"
    "                               segmented files                          \n"
    "  -R,  --report              save results in the \"report\" file        \n"
    "  -h,  --help                usage guide                                \n"
    "  -rm, --ref-model [\U0001D705,[\U0001D464,\U0001D451,]ir,\U0001D6FC,"
                         "\U0001D6FE/\U0001D70F,ir,\U0001D6FC,\U0001D6FE:...]\n"
    "  -tm, --tar-model [...]                                                \n"
    "                             parameters of models                       \n"
    "                               \U0001D705:  context size                \n"
    "                               \U0001D464:  width of sketch in log2 form,\n"
    "                                     e.g., set 10 for w=2^10=1024       \n"
    "                               \U0001D451:  depth of sketch             \n"
    "                               ir: inverted repeat (0|1|2)              \n"
    "                                   [0]: regular (not inverted)          \n"
    "                                   [1]: inverted, solely                \n"
    "                                   [2]: both regular and inverted       \n"
    "                               \U0001D6FC:  estimator                   \n"
    "                               \U0001D6FE:  forgetting factor [0;1)     \n"
    "                               \U0001D70F:  threshold (# substitutions) \n"
    "                                                                        \n"
    +b("AUTHOR")                                                        +"\n"
    "  Morteza Hosseini    seyedmorteza@ua.pt                                \n"
    "                                                                        \n"
    +b("COPYRIGHT")                                                     +"\n"
    "  Copyright (C) "<< DEV_YEARS <<", IEETA, University of Aveiro. You may "
                                                                  "redistri- \n"
    "  bute copies of this Free software under the terms of the GNU (Gen-    \n"
    "  eral Public License) v3 <http://www.gnu.org/licenses/gpl.html>.       \n"
    "  There is NOT ANY WARRANTY, to the extent permitted by law."       <<endl;
}

inline WType Param::win_type (const string& t) const {
  if      (t=="0" || t=="rectangular")   return WType::RECTANGULAR;
  else if (t=="1" || t=="hamming")       return WType::HAMMING;
  else if (t=="2" || t=="hann")          return WType::HANN;
  else if (t=="3" || t=="blackman")      return WType::BLACKMAN;
  else if (t=="4" || t=="triangular")    return WType::TRIANGULAR;
  else if (t=="5" || t=="welch")         return WType::WELCH;
  else if (t=="6" || t=="sine")          return WType::SINE;
  else if (t=="7" || t=="nuttall")       return WType::NUTTALL;
}

inline string Param::print_win_type () const {
  switch (wtype) {
  case WType::RECTANGULAR:  return "Rectangular";  break;
  case WType::HAMMING:      return "Hamming";      break;
  case WType::HANN:         return "Hann";         break;
  case WType::BLACKMAN:     return "Blackman";     break;
  case WType::TRIANGULAR:   return "Triangular";   break;
  case WType::WELCH:        return "Welch";        break;
  case WType::SINE:         return "Sine";         break;
  case WType::NUTTALL:      return "Nuttall";      break;
  default:                  return "Rectangular";
  }
}

inline FilterScale Param::filter_scale (const string& s) const {
  if      (s=="S" || s=="small")   return FilterScale::S;
  else if (s=="M" || s=="medium")  return FilterScale::M;
  else if (s=="L" || s=="large")   return FilterScale::L; 
}

inline string Param::print_filter_scale () const {
  switch (filterScale) {
  case FilterScale::S:  return "Small";   break;
  case FilterScale::M:  return "Medium";  break;
  case FilterScale::L:  return "Large";   break;
  default:              return "Large";
  }
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
      auto range = make_unique<ValRange<u32>>(MIN_LINK, MAX_LINK, DEF_LINK, 
        "Link", "[]", "default", Problem::WARNING);
      range->assert(link);
    }
    else if ((*i=="-w" || *i=="--width") && i+1!=vArgs.end()) {
      width = static_cast<u32>(stoul(*++i));
      auto range = make_unique<ValRange<u32>>(MIN_WIDT, MAX_WIDT, DEF_WIDT,
        "Width", "[]", "default", Problem::WARNING);
      range->assert(width);
    }
    else if ((*i=="-s" || *i=="--space") && i+1!=vArgs.end()) {
      space = static_cast<u32>(stoul(*++i));
      auto range = make_unique<ValRange<u32>>(MIN_SPAC, MAX_SPAC, DEF_SPAC,
        "Space", "[]", "default", Problem::WARNING);
      range->assert(space);
    }
    else if ((*i=="-m" || *i=="--mult")  && i+1!=vArgs.end()) {
      mult = static_cast<u32>(stoul(*++i));
      auto range = make_unique<ValRange<u32>>(MIN_MULT, MAX_MULT, DEF_MULT,
        "Mult", "[]", "default", Problem::WARNING);
      range->assert(mult);
    }
    else if ((*i=="-b" || *i=="--begin") && i+1!=vArgs.end()) {
      start = static_cast<u32>(stoul(*++i));
      auto range = make_unique<ValRange<u32>>(MIN_BEGI, MAX_BEGI, DEF_BEGI,
        "Begin", "[]", "default", Problem::WARNING);
      range->assert(start);
    }
    else if ((*i=="-c" || *i=="--min")   && i+1!=vArgs.end()) {
      min = static_cast<u32>(stoul(*++i));
      auto range = make_unique<ValRange<u32>>(MIN_MINP, MAX_MINP, DEF_MINP,
        "Min", "[]", "default", Problem::WARNING);
      range->assert(min);
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
    "                             Smash++ tool (*.pos)                       \n"
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