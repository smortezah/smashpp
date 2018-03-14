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

class Param    // Parameters
{
 public:
  string tar;
  string ref;
  vector<bool>  ir;     // Inverted repeat
  vector<u8>    k;      // Context-order size
  vector<float> alpha;
  bool verbose;
  u8   nthr;
  u8   nMdl;
  char mode;            // Table or Hash table
  u8   level;
  
  Param      ();        // Define Param::Param(){} in *.hpp => compile error
  void parse (int, char**&);

 private:
  void checkFile   (const string&)  const;    // Can be opened and not empty
//  void setModelPar (const string&);           // Set models parameters
  void help        ()               const;
};

#endif //SMASHPP_PAR_HPP