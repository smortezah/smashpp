//
// Created by morteza on 02-02-2018.
//

#ifndef SMASHPP_PAR_HPP
#define SMASHPP_PAR_HPP

//#include <cstring>
#include <vector>
#include <algorithm>
#include "def.hpp"

class Parameters    // Command line
{
public:
    string tar;
    string ref;
    bool   verbose;
    u8     nthr;
    bool   help;
    
    Parameters () : verbose(false), nthr(DEF_THR), help(false) {};
    inline void parse (int&, char**&);

private:
    inline std::vector<string>::iterator where (std::vector<string>, string);
    inline bool isIn (std::vector<string>, const string &);
    inline void guide ();
};


/*
 * Parse
 */
void Parameters::parse (int& argc, char**& argv)
{
    if (argc<2) { guide();  return; }
    else {
        std::vector<string> vArgs;   vArgs.reserve((u64) argc);
        for (int i=0; i!=argc; ++i)  vArgs.emplace_back(string(argv[i]));
        
//        auto posShort = std::find(vArgs.begin(), vArgs.end(), "-h");
//        auto posLong  = std::find(vArgs.begin(), vArgs.end(), "--help");
//        if (posShort!=vArgs.end() || posLong!=vArgs.end()) {
//            guide();
//            return;
//        }
////        auto posShort=where(vArgs, "-h");
////        auto posLong =where(vArgs, "--help");
////        if (posShort!=vArgs.end() || posLong!=vArgs.end())
////        {
//////            cout << *posShort;
//////            cout << *posLong;
////        }
        for (auto i=vArgs.begin(); i!=vArgs.end(); ++i)
        {
            if (*i=="-h" || *i=="--help")
            {
                guide();
                return;
            }
            else if (*i=="-v" || *i=="--verbose")
                verbose=true;
        }
        
//        for (int i=0; i!=argc; ++i) {
//            if (string(argv[i])=="-h" || string(argv[i])=="--help") {
//                guide();
//                return;
//            }
//            else if (string(argv[i])=="-t" || string(argv[i])=="--tar")
//            {
//                if (i+1!=argc)  tar=string(argv[i+1]);
//                else {
//                    cout << "Please specify the target file address with "
//                         << "\"-t fileName\".";
//                    return;
//                }
//            }
//            else if (string(argv[i])=="-r" || string(argv[i])=="--ref") {
//                if (i+1!=argc)  ref=string(argv[i+1]);
//                else {
//                    cout << "Please specify the reference file address with "
//                         << "\"-r fileName\".";
//                    return;
//                }
//            }
//            else if (string(argv[i])=="-v" || string(argv[i])=="--verbose")
//                verbose = true;
//            else if ((string(argv[i])=="-n" || string(argv[i])=="--nthreads") && i+1!=argc)
//                nthr = (u8) stoi(string(argv[i+1]));
//        }
    }
}

inline bool Parameters::isIn (std::vector<string> v, const string &s)
{
    return std::find(v.begin(), v.end(), s) != v.end();
}

inline std::vector<string>::iterator Parameters::where ( std::vector<string> v, string s)
{
    return std::find(v.begin(), v.end(), s);
}


/*
 * Usage guide
 */
inline void Parameters::guide ()
{
    cout                                                                 << '\n'
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
//        << "           usage guide"                                      << '\n'
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
        << "      ANY WARRANTY, to the extent permitted by law."         << '\n';
}

#endif //SMASHPP_PAR_HPP