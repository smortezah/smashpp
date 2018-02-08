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
using std::stoi;
using std::stof;

class Parameters
{
public:
    string tar;
    string ref;
    bool   ir;   // Inverted repeat
    u8     k;    // Context-order size
    float  alpha;
    bool   verbose;
    u8     nthr;

    Parameters () {    // Parameters::Parameters(){} in *.hpp => compile error
        ir      = false;
        k       = 10;
        alpha   = 0.01;
        verbose = false;
        nthr    = DEF_THR;
    };
    void parse (int, char**&);

private:
    void help () const;
};


/*
 * Parse
 */
inline void Parameters::parse (int argc, char**& argv)
{
    if (argc<2) {
        help();    return;
    }
    else {
        vector<string> vArgs;    vArgs.reserve((u64) argc);
        for (int i=0; i!=argc; ++i)  vArgs.emplace_back(string(argv[i]));
        
        for (auto i=vArgs.begin(); i!=vArgs.end(); ++i) {
            if (*i=="-h" || *i=="--help") {
                help();    return;
            }
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
            else if ((*i=="-m" || *i=="--model") && i+1!=vArgs.end()) {
                string m = *++i;
                auto beg = m.begin();
                vector<string> mPar;    mPar.reserve(3);
                
                for (auto j=beg; j!=m.end(); ++j) {
                    if (*j==',') {
                        mPar.emplace_back(string(beg, j));
                        beg = j+1;
                    }
                }
                mPar.emplace_back(string(beg, m.end()));
                
                ir    = (bool) stoi(mPar[0]);
                k     = (u8)   stoi(mPar[1]);
                alpha =        stof(mPar[2]);
            }
            else if (*i=="-v" || *i=="--verbose")
                verbose = true;
            else if ((*i=="-n" || *i=="--nthreads") && i+1!=vArgs.end())
                nthr = (u8) stoul(*++i);
        }
        
        // Mandatory args
        bool tExist =
                std::find(vArgs.begin(), vArgs.end(), "-t")    != vArgs.end();
        bool tarExist =
                std::find(vArgs.begin(), vArgs.end(), "--tar") != vArgs.end();
        bool rExist =
                std::find(vArgs.begin(), vArgs.end(), "-r")    != vArgs.end();
        bool refExist =
                std::find(vArgs.begin(), vArgs.end(), "--ref") != vArgs.end();
        
        if (!tExist && !tarExist)
            cerr << "Please specify the target file address, with "
                 << "\"-t fileName\".";
        else if (!rExist && !refExist)
            cerr << "Please specify the reference file address, with "
                 << "\"-r fileName\".";
    }
}

/*
 * Usage guide
 */
inline void Parameters::help () const
{
    cerr                                                                 << '\n'
        << "NAME"                                                        << '\n'
        << "    Smash++ v" << VERSION << " - rearrangements finder"      << '\n'
                                                                         << '\n'
        << "AUTHORS"                                                     << '\n'
        << "    Morteza Hosseini    seyedmorteza@ua.pt"                  << '\n'
        << "    Diogo Pratas        pratas@ua.pt"                        << '\n'
        << "    Armando J. Pinho    ap@ua.pt"                            << '\n'
                                                                         << '\n'
        << "SYNOPSIS"                                                    << '\n'
        << "    ./smashpp [OPTION]...  -t [TAR_FILE] -r [REF_FILE]"      << '\n'
                                                                         << '\n'
//        << "SAMPLE"                                                      << '\n'
//        << "      Compress:    ./cryfa -k pass.txt in.fq > comp"         << '\n'
//        << "      Decompress:  ./cryfa -k pass.txt -d comp > orig.fq"    << '\n'
//                                                                         << '\n'
        << "DESCRIPTION"                                                 << '\n'
//        << "      Compress and encrypt FASTA/FASTQ files."               << '\n'
//                                                                         << '\n'
//        << "      The KEY_FILE specifies a file including the password." << '\n'
//                                                                         << '\n'
        << "    -t [FILE], --tar          target file    -- MANDATORY"   << '\n'
        << "    -r [FILE], --ref          reference file -- MANDATORY"   << '\n'
        << "    -h,        --help         usage guide"                   << '\n'
        << "    -v,        --verbose      more information"              << '\n'
        << "    -t [NUM],  --thread       number of threads"             << '\n'
                                                                         << '\n'
        << "COPYRIGHT"                                                   << '\n'
        << "    Copyright (C) "<< DEV_YEARS <<", IEETA, University of Aveiro.\n"
        << "    You may redistribute copies of this Free software"       << '\n'
        << "    under the terms of the GNU - General Public License"     << '\n'
        << "    v3 <http://www.gnu.org/licenses/gpl.html>. There"        << '\n'
        << "    is NOT ANY WARRANTY, to the extent permitted by law."    <<'\n';
}

#endif //SMASHPP_PAR_HPP