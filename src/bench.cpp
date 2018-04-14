//
// Created by morteza on 11-04-2018.
//

#include <iostream>
#include <fstream>
#include <memory>
#include <array>
#include <vector>
#include <numeric>
using std::string;
using std::to_string;
using std::vector;
using std::array;
using std::cerr;
using std::fstream;
using std::ifstream;
using std::ofstream;
typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;

// Global
static const string repName = "report";


void run (const string& cmd) {
  if (std::system(cmd.c_str()) != 0)
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

void loadReport () {
  ifstream ifs(repName);
  vector<u8>     vir    {};
  vector<u8>     vk     {};
  vector<float>  valpha {};
  vector<double> vent   {};
  string tar;
  string ref;
  
  u8     ir;
  u8     k;
  float  alpha;
  double ent;
  
  
  
  ifs.close();
}

void plot () {
  loadReport();
}


int main (int argc, char* argv[])
{
  try {
//  static std::vector<std::vector<u8>> level;
    vector<u8>    vir    {0, 1};
    vector<u8>    vk     {1, 2};
    vector<float> valpha {1, 0.1};
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

//    ofstream fs("report");
//    fs << "ir\tk\talpha\tH\ttar\tref\n";
//    fs.close();
    
    for (u8 tIdx=0; tIdx!=vTar.size(); ++tIdx) {
//      for (u8 lIdx=0; lIdx!=vLevel.size(); ++lIdx)
      for (const auto& ir : vir)
        for (const auto& k : vk)
          for (const auto& a : valpha) {
            string model {combine(ir, k, a)};

            cerr << model<<'\n';
        run("./smashpp"
            " -t " + vTar[tIdx] +
            " -r " + vRef[tIdx] +
            " -m " + model +
//            " -l " + vLevel[lIdx] +
            verbose +
            report
        );
          }
    }
    
    // Plot results
    plot();
  }
  catch (std::exception& e) { cerr << e.what(); }
  
  return 0;
}