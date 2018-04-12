//
// Created by morteza on 11-04-2018.
//
#include <iostream>
#include <memory>
#include <array>
#include <vector>

using std::string;
using std::to_string;
using std::vector;
using std::array;
using std::cerr;
typedef unsigned char  u8;
typedef unsigned int  u32;

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


int main (int argc, char* argv[])
{
  try {
    constexpr bool           bV {true};//false
    string      verbose = [](string s=""){ return bV ? (s="-v") : s;};
    static const vector<string> vTar    {"A"};
    static const vector<string> vRef    {"A"};
    static const vector<string> vLevel  {"0"
//                           , "1"
    };

    for (u8 tIdx=0; tIdx!=vTar.size(); ++tIdx) {
      for (u8 lIdx=0; lIdx!=vLevel.size(); ++lIdx)
        run("./smashpp"
            " -t " + vTar[tIdx] +
            " -r " + vRef[tIdx] +
            " -l " + vLevel[lIdx] +
              verbose);
    }
  }
  catch (std::exception& e) { cerr << e.what(); }
  
  return 0;
}