//
// Created by morteza on 11-04-2018.
//
#include <iostream>
#include <memory>
#include <array>
using std::string;
using std::array;
using std::cerr;
typedef unsigned int  u32;

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
//  std::system("ls -lh");
  cerr << exec("./smashpp -t A -r A -l 0");
  cerr << exec("./smashpp -t A -r A -l 1");
  std::system("./smashpp -t A -r A -l 2");
  popen("./smashpp -t A -r A -l 0", "r");
  
  return 0;
}