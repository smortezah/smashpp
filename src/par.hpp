#ifndef SMASHPP_PAR_HPP
#define SMASHPP_PAR_HPP

#include <fstream>
#include <vector>
#include <algorithm>
#include "assert.hpp"
#include "def.hpp"
#include "exception.hpp"
#include "container.hpp"
#include "file.hpp"
#include "vizdef.hpp"

namespace smashpp {
class Param {   // Parameters
 public:
  string      ref, tar, seq;
  string      refName, tarName;
  u8          level;
  bool        verbose;
  prc_t       entropyN;
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
  u32         ID;

  // Define Param::Param(){} in *.hpp => compile error
  Param () : level(LVL), verbose(false), entropyN(ENTR_N), nthr(THRD), 
    wsize(WS), wtype(WT), sampleStep(1ull), thresh(THRSH), manWSize(false),
    manThresh(false), manFilterScale(false), filterScale(FS), saveSeq(false),
    saveProfile(false), saveFilter(false), saveSegment(false), saveAll(false),
    refType(FileType::SEQ), tarType(FileType::SEQ), showInfo(true), 
    compress(false), filter(false), segment(false), ID(0) {}

  auto parse (int, char**&) -> void;
  auto win_type (const string&) const -> WType;
  auto print_win_type () const -> string;
  auto filter_scale (const string&) const -> FilterScale;
  auto print_filter_scale () const -> string;

 private:
  void help () const;
};

class VizParam {
 public:
  bool   verbose, inverse, regular, showPos, showNRC, showRedun, showAnnot;
  string image;
  u8     link, color;
  float  opacity;
  u32    width, space, mult, start, min;
  string posFile;

  VizParam () : verbose(false), inverse(true), regular(true), showPos(false),
    showNRC(false), showRedun(false), showAnnot(false), image(IMAGE), 
    link(LINK), color(COLOR), opacity(OPAC), width(WDTH), space(SPC), 
    mult(MULT), start(BEGN), min(MINP) {}

  void parse (int, char**&);

 private:
  void help () const;
};


inline void Param::parse (int argc, char**& argv) {
  if (argc < 2) { help();  throw EXIT_SUCCESS; }

  vector<string> vArgs(static_cast<u64>(argc));
  for (int i=0; i!=argc; ++i)
    vArgs.emplace_back(static_cast<string>(argv[i]));

  bool man_rm=false, man_tm=false;
  for (auto i=vArgs.begin(); i!=vArgs.end(); ++i) {
    if      (*i=="-h"  || *i=="--help") { help();  throw EXIT_SUCCESS; }
    else if (*i=="-v"  || *i=="--verbose")         verbose    =true;
    else if (*i=="-compress")                      compress   =true;
    else if (*i=="-filter")                        filter     =true;
    else if (*i=="-segment")                       segment    =true;
    else if (*i=="-sb" || *i=="--save-seq")        saveSeq    =true;
    else if (*i=="-sp" || *i=="--save-profile")    saveProfile=true;
    else if (*i=="-sf" || *i=="--save-fitler")     saveFilter =true;
    else if (*i=="-ss" || *i=="--save-segment")    saveSegment=true;
    else if (*i=="-sa" || *i=="--save-all")        saveAll    =true;
    else if (*i=="-t"  || *i=="--tar") {
      if (i+1 != vArgs.end()) {
        tar = *++i;
        check_file(tar);
        tarName = file_name(tar);
      }
      else
        error("target file not specified. Use \"-t <fileName>\".");
    }
    else if (*i=="-r"  || *i=="--ref") {
      if (i+1 != vArgs.end()) {
        ref = *++i;
        check_file(ref);
        refName = file_name(ref);
      }
      else error("reference file not specified. Use \"-r <fileName>\".");
    }
    else if ((*i=="-l" || *i=="--level") && i+1!=vArgs.end()) {
      level = static_cast<u8>(stoi(*++i));
      auto range = make_unique<ValRange<u8>>(MIN_LVL, MAX_LVL, LVL, 
        "Level", "[]", "default", Problem::WARNING);
      range->assert(level);
    }
    else if ((*i=="-e" || *i=="--ent-n") && i+1!=vArgs.end()) {
      entropyN = static_cast<prc_t>(stod(*++i));
      auto range = make_unique<ValRange<prc_t>>(MIN_ENTR_N, MAX_ENTR_N, ENTR_N, 
        "Entropy of N bases", "[]", "default", Problem::WARNING);
      range->assert(entropyN);
    }
    else if ((*i=="-n" || *i=="--nthr") && i+1!=vArgs.end()) {
      nthr = static_cast<u8>(stoi(*++i));
      auto range = make_unique<ValRange<u8>>(MIN_THRD, MAX_THRD, THRD, 
        "Number of threads", "[]", "default", Problem::WARNING);
      range->assert(nthr);
    }
    else if ((*i=="-rm" || *i=="--ref-model") && i+1!=vArgs.end()) {
      man_rm = true;
      rmodelsPars = *++i;
      if (rmodelsPars[0]=='-' || rmodelsPars.empty())
        error("incorrect reference model parameters.");
    }
    else if ((*i=="-tm" || *i=="--tar-model") && i+1!=vArgs.end()) {
      man_tm = true;
      tmodelsPars = *++i;
      if (tmodelsPars[0]=='-' || tmodelsPars.empty())
        error("incorrect target model parameters.");
    }
    else if ((*i=="-w" || *i=="--wsize") && i+1!=vArgs.end()) {
      manWSize = true;
      wsize = static_cast<u32>(stoi(*++i));
      auto range = make_unique<ValRange<u32>>(MIN_WS, MAX_WS, WS, 
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
      auto set = make_unique<ValSet<WType>>(SET_WTYPE, WT, "Window type",
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
      auto range = make_unique<ValRange<float>>(MIN_THRSH, MAX_THRSH, THRSH,
        "Threshold", "(]", "default", Problem::WARNING);
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
      auto set = make_unique<ValSet<FilterScale>>(SET_FSCALE, FS,
        "Filter scale", "default", Problem::WARNING, filter_scale(cmd),
        is_filter_scale(cmd));
      set->assert(filterScale);
    }
    else if (*i=="-R"  || *i=="--report")
      report = (i+1!=vArgs.end()) ? *++i : "report.txt";
  }

  // Mandatory args
  const bool has_t   {has(vArgs.begin(), vArgs.end(), "-t")   };
  const bool has_tar {has(vArgs.begin(), vArgs.end(), "--tar")};
  const bool has_r   {has(vArgs.begin(), vArgs.end(), "-r")   };
  const bool has_ref {has(vArgs.begin(), vArgs.end(), "--ref")};
  if (!has_t && !has_tar)
    error("target file not specified. Use \"-t <fileName>\".");
  else if (!has_r && !has_ref)
    error("reference file not specified. Use \"-r <fileName>\".");

  if (!man_rm && !man_tm) {
    rmodelsPars = LEVEL[level];
    tmodelsPars = REFFREE_LEVEL[level];
  }
  else if (!man_rm && man_tm) {
    rmodelsPars = tmodelsPars;
  }
  else if (man_rm  && !man_tm) {
    tmodelsPars = rmodelsPars;
  }

  manFilterScale = !manThresh;
  manFilterScale = !manWSize;
  
  keep_in_range(0ull, wsize, min(file_size(ref),file_size(tar)));

  // Fasta/Fastq to Seq
  auto convert_to_seq = [&](const string& f, const FileType& type) {
    rename(f.c_str(), (f+LBL_BAK).c_str());
    to_seq(f+LBL_BAK, f, type);
  };
  refType = file_type(ref);
  if      (refType==FileType::FASTA)  
    convert_to_seq(ref, FileType::FASTA);
  else if (refType==FileType::FASTQ)  
    convert_to_seq(ref, FileType::FASTQ);
  else if (refType!=FileType::SEQ) 
    error("\""+refName+"\" has unknown format.");
  // else if (refType!=FileType::SEQ)    error("\""+ref+"\" has unknown format.");

  tarType = file_type(tar);
  if      (tarType==FileType::FASTA)  
    convert_to_seq(tar, FileType::FASTA);
  else if (tarType==FileType::FASTQ) 
    convert_to_seq(tar, FileType::FASTQ);
  else if (tarType!=FileType::SEQ)    
    error("\""+tarName+"\" has unknown format.");
    // error("\""+tar+"\" has unknown format.");
}

inline void Param::help () const {
  const auto b   = [] (string&& s) { return bold(std::move(s));          };
  const auto it  = [] (string&& s) { return italic(std::move(s));        };
  const auto fit = [] (string&& s) { return faint(italic(std::move(s))); };
  const auto ul  = [] (string&& s) { return underline(std::move(s));     };

  cerr 
  << b("NAME")                                                            <<'\n'
  << "  Smash++ v" << VERSION << " - rearrangements finder"               <<'\n'
  <<                                                                        '\n'
  << b("SYNOPSIS")                                                        <<'\n'
  << "  ./smashpp [" << ul("OPTIONS") << "]...  "
     "-r [" << ul("REF-FILE") << "] -t [" << ul("TAR-FILE") << "]"        <<'\n'
  <<                                                                        '\n'
  << b("SAMPLE")                                                          <<'\n'
//    "  ./smashpp -t TAR -r REF                                             \n"
  <<                                                                        '\n'
  << b("DESCRIPTION")                                                     <<'\n'
  << "  " << it("Mandatory arguments") << ":"                             <<'\n'
  << "  " << b("-r") << ",  " << b("--ref") << " " << ul("FILE") << 
     "            reference file (Seq/Fasta/Fastq)"                       <<'\n'
  << "  " << b("-t") << ",  " << b("--tar") << " " << ul("FILE") << 
     "            target file    (Seq/Fasta/Fastq)"                       <<'\n'
  <<                                                                        '\n'
  << "  " << it("Options") << ":"                                         <<'\n'
  << "  " << b("-v") << ",  " << b("--verbose") << "             "
     "more information"                                                   <<'\n'
  << "  " << b("-l") << ",  " << b("--level") << " " << ul("INT") << 
     "           level of compression "
     "[" << to_string(MIN_LVL) << ";" << to_string(MAX_LVL) << "]     " << 
     fit("COMPRESS")                                                      <<'\n'
  << "  " << b("-e") << ",  " << b("--ent-n") << " " << ul("FLOAT") << 
     "         Entropy of 'N's [" << 
     string_format("%.1f",MIN_ENTR_N) << ";" << string_format("%.1f",MAX_ENTR_N)
     << "]    " << fit("COMPRESS")                                        <<'\n'
  << "  " << b("-n") << ",  " << b("--nthr") << "  " << ul("INT") << 
     "           number of threads "
     "[" << to_string(MIN_THRD) << ";" << to_string(MAX_THRD) << "]"      <<'\n'
  << "  " << b("-fs") << ", " << b("--filter-scale") << " S|M|L  "
     "scale of the filter {S|small,    " << fit("FILTER")                 <<'\n'
  << "                             M|medium, L|large}"                    <<'\n'
  << "  " << b("-w") << ",  " << b("--wsize") << " " << ul("INT") <<
     "           window size "
     "[" << to_string(MIN_WS) << ";" << to_string(MAX_WS) << "]           " << 
     fit("FILTER")                                                        <<'\n'
  << "  " << b("-wt") << ", " << b("--wtype") << " [0;7]         "
     "type of windowing function       " << fit("FILTER")                 <<'\n'
  << "                             {0|rectangular, 1|hamming, 2|hann,"    <<'\n'
  << "                             3|blackman, 4|triangular, 5|welch,"    <<'\n'
  << "                             6|sine, 7|nuttall}"                    <<'\n'
  << "  " << b("-d") << ",  " << b("--step") << "   " << ul("INT") << 
     "          sampling steps                   " << fit("FILTER")       <<'\n'
  << "  " << b("-th") << ", " << b("--thresh") << " " << ul("FLOAT") << 
     "        threshold [" << 
     string_format("%.1f",MIN_THRSH) << ";" << string_format("%.1f",MAX_THRSH)
     << "]             " << fit("FILTER")                                 <<'\n'
  << "  " << b("-sp") << ", " << b("--save-profile") << "        "
     "save profile (*.prf)               " << fit("SAVE")                 <<'\n'
  << "  " << b("-sf") << ", " << b("--save-filter") << "         "
     "save filtered file (*.fil)         " << fit("SAVE")                 <<'\n'
  << "  " << b("-sb") << ", " << b("--save-seq") << "            "
     "save sequence (input: Fasta/Fastq) " << fit("SAVE")                 <<'\n'
  << "  " << b("-ss") << ", " << b("--save-segment") << "        "
     "save segmented files (*-s\U00002099)        " << fit("SAVE")        <<'\n'
  << "  " << b("-sa") << ", " << b("--save-all") << "            "
     "save profile, filetered and        " << fit("SAVE")                 <<'\n'
  << "                             segmented files"                       <<'\n'
  // << "  " << b("-R") << ",  " << b("--report") << "              "
  //    "save results in file \"report\"    " << fit("REPORT")               <<'\n'
  << "  " << b("-h") << ",  " << b("--help") << "                usage guide \n"
  << "  " << b("-rm") << ", " << b("--ref-model") << " [\U0001D705,"
     "[\U0001D464,\U0001D451,]ir,\U0001D6FC,\U0001D6FE/\U0001D70F,ir,"
     "\U0001D6FC,\U0001D6FE:...]               " << fit("MODEL")          <<'\n'
  << "  " << b("-tm") << ", " << b("--tar-model") << " [\U0001D705,"
     "[\U0001D464,\U0001D451,]ir,\U0001D6FC,\U0001D6FE/\U0001D70F,ir,"
     "\U0001D6FC,\U0001D6FE:...]               " << fit("MODEL")          <<'\n'
  << "                             parameters of models"                  <<'\n'
  << "                       (" << ul("INT") << ") \U0001D705:  context size \n"
  << "                       (" << ul("INT") << ") \U0001D464:  width of "
     "sketch in log2 form, "                                              <<'\n'
  << "                                 e.g., set 10 for w=2^10=1024"      <<'\n'
  << "                       (" << ul("INT") << ") \U0001D451:  depth "
     "of sketch"                                                          <<'\n'
  << "                             ir: inverted repeat {0, 1, 2}"         <<'\n'
  << "                                 0: regular (not inverted)"         <<'\n'
  << "                                 1: inverted, solely"               <<'\n'
  << "                                 2: both regular and inverted"      <<'\n'
  << "                     (" << ul("FLOAT") << ") \U0001D6FC:  estimator"<<'\n'
  << "                     (" << ul("FLOAT") << ") \U0001D6FE:  "
     "forgetting factor [0.0;1.0)"                                        <<'\n'
  << "                       (" << ul("INT") << ") \U0001D70F:  "
     "threshold (no. substitutions)"                                      <<'\n'
  <<                                                                        '\n'
  << b("AUTHOR")                                                          <<'\n'
  << "  Morteza Hosseini           seyedmorteza@ua.pt"                    <<'\n'
  <<                                                                        '\n'
  << b("COPYRIGHT")                                                       <<'\n'
  << "  Copyright (C) " << DEV_YEARS << ", IEETA, University of Aveiro. "
     "You may redistri-"                                                    "\n"
  << "  bute copies of this Free software under the terms of the GNU (Gen-   \n"
  << "  eral Public License) v3 <http://www.gnu.org/licenses/gpl.html>.      \n"
  << "  There is NOT ANY WARRANTY, to the extent permitted by law."      <<endl;
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
  else                                   return WType::HANN;
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
  else                             return FilterScale::L;
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
    else if (*i=="-sp" || *i=="--show-pos")         showPos=true;
    else if (*i=="-sn" || *i=="--show-nrc")         showNRC=true;
    else if (*i=="-sr" || *i=="--show-redun")       showRedun=true;
    else if (*i=="-sa" || *i=="--show-annot")       showAnnot=true;
    else if (*i=="-ni" || *i=="--dont-show-inv")    inverse=false;
    else if (*i=="-nr" || *i=="--dont-show-reg")    regular=false;
    else if ((*i=="-o" || *i=="--out")   && i+1!=vArgs.end())
      image = *++i;
    else if ((*i=="-l" || *i=="--link")  && i+1!=vArgs.end()) {
      link = static_cast<u8>(stoul(*++i));
      auto range = make_unique<ValRange<u8>>(MIN_LINK, MAX_LINK, LINK, 
        "Link", "[]", "default", Problem::WARNING);
      range->assert(link);
    }
    else if ((*i=="-c" || *i=="--color")  && i+1!=vArgs.end()) {
      color = static_cast<u8>(stoi(*++i));
      auto range = make_unique<ValRange<u8>>(MIN_COLOR, MAX_COLOR, COLOR, 
        "Color", "[]", "default", Problem::WARNING);
      range->assert(color);
    }
    else if ((*i=="-p" || *i=="--opacity")  && i+1!=vArgs.end()) {
      opacity = stof(*++i);
      auto range = make_unique<ValRange<float>>(MIN_OPAC, MAX_OPAC, OPAC, 
        "Opacity", "[]", "default", Problem::WARNING);
      range->assert(opacity);
    }
    else if ((*i=="-w" || *i=="--width") && i+1!=vArgs.end()) {
      width = static_cast<u32>(stoul(*++i));
      auto range = make_unique<ValRange<u32>>(MIN_WDTH, MAX_WDTH, WDTH,
        "Width", "[]", "default", Problem::WARNING);
      range->assert(width);
    }
    else if ((*i=="-s" || *i=="--space") && i+1!=vArgs.end()) {
      space = static_cast<u32>(stoul(*++i));
      auto range = make_unique<ValRange<u32>>(MIN_SPC, MAX_SPC, SPC,
        "Space", "[]", "default", Problem::WARNING);
      range->assert(space);
    }
    else if ((*i=="-m" || *i=="--mult")  && i+1!=vArgs.end()) {
      mult = static_cast<u32>(stoul(*++i));
      auto range = make_unique<ValRange<u32>>(MIN_MULT, MAX_MULT, MULT,
        "Mult", "[]", "default", Problem::WARNING);
      range->assert(mult);
    }
    else if ((*i=="-b" || *i=="--begin") && i+1!=vArgs.end()) {
      start = static_cast<u32>(stoul(*++i));
      auto range = make_unique<ValRange<u32>>(MIN_BEGN, MAX_BEGN, BEGN,
        "Begin", "[]", "default", Problem::WARNING);
      range->assert(start);
    }
    else if ((*i=="-k" || *i=="--min")   && i+1!=vArgs.end()) {
      min = static_cast<u32>(stoul(*++i));
      auto range = make_unique<ValRange<u32>>(MIN_MINP, MAX_MINP, MINP,
        "Min", "[]", "default", Problem::WARNING);
      range->assert(min);
    }
  }
  posFile = vArgs.back();
}

inline void VizParam::help () const {
  const auto b   = [](string&& s) { return bold(std::move(s));          };
  const auto it  = [](string&& s) { return italic(std::move(s));        };
  const auto fit = [](string&& s) { return faint(italic(std::move(s))); };
  const auto ul  = [](string&& s) { return underline(std::move(s));     };

  cerr 
  << b("NAME")                                                            <<'\n'
  << "  Smash++ Visualizer v" << VERSION << " - "
     "Visualization of Samsh++ output"                                    <<'\n'
  <<                                                                        '\n'
  << b("SYNOPSIS")                                                        <<'\n'
  << "  ./smashpp -viz [" << ul("OPTION")   << "]...  "
     "-o [" << ul("SVG-FILE") << "] " << ul("POS-FILE") << "]"            <<'\n'
  <<                                                                        '\n'
  << b("SAMPLE")                                                          <<'\n'
//    "  ./smashpp -viz -sp -sn -o out.svg ab.pos                            \n"
  <<                                                                        '\n'
  << b("DESCRIPTION")                                                     <<'\n'
  << "  " << it("Mandatory arguments") << ":"                             <<'\n'
  << "  " << ul("POS-FILE") << "                   "
     "positions file, enerated by      " << fit("INPUT")                  <<'\n'
  << "                             Smash++ tool (*.pos)"                  <<'\n'
  <<                                                                        '\n'
  << "  " << it("Options") << ":"                                         <<'\n'
  << "  " << b("-v")  << ",  " << b("--verbose") << "             "
     "more information"                                                   <<'\n'
  << "  " << b("-o")  << ",  " << b("--out") << " " << ul("SVG-FILE") <<
     "        output image name (*.svg)       " << fit("OUTPUT")          <<'\n'
  << "  " << b("-sp") << ", "  << b("--show-pos") << "            "
     "show positions                    " << fit("SHOW")                  <<'\n'
  << "  " << b("-sn") << ", "  << b("--show-nrc") << "            "
     "show normalized relative          " << fit("SHOW")                  <<'\n'
  << "                             compression (NRC)"                     <<'\n'
  << "  " << b("-sr") << ", "  << b("--show-redun") << "          "
     "show self complexity              " << fit("SHOW")                  <<'\n'
  << "  " << b("-sa") << ", "  << b("--show-annot") << "          "
     "show annotation                   " << fit("SHOW")                  <<'\n'
  << "  " << b("-ni") << ", "  << b("--dont-show-inv") << "       "
     "do NOT show inverse maps       " << fit("NO SHOW")                  <<'\n'
  << "  " << b("-nr") << ", "  << b("--dont-show-reg") << "       "
     "do NOT show regular maps       " << fit("NO SHOW")                  <<'\n'
  << "  " << b("-l") << ",  "  << b("--link") << "    " << ul("INT") <<
     "         type of the link between maps "
     "[" << to_string(MIN_LINK) << ";" << to_string(MAX_LINK) << "]"      <<'\n'
  << "  " << b("-c") << ",  "  << b("--color") << "   " << ul("INT") <<
     "         color mode "
     "[" << to_string(MIN_COLOR) << ";" << to_string(MAX_COLOR) << "]"    <<'\n'
  << "  " << b("-p") << ",  "  << b("--opacity") << " " << ul("FLOAT") <<
     "       opacity [" << string_format("%.1f",MIN_OPAC) << ";" 
                        << string_format("%.1f",MAX_OPAC) << "]"          <<'\n'
  << "  " << b("-w") << ",  "  << b("--width") << "   " << ul("INT") <<
     "         width of the image sequence "
     "[" << to_string(MIN_WDTH) << ";" << to_string(MAX_WDTH) << "]"      <<'\n'
  << "  " << b("-s") << ",  "  << b("--space") << "   " << ul("INT") <<
     "         space between sequences "
     "[" << to_string(MIN_SPC) << ";" << to_string(MAX_SPC) << "]"        <<'\n'
  << "  " << b("-m") << ",  "  << b("--mult") << "    " << ul("INT") <<
     "         multiplication factor for"                                 <<'\n'
  << "                             color ID "
     "[" << to_string(MIN_MULT) << ";" << to_string(MAX_MULT) << "]"      <<'\n'
  << "  " << b("-b") << ",  "  << b("--begin") << "   " << ul("INT") <<
     "         beginning of color ID "
     "[" << to_string(MIN_BEGN) << ";" << to_string(MAX_BEGN) << "]"      <<'\n'
  << "  " << b("-k") << ",  "  << b("--min") << "     " << ul("INT") <<
     "         minimum block size to"                                     <<'\n'
  << "                             consider "
     "[" << to_string(MIN_MINP) << ";" << to_string(MAX_MINP) << "]"      <<'\n'
  << "  " << b("-h") << ",  " << b("--help") << "                usage guide \n"
  <<                                                                        '\n'
  << b("AUTHORS")                                                         <<'\n'
  << "  Morteza Hosseini           seyedmorteza@ua.pt"                    <<'\n'
  << "  Diogo   Pratas             pratas@ua.pt"                          <<'\n'
  <<                                                                        '\n'
  << b("COPYRIGHT")                                                       <<'\n'
  << "  Copyright (C) " << DEV_YEARS << ", IEETA, University of Aveiro. "
     "You may redistri-"                                                    "\n"
  << "  bute copies of this Free software under the terms of the GNU (Gen-   \n"
  << "  eral Public License) v3 <http://www.gnu.org/licenses/gpl.html>.      \n"
  << "  There is NOT ANY WARRANTY, to the extent permitted by law."      <<endl;
}
}

#endif //SMASHPP_PAR_HPP