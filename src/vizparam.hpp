//
// Created by morteza on 30-10-2018.
//

#ifndef PROJECT_VIZPARAM_HPP
#define PROJECT_VIZPARAM_HPP

#include "def.hpp"
#include "fn.hpp"
#include "vizdef.hpp"

namespace smashpp {
class VizParam {
 public:
  bool   help, verbose, force, inverse, regular;
  string image;
  u32    link, width, space, mult, start, min;
  string posFile;

  VizParam() : help(DEF_HELP), verbose(DEF_VERBOSE), force(DEF_FORCE),
               inverse(DEF_INVE), regular(DEF_REGU), image(DEF_IMAGE),
               link(DEF_LINK), width(DEF_WIDT), space(DEF_SPAC), mult(DEF_MULT),
               start(DEF_BEGI), min(DEF_MINP) {}
  void parse (int, char**&);

 private:
  void print_menu_visual () const;
  void print_version     () const;
};

inline void VizParam::parse (int argc, char**& argv) {
  if (argc < 2) {
    print_menu_visual();  throw EXIT_SUCCESS;
  }
  else {
    vector<string> vArgs(static_cast<u64>(argc));
    for (int i=0; i!=argc; ++i)
      vArgs.emplace_back(static_cast<string>(argv[i]));

    const auto check_range = [=](u32 val, u32 min, u32 max) {
      if (val<min || val>max)
        error(
          "invalid number! Interval: ["+to_string(min)+";"+to_string(max)+"].");
    };

    for (auto i=vArgs.begin(); i!=vArgs.end(); ++i) {
      if      (*i=="-h") { print_menu_visual(); help=true; throw EXIT_SUCCESS; }
      else if (*i=="-V") { print_version();                throw EXIT_SUCCESS; }
      else if (*i=="-v")   verbose=true;
      else if (*i=="-F")   force=true;
      else if (*i=="-i")   inverse=true;
      else if (*i=="-r")   regular=true;
      else if (*i=="-o" && i+1!=vArgs.end())
        image = *++i;
      else if (*i=="-l" && i+1!=vArgs.end()) {
        link = static_cast<u32>(stoul(*++i));
        check_range(link, MIN_LINK, MAX_LINK);
      }
      else if (*i=="-w" && i+1!=vArgs.end()) {
        width = static_cast<u32>(stoul(*++i));
        check_range(width, MIN_WIDT, MAX_WIDT);
      }
      else if (*i=="-s" && i+1!=vArgs.end()) {
        space = static_cast<u32>(stoul(*++i));
        check_range(space, MIN_SPAC, MAX_SPAC);
      }
      else if (*i=="-m" && i+1!=vArgs.end()) {
        mult = static_cast<u32>(stoul(*++i));
        check_range(mult, MIN_MULT, MAX_MULT);
      }
      else if (*i=="-b" && i+1!=vArgs.end()) {
        start = static_cast<u32>(stoul(*++i));
        check_range(start, MIN_BEGI, MAX_BEGI);
      }
      else if (*i=="-c" && i+1!=vArgs.end()) {
        min = static_cast<u32>(stoul(*++i));
        check_range(min, MIN_MINP, MAX_MINP);
      }
    }
    posFile = vArgs.back();
  }
}

inline void VizParam::print_menu_visual () const {
  cerr <<
    "Usage: visualizer-visual [OPTION]... [FILE]                             \n"
    "Visualization of visualizer-map output.                                 \n"
    "                                                                        \n"
    "Non-mandatory arguments:                                                \n"
    "                                                                        \n"
    "  -h                         give this help,                            \n"
    "  -V                         display version number,                    \n"
    "  -v                         verbose mode (more information),           \n"
    "  -l <link>                  link type between maps [0;4],              \n"
    "  -w <width>                 image sequence width,                      \n"
    "  -s <space>                 space between sequences,                   \n"
    "  -m <mult>                  color id multiplication factor,            \n"
    "  -b <begin>                 color id beggining,                        \n"
    "  -c <min>               min block size to consider,            \n"
    "  -i                         do NOT show inverse maps,                \n"
    "  -r                         do NOT show regular maps,                  \n"
    "  -o <FILE>                  output image filename with map,            \n"
    "                                                                        \n"
    "Mandatory arguments:                                                    \n"
    "                                                                        \n"
    "  <FILE>                     filename with positions from the           \n"
    "                             visualizer-map program (*.pos),            \n"
    "                                                                        \n"
    "Report bugs to <pratas@ua.pt>."                                     <<endl;
}

inline void VizParam::print_version () const {
  cerr <<
    "                                                                        \n"
    "                         =====================                          \n"
    "                         |   ELEANORA v"
    <<VERSION_viz<<"."<<RELEASE_viz<<"   |                         \n"
    "                         =====================                          \n"
    "                                                                        \n"
    "                  Ultra-fast mapping and visualization                  \n"
    "                       of similar genomic regions                       \n"
    "                                                                        \n"
    "                  It includes the following programs:                   \n"
    "                           * visualizer-map                             \n"
    "                           * visualizer-visual                          \n"
    "                                                                        \n"
    "Copyright (C) 2015-2018 University of Aveiro. This is a Free software.  \n"
    "You may redistribute copies of it under the terms of the GNU - General  \n"
    "Public License v3 <http://www.gnu.org/licenses/gpl.html>. There is NOT  \n"
    "ANY WARRANTY, to the extent permitted by law. Developed and Written by  \n"
    "Diogo Pratas and Morteza Hosseini.\n"                               <<endl;
}
}

#endif //PROJECT_VIZPARAM_HPP