//
// Created by morteza on 16-10-2018.
//
#include <iostream>
#include <fstream>

using namespace std;

int main (int argc, char* argv[])
{
  for (vector<char> buffer(FILE_BUF,0); cin;) {
    cin.read(buffer.data(), FILE_BUF);
    string out;
    for (auto it=buffer.begin(); it!=buffer.begin()+cin.gcount(); ++it) {
      if (*it!='N')  out+=*it;
//      const auto c = *it;
//      if (c!='N')  out+=c;
    }
    cout.write(out.data(), out.size());
  }
  return 0;
}