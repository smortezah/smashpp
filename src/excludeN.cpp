//
// Created by morteza on 16-10-2018.
//
#include <iostream>
#include <fstream>

using namespace std;

int main (int argc, char* argv[])
{
  for (char c; cin.get(c);) {
    if (c != 'N')
      cout << c;
  }

  return 0;
}