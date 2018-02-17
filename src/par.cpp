//
// Created by morteza on 17-02-2018.
//

#include "par.hpp"

/*
 * Constructor
 */
Param::Param ()
{
  tar     = "";
  ref     = "";
  ir      = false;
  k       = 10;
  alpha   = 0.01;
  verbose = false;
  nthr    = DEF_THR;
};

/*
 * Parse
 */
void Param::parse (int argc, char**& argv)
{
  if (argc<2) { help();    throw EXIT_SUCCESS; }
  else {
    vector<string> vArgs;        vArgs.reserve(static_cast<u64>(argc));
    for (int i=0; i!=argc; ++i)
      vArgs.emplace_back(static_cast<string>(argv[i]));

    for (auto i=vArgs.begin(); i!=vArgs.end(); ++i) {
      if (*i=="-h" || *i=="--help") { help();    throw EXIT_SUCCESS; }
      else if (*i=="-t" || *i=="--tar") {
        if (i+1!=vArgs.end()) {
          tar = *++i;
          checkFile(tar);
        }
        else {
          cerr << "Error: target file address not specified. Use "
               << "\"-t fileName\".\n";
          throw EXIT_FAILURE;
        }
      }
      else if (*i=="-r" || *i=="--ref") {
        if (i+1!=vArgs.end()) {
          ref = *++i;
          checkFile(ref);
        }
        else {
          cerr << "Error: reference file address not specified. Use "
               << "\"-r fileName\".\n";
          throw EXIT_FAILURE;
        }
      }
      else if ((*i=="-m" || *i=="--model") && i+1!=vArgs.end()) {
        setModelPar(*++i);
      }
      else if (*i=="-v" || *i=="--verbose") {
        verbose = true;
      }
      else if ((*i=="-n" || *i=="--nthreads") && i+1!=vArgs.end()) {
        nthr = static_cast<u8>(stoul(*++i));
      }
    }
    
    // Mandatory args
    bool tExist   = std::find(vArgs.begin(), vArgs.end(), "-t")   !=vArgs.end();
    bool tarExist = std::find(vArgs.begin(), vArgs.end(), "--tar")!=vArgs.end();
    bool rExist   = std::find(vArgs.begin(), vArgs.end(), "-r")   !=vArgs.end();
    bool refExist = std::find(vArgs.begin(), vArgs.end(), "--ref")!=vArgs.end();
    
    if (!tExist && !tarExist) {
      cerr << "Error: target file address not specified. Use "
           << "\"-t fileName\".\n";
      throw EXIT_FAILURE;
    }
    else if (!rExist && !refExist) {
      cerr << "Error: reference file address not specified. Use "
           << "\"-r fileName\".\n";
      throw EXIT_FAILURE;
    }
  }
}

/*
 * Chack if files can be opened and are not empty
 */
void Param::checkFile (const string& s) const
{
  ifstream f(s);
  
  if (!f) {
    f.close();
    cerr << "Error: the file \"" << s << "\" cannot be opened or is empty.\n";
    throw EXIT_FAILURE;
  }
  else {
    char c;
    bool foundChar = false;
    while (f.get(c) && !foundChar)
      if (c!=' ' && c!='\n' && c!='\t')
        foundChar = true;
  
    if (!foundChar) {
      cerr << "Error: the file \"" << s << "\" is empty.\n";
      throw EXIT_FAILURE;
    }
    f.close();
  }
}

/*
 * Set models parameters
 */
void Param::setModelPar (const string& m)
{
  auto beg = m.begin();
  vector<string> mPar;    mPar.reserve(3);
  
  for (auto j=beg; j!=m.end(); ++j) {
    if (*j==',') {
      mPar.emplace_back(string(beg, j));
      beg = j+1;
    }
  }
  mPar.emplace_back(string(beg, m.end()));
  
  ir    = static_cast<bool>(stoi(mPar[0]));
  k     = static_cast<u8>(stoi(mPar[1]));
  alpha = stof(mPar[2]);
}

/*
 * Usage guide
 */
inline void Param::help () const
{
  cerr                                                                   << '\n'
    << "NAME"                                                            << '\n'
    << "    Smash++ v" << VERSION << " - rearrangements finder"          << '\n'
                                                                         << '\n'
    << "AUTHORS"                                                         << '\n'
    << "    Morteza Hosseini    seyedmorteza@ua.pt"                      << '\n'
    << "    Diogo Pratas        pratas@ua.pt"                            << '\n'
    << "    Armando J. Pinho    ap@ua.pt"                                << '\n'
                                                                         << '\n'
    << "SYNOPSIS"                                                        << '\n'
    << "    ./smashpp [OPTION]...  -t [TAR_FILE] -r [REF_FILE]"          << '\n'
                                                                         << '\n'
//  << "SAMPLE"                                                          << '\n'
//  << "      Compress:    ./cryfa -k pass.txt in.fq > comp"             << '\n'
//  << "      Decompress:  ./cryfa -k pass.txt -d comp > orig.fq"        << '\n'
//                                                                       << '\n'
    << "DESCRIPTION"                                                     << '\n'
//  << "      Compress and encrypt FASTA/FASTQ files."                   << '\n'
//                                                                       << '\n'
//  << "      The KEY_FILE specifies a file including the password."     << '\n'
//                                                                       << '\n'
    << "    -t [FILE], --tar          target file    -- MANDATORY"       << '\n'
    << "    -r [FILE], --ref          reference file -- MANDATORY"       << '\n'
    << "    -h,        --help         usage guide"                       << '\n'
    << "    -v,        --verbose      more information"                  << '\n'
    << "    -t [NUM],  --thread       number of threads"                 << '\n'
                                                                         << '\n'
    << "COPYRIGHT"                                                       << '\n'
    << "    Copyright (C) "<< DEV_YEARS <<", IEETA, University of Aveiro."<<'\n'
    << "    You may redistribute copies of this Free software"           << '\n'
    << "    under the terms of the GNU - General Public License"         << '\n'
    << "    v3 <http://www.gnu.org/licenses/gpl.html>. There"            << '\n'
    << "    is NOT ANY WARRANTY, to the extent permitted by law."        <<endl;
}