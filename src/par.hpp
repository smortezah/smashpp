//
// Created by morteza on 02-02-2018.
//

#ifndef SMASHPP_PAR_HPP
#define SMASHPP_PAR_HPP

#include <fstream>
#include <vector>
#include <algorithm>
#include "def.hpp"
using std::cerr;
using std::endl;
using std::vector;
using std::string;
using std::stoi;
using std::stof;
using std::ifstream;

class Param
{
 public:
  string tar;
  string ref;
  bool   ir;         // Inverted repeat
  u8     k;          // Context-order size
  float  alpha;
  bool   verbose;
  u8     nthr;
  
  Param      ();     // Define Param::Param(){} in *.hpp => compile error
  void parse (int, char**&);

 private:
  void checkFile   (const string&)  const;
  void setModelPar (const string&);
  void help        ()               const;
};

#endif //SMASHPP_PAR_HPP