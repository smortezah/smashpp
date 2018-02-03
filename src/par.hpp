//
// Created by morteza on 02-02-2018.
//

#ifndef SMASHPP_PAR_HPP
#define SMASHPP_PAR_HPP

#include <vector>
#include <algorithm>
#include "def.hpp"
using std::cerr;
using std::vector;
using std::string;

// Command line
class Parameters
{
public:
    string tar;
    string ref;
    bool   verbose;
    u8     nthr;
    
    Parameters () : verbose(false), nthr(DEF_THR) {};
    inline void parse (int&, char**&);

private:
    inline void help ();
};


/*
 * Parse
 */
void Parameters::parse (int& argc, char**& argv)
{
    if (argc<2) { help();  return; }
    else {
        vector<string> vArgs;    vArgs.reserve((u64) argc);
        for (int i=0; i!=argc; ++i)  vArgs.emplace_back(string(argv[i]));
        
        for (auto i=vArgs.begin(); i!=vArgs.end(); ++i) {
            if      (*i=="-h" || *i=="--help") { help();  return; }
            else if (*i=="-t" || *i=="--tar") {
                if (i+1!=vArgs.end())
                    tar = *++i;
                else
                    cerr << "Please specify the target file address with "
                         << "\"-t fileName\".";
            }
            else if (*i=="-r" || *i=="--ref") {
                if (i+1!=vArgs.end())
                    ref = *++i;
                else
                    cerr << "Please specify the reference file address with "
                         << "\"-r fileName\".";
            }
            else if (*i=="-v" || *i=="--verbose")
                verbose = true;
            else if ((*i=="-n" || *i=="--nthreads") && i+1!=vArgs.end())
                nthr = (u8) stoul(*++i);
        }
        
        // Mandatory args
        if (std::find(vArgs.begin(), vArgs.end(), "-t") == vArgs.end()
            || std::find(vArgs.begin(), vArgs.end(), "--tar") == vArgs.end())
            cerr << "Please specify the target file address with "
                 << "\"-t fileName\".";
        else if (std::find(vArgs.begin(), vArgs.end(), "-r") == vArgs.end()
                 || std::find(vArgs.begin(), vArgs.end(), "--ref") == vArgs.end())
            cerr << "Please specify the reference file address with "
                 << "\"-r fileName\".";
    }
}

/*
 * Usage help
 */
inline void Parameters::help ()
{
    cerr                                                                 << '\n'
        << "NAME"                                                        << '\n'
        << "      Smash++ v" << VERSION << " - rearrangements finder"    << '\n'
                                                                         << '\n'
        << "AUTHORS"                                                     << '\n'
        << "      Morteza Hosseini    seyedmorteza@ua.pt"                << '\n'
        << "      Diogo Pratas        pratas@ua.pt"                      << '\n'
        << "      Armando J. Pinho    ap@ua.pt"                          << '\n'
                                                                         << '\n'
        << "SYNOPSIS"                                                    << '\n'
//        << "      ./smashpp [OPTION]...  [-d] [IN_FILE] > [OUT_FILE]"    << '\n'
//                                                                         << '\n'
//        << "SAMPLE"                                                      << '\n'
//        << "      Compress:    ./cryfa -k pass.txt in.fq > comp"         << '\n'
//        << "      Decompress:  ./cryfa -k pass.txt -d comp > orig.fq"    << '\n'
//                                                                         << '\n'
//        << "DESCRIPTION"                                                 << '\n'
//        << "      Compress and encrypt FASTA/FASTQ files."               << '\n'
//        << "      Shuffle and encrypt any other text-based files."       << '\n'
//                                                                         << '\n'
//        << "      The KEY_FILE specifies a file including the password." << '\n'
//                                                                         << '\n'
//        << "      -h,  --help"                                           << '\n'
//        << "           usage help"                                      << '\n'
//                                                                         << '\n'
//        << "      -d,  --dec"                                            << '\n'
//        << "           decompress & decrypt"                             << '\n'
//                                                                         << '\n'
//        << "      -v,  --verbose"                                        << '\n'
//        << "           verbose mode (more information)"                  << '\n'
//                                                                         << '\n'
//        << "      -t [NUMBER],  --thread [NUMBER]"                       << '\n'
//        << "           number of threads"                                << '\n'
                                                                         << '\n'
        << "COPYRIGHT"                                                   << '\n'
        << "      Copyright (C) " << DEV_YEARS
                                  << ", IEETA, University of Aveiro."    << '\n'
        << "      You may redistribute copies of this Free software"     << '\n'
        << "      under the terms of the GNU - General Public License v3"<< '\n'
        << "      <http://www.gnu.org/licenses/gpl.html>. There is NOT"  << '\n'
        << "      ANY WARRANTY, to the extent permitted by law."         <<'\n';
}

#endif //SMASHPP_PAR_HPP