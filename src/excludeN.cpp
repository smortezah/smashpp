//
// Created by morteza on 16-10-2018.
//
#include <iostream>
#include <fstream>
#include "def.hpp"
using namespace std;

int main (int argc, char* argv[])
{
  for (vector<char> buffer(FILE_BUF,0); cin;) {
    cin.read(buffer.data(), FILE_BUF);
    string out;
    for (auto it=buffer.begin(); it!=buffer.begin()+cin.gcount(); ++it) {
      if (*it!='N')  out+=*it;
    }
    cout.write(out.data(), out.size());
  }
  return 0;
}