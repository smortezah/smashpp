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
using std::string;
using std::to_string;
using std::vector;
using std::array;
using std::cerr;
using std::fstream;
using std::ifstream;
using std::ofstream;
using std::istringstream;
typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;
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
  FILE* gnuplotpipe;
};


struct PlotPar {
  PlotPar () : terminal("pdfcairo"), output("plot.pdf") {}
  
  string terminal;
  string output;
  
  string data;
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
  return to_string(ir)+","+to_string(k)+","+to_string(alpha);
}

string makeCmd (const string& keywords, const string& myCmd) {
  return keywords + " " + myCmd + "\n";
}

string makeCmdQuote (const string& keywords, const string& myCmd) {
  return keywords + " \"" + myCmd + "\"\n";
}

void plot (
//  const PlotPar& p
) {
//  ifstream ifs(repName);
//  vector<u16>    vir    {};
//  vector<u16>    vk     {};
//  vector<float>  valpha {};
//  vector<double> vent   {};
//  vector<string> vtar   {};
//  vector<string> vref   {};
//
//  IGNORE_LINE(ifs);  // Ignore the header line
//  for (string line; getline(ifs, line);) {
//    u16 ir;  u16 k;  float alpha;  double ent;  string tar;  string ref;
//
//    istringstream ss(line);
//    ss >> tar >> ref >> ir >> k >> alpha >> ent;
//
//    vir.emplace_back(ir);   vk.emplace_back(k);     valpha.emplace_back(alpha);
//    vent.emplace_back(ent); vtar.emplace_back(tar); vref.emplace_back(ref);
//  }
//
  string cmd;
//  cmd+=makeCmd("set terminal", p.terminal);
//  cmd+=makeCmdQuote("set output", p.output);
//  cmd+=makeCmdQuote("set title", title);
//  cmd+=makeCmdQuote("set xlabel", xlabel);
//  cmd+=makeCmdQuote("set ylabel", ylabel);
//  cmd+=makeCmdQuote("set xtics", xtics);
  
  cmd+=makeCmd("set terminal", "pdfcairo");
  cmd+=makeCmdQuote("set output", "plot.pdf");
//  cmd+=makeCmdQuote("set title", title);
  cmd+=makeCmdQuote("set xlabel", "Context size (k)");
  cmd+=makeCmdQuote("set ylabel", "Average entropy (H)");
  cmd+=makeCmdQuote("set xtics", "1");
  cmd+="set key off\n";
  cmd+= "plot [5:21]'-' with lines, '-', '-', '-', '-', '-', '-'\n";
//  for (int i = 0; i<14; i += 1)
//    cmd += to_string(vk[i])+" "+to_string(vent[i])+"\n";
//  cmd+="e\n";
//  for (int i = 0; i<14; i += 1)
//    cmd += to_string(vk[i])+" "+to_string(valpha[i])+"\n";
  cmd+="5 2.000\n";
  cmd+="10 1.978\n";
  cmd+="12 1.577\n";
  cmd+="13 0.807\n";
  cmd+="14 0.266\n";
  cmd+="e\n";
  cmd+="20 2.678\n";
  cmd+="e\n";
  cmd+="20 2.403\n";
  cmd+="e\n";
  cmd+="20 2.279\n";
  cmd+="e\n";
  cmd+="20 2.195\n";
  cmd+="e\n";
  cmd+="20 2.084\n";
  cmd+="e\n";
  cmd+="20 2.056\n";
  cmd+="e\n";
  cerr<<cmd;
  Gnuplot gp;
  gp << cmd;

////  runGnuplot(cmd);
//
//  ifs.close();
}

void writeHeader (bool append) {
  ofstream f;
  if (append)  f.open(repName, ofstream::app);
  else         f.open(repName);
  f << "tar\tref\tir\tk\talpha\tw\td\tH\n";
  f.close();
}


int main (int argc, char* argv[])
{
  try {bool writeHdr= false, execute= false, plt= true;
  
//  static std::vector<std::vector<u8>> level;
    vector<u8>    vir    {0};
//    vector<u8>    vk     {1, 2, 3, 4, 5};
    vector<u8>    vk;    vk.resize(14);    std::iota(vk.begin(),vk.end(),1);
    vector<float> valpha {1};
//    vector<u8>    vir    {0, 1};
//    vector<u8>    vk;    vk.resize(14);    std::iota(vk.begin(),vk.end(),1);
//    vector<float> valpha {1, 0.1, 0.01, 0.001};
    constexpr bool bVerbose {false};
    static string verbose="";  if (bVerbose) verbose=" -v";
    constexpr bool bRep {true};
    static string report="";  if (bRep) report=" -R "+repName;
    static const vector<string> vTar    {"A"};
    static const vector<string> vRef    {"A"};
//    static const vector<string> vLevel  {"0", "1"};
//    static const string model
////      {"0,14,0.001"};
////      {"0,12,0.6:1,14,0.9:0,5,0.2:1,17,0.4,20,5"};
////    {"0,12,0.6:0,5,0.2:1,17,0.4,20,5:1,14,0.9"};

    
    if (writeHdr) {
    writeHeader(false);
    }
    if (execute) {
    for (u8 tIdx=0; tIdx!=vTar.size(); ++tIdx) {
//      for (u8 lIdx=0; lIdx!=vLevel.size(); ++lIdx)
      for (const auto& ir : vir)
        for (const auto& k : vk)
          for (const auto& a : valpha) {
            string model {combine(ir, k, a)};

//            cerr << model<<'\n';
        run("./smashpp"
            " -t " + vTar[tIdx] +
            " -r " + vRef[tIdx] +
            " -m " + model +
//            " -l " + vLevel[lIdx] +
            report +
            verbose
        );
          }
    }
    }
    if (plt) {// Plot results
      
      plot(
//        "pdfcairo",
//           "plot.pdf",
//           "Average entropy VS. context size",
//           "Context size (k)",
//           "Average entropy (H)",
//           "1"
      );
    }
  }
  catch (std::exception& e) { cerr << e.what(); }
  
  return 0;
}