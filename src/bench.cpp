//
// Created by morteza on 11-04-2018.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <array>
#include <vector>
#include <numeric>
#include <limits>
#include <tgmath.h>

using std::string;
using std::to_string;
using std::vector;
using std::array;
using std::cerr;
using std::fstream;
using std::ifstream;
using std::ofstream;
using std::istringstream;
using u8  = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;
#define IGNORE_LINE(in) \
        (in).ignore(std::numeric_limits<std::streamsize>::max(), '\n')

// Global
static const string repName = "report";

class Gnuplot
{
public:
  Gnuplot () {
    gnuplotpipe = popen("gnuplot -persist", "w");
    if (!gnuplotpipe)
      cerr << "gnuplot not found!";
  }
  ~Gnuplot () {
    fprintf(gnuplotpipe, "exit\n");
    pclose(gnuplotpipe);
  }
  void operator<< (const string& cmd) {
    fprintf(gnuplotpipe, "%s\n", cmd.c_str());
    fflush(gnuplotpipe);
  }
  
private:
  FILE*  gnuplotpipe;
};

void run (const string& cmd) {
  if (std::system(cmd.c_str()) != 0)
    throw std::runtime_error("Error: failed to execute.");
}

void runGnuplot (const string& cmd) {
  string gnuplotCmd = "Gnuplot -p -e ";    // -p=-persist
  gnuplotCmd += "\"" + cmd + "\"";
  if (std::system(gnuplotCmd.c_str()) != 0)
    throw std::runtime_error("Error: failed to execute.");
}

string exec (const char* cmd) {
  const u32 bufSize = 128;
  array<char, bufSize> buffer{};
  string result;
  std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
  if (!pipe)
    throw std::runtime_error("Error: popen() failed!");
  while (!feof(pipe.get())) {
    if (fgets(buffer.data(), bufSize, pipe.get()) != nullptr)
      result += buffer.data();
  }
  return result;
}

void memUsage (u8 k) {//todo

}

string combine (u8 ir, u8 k, float alpha) {
  return to_string(ir) + "," + to_string(k) + "," + to_string(alpha);
}

string combine (u8 ir, u8 k, float alpha, u64 w, u8 d) {
  return to_string(ir) + "," + to_string(k) + "," + to_string(alpha) +
         "," + to_string(w) + "," + to_string(d);
}

void plot () {
//  ifstream ifs(repName);
  ifstream ifs("rep");
  vector<u16> vir{};      vector<u16> vk{};      vector<float> valpha{};
  vector<u64> vw{};       vector<u16> vd{};      vector<double> vent{};
  vector<string> vtar{};  vector<string> vref{};

  IGNORE_LINE(ifs);  // Ignore the header line
  for (string line; getline(ifs, line);) {
    u16 ir, k;  float alpha;  u64 lg2w;  u16 d;  double ent;  string tar, ref;
    istringstream ss(line);
    ss >> tar >> ref >> ir >> k >> alpha >> lg2w >> d >> ent;
    vir.emplace_back(ir);   vk.emplace_back(k);     valpha.emplace_back(alpha);
    lg2w==0 ? vw.emplace_back(0) : vw.emplace_back(std::pow(2,lg2w));
    vd.emplace_back(d);     vent.emplace_back(ent); vtar.emplace_back(tar);
    vref.emplace_back(ref);
  }
  
  Gnuplot gp;
  gp << "set terminal pdfcairo size 10,2.5";
  gp << "set output \"plot.pdf\"";
  gp << "set multiplot layout 1, 2";
  gp << "set style line 1 lt 1 lw 2 pt 1 ps 0.5";
  gp << "set style line 2 lt 2 lw 2 pt 2 ps 0.5";
  gp << "set style line 3 lt 3 lw 2 pt 3 ps 0.5";
  gp << "set style line 4 lt 4 lw 2 pt 4 ps 0.5";
  gp << "set grid lc rgb \"black\"";
  gp << "set tic scale 0";
  gp << "set title \"Ref: A (100 MB), Tar: A (100 MB) -- mutation: 0%";
  gp << "set xlabel \"Context size (k)\"";
  gp << "set ylabel \"Average entropy (H)\"";
  gp << "set xtics \"1\"";
  gp << "set key reverse Left invert bottom left";
  gp << "set size 0.5,1";
//  gp << "set origin 0,0.55";
  gp << "plot '-' with linespoints ls 1 ps 0.75 title \"alpha=0.001\", \\";
  gp << "'-' with linespoints ls 2 ps 0.75 title \"alpha=0.01\", \\";
  gp << "'-' with linespoints ls 3 ps 0.75 title \"alpha=0.1\", \\";
  gp << "'-' with linespoints ls 4 title \"alpha=1\"";
  for (u8 i=0; i!=25; ++i)
    gp << to_string(vk[i]) + " " + to_string(vent[i]);
  gp << "e";
  for (u8 i=25; i!=2*25; ++i)
    gp << to_string(vk[i]) + " " + to_string(vent[i]);
  gp << "e";
  for (u8 i=2*25; i!=3*25; ++i)
    gp << to_string(vk[i]) + " " + to_string(vent[i]);
  gp << "e";
  for (u8 i=3*25; i!=4*25; ++i)
    gp << to_string(vk[i]) + " " + to_string(vent[i]);
  gp << "e";
//  gp << "unset key";
  gp << "unset title";
  gp << "set title \"Ref: A (100 MB), Tar: A4m (100 MB) -- mutation: 5%";
  gp << "set size 0.5,1";
////  gp << "set origin 0,0";
  gp << "plot '-' with linespoints ls 1 ps 0.75 title \"alpha=0.001\", \\";
  gp << "'-' with linespoints ls 2 ps 0.75 title \"alpha=0.01\", \\";
  gp << "'-' with linespoints ls 3 ps 0.75 title \"alpha=0.1\", \\";
  gp << "'-' with linespoints ls 4 title \"alpha=1\"";
  for (u8 i=4*25; i!=5*25; ++i)
    gp << to_string(vk[i]) + " " + to_string(vent[i]);
  gp << "e";
  for (u8 i=5*25; i!=6*25; ++i)
    gp << to_string(vk[i]) + " " + to_string(vent[i]);
  gp << "e";
  for (u8 i=6*25; i!=7*25; ++i)
    gp << to_string(vk[i]) + " " + to_string(vent[i]);
  gp << "e";
  for (u8 i=7*25; i!=8*25; ++i)
    gp << to_string(vk[i]) + " " + to_string(vent[i]);
  gp << "e";
  gp << "unset key";
  gp << "unset multiplot";

  
////  runGnuplot(cmd);
//
//  ifs.close();
}

void writeHeader (bool append) {
  ofstream f;
  if (append)  f.open(repName, ofstream::app);
  else         f.open(repName);
  f << "tar\tref\tir\tk\talpha\tlog2w\td\tH\n";
  f.close();
}


int main (int argc, char* argv[])
{
  try {
//    bool writeHdr= false, execute= false, plt= true;
//    bool writeHdr= false, execute= true, plt= false;
    bool writeHdr= true, execute= true, plt= false;
  
//    vector<u8>    vk;    vk.resize(14);    std::iota(vk.begin(),vk.end(),1);
    constexpr bool bVerbose {false};
    static string verbose="";  if (bVerbose) verbose=" -v";
    constexpr bool bRep {true};
    static string report="";  if (bRep) report=" -R "+repName;
    static const vector<string> vRef {"A"};
    static const vector<string> vTar {"A5m","A4m"};
//    static const vector<string> vTar {"A5m","A4m","A3m","A2m","A1m"};
//    static std::vector<std::vector<u8>> level;
//    static const vector<string> vLevel  {"0", "1"};
  
    if (writeHdr) {
      writeHeader(false);
    }
    if (execute) {
      run("./smashpp -r A -t A -m 0,7,0.001:0,12,0.001:0,14,0.001:0,20,0.001,31,5 -R report");
      
//      for (const auto &ir : {0, 1}) {
//        for (u8 rIdx = 0; rIdx!=vRef.size(); ++rIdx) {
//          for (u8 tIdx = 0; tIdx!=vTar.size(); ++tIdx) {
//            for (const auto &a : {0.001, 0.01, 0.1, 1.0}) {
//              for (const auto &k : {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}) {
//                string model{combine(ir, k, a)};
////            cerr << model<<'\n';
//                run("./smashpp"
//                    " -r "+vRef[rIdx]+
//                    " -t "+vTar[tIdx]+
//                    " -m "+model+
//                    //            " -l " + vLevel[lIdx] +
//                    report+
//                    verbose
//                );
//              }
//              for (const auto &k : {15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25}) {
//                for (const auto &w : {31}) {
//                  for (const auto &d : {5}) {
//                    string model{combine(ir, k, a, w, d)};
////                  cerr << model<<'\n';
//                    run("./smashpp"
//                        " -r "+vRef[rIdx]+
//                        " -t "+vTar[tIdx]+
//                        " -m "+model+
//                        report+
//                        verbose
//                    );
//                  }
//                }
//              }
//            }
//          }
//        }
//      }
    }
    if (plt) {// Plot results
      plot();
    }
  }
  catch (std::exception& e) { cerr << e.what(); }
  
  return 0;
}