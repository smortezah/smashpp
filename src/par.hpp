//
// Created by morteza on 02-02-2018.
//

#ifndef SMASHPP_PAR_HPP
#define SMASHPP_PAR_HPP

#include <fstream>
#include <vector>
#include <algorithm>
#include "def.hpp"
using std::cerr;
using std::endl;
using std::vector;
using std::string;
using std::stoi;
using std::stof;
using std::ifstream;

#include <stdexcept>
#include <sstream>

class Param    // Parameters
{
 public:
  string tar;
  string ref;
  bool   verbose;
  u8     nthr;
  u8     level;
  
  Param () {    // Define Param::Param(){} in *.hpp => compile error
    tar     = "";
    ref     = "";
    verbose = false;
    nthr    = DEF_THR;
    level   = DEF_LVL;
  }
  
  void parse (int argc, char**& argv) {
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
          if (i+1 != vArgs.end()) {
            tar = *++i;
            checkFile(tar);
          } else {
            throw std::runtime_error
              ("Error: target file not specified. Use \"-t fileName\".\n");
          }
        }
        else if (*i=="-r" || *i=="--ref") {
          if (i+1 != vArgs.end()) {
            ref = *++i;
            checkFile(ref);
          } else {
            throw std::runtime_error
              ("Error: reference file not specified. Use \"-r fileName\".\n");
          }
        }
        else if ((*i=="-l" || *i=="--level") && i+1!=vArgs.end()) {
          level = static_cast<u8>(stoi(*++i));
        }
        else if (*i=="-v" || *i=="--verbose") {
          verbose = true;
        }
        else if ((*i=="-n" || *i=="--nthr") && i+1!=vArgs.end()) {
          nthr = static_cast<u8>(stoi(*++i));
        }
      }
      
      // Mandatory args
      const bool has_t {std::find(vArgs.begin(),vArgs.end(),"-t")!=vArgs.end()};
      const bool has_tar
        {std::find(vArgs.begin(),vArgs.end(),"--tar")!=vArgs.end()};
      const bool has_r {std::find(vArgs.begin(),vArgs.end(),"-r")!=vArgs.end()};
      const bool has_ref
        {std::find(vArgs.begin(),vArgs.end(),"--ref")!=vArgs.end()};
      if (!has_t && !has_tar)
        throw std::runtime_error
          ("Error: target file not specified. Use \"-t fileName\".\n");
      else if (!has_r && !has_ref)
        throw std::runtime_error
          ("Error: reference file not specified. Use \"-r fileName\".\n");
    }
  }

 private:
  void checkFile (const string& s) const {    // Can be opened and not empty
    ifstream f(s);
    if (!f) {
      f.close();
      throw std::runtime_error
        ("Error: the file \"" + s + "\" cannot be opened or is empty.\n");
    }
    else {
      char c;
      bool foundChar {false};
      while (f.get(c) && !foundChar)
        if (c!=' ' && c!='\n' && c!='\t')
          foundChar = true;
      if (!foundChar)
        throw std::runtime_error("Error: the file \"" + s + "\" is empty.\n");
      f.close();
    }
  }
  
  void help () const {
    cerr                                                                 << '\n'
      << "NAME"                                                          << '\n'
      << "    Smash++ v" << VERSION << " - rearrangements finder"        << '\n'
                                                                         << '\n'
      << "AUTHOR"                                                        << '\n'
      << "    Morteza Hosseini    seyedmorteza@ua.pt"                    << '\n'
                                                                         << '\n'
      << "SYNOPSIS"                                                      << '\n'
      << "    ./smashpp [OPTION]...  -t [TAR_FILE] -r [REF_FILE]"        << '\n'
                                                                         << '\n'
  //  << "SAMPLE"                                                        << '\n'
  //  << "      Compress:    ./cryfa -k pass.txt in.fq > comp"           << '\n'
  //  << "      Decompress:  ./cryfa -k pass.txt -d comp > orig.fq"      << '\n'
  //                                                                     << '\n'
      << "DESCRIPTION"                                                   << '\n'
  //  << "      Compress and encrypt FASTA/FASTQ files."                 << '\n'
  //                                                                     << '\n'
  //  << "      The KEY_FILE specifies a file including the password."   << '\n'
  //                                                                     << '\n'
      << "    -t [FILE], --tar          target file    -- MANDATORY"     << '\n'
      << "    -r [FILE], --ref          reference file -- MANDATORY"     << '\n'
      << "    -l [NUM],  --level        level"                           << '\n'
      << "    -n [NUM],  --nthr         number of threads"               << '\n'
      << "    -v,        --verbose      more information"                << '\n'
      << "    -h,        --help         usage guide"                     << '\n'
                                                                         << '\n'
      << "COPYRIGHT"                                                     << '\n'
      << "    Copyright (C) "<< DEV_YEARS <<", IEETA, University of Aveiro.  \n"
      << "    You may redistribute copies of this Free software"         << '\n'
      << "    under the terms of the GNU - General Public License"       << '\n'
      << "    v3 <http://www.gnu.org/licenses/gpl.html>. There"          << '\n'
      << "    is NOT ANY WARRANTY, to the extent permitted by law."      <<endl;
    }
};

#endif //SMASHPP_PAR_HPP