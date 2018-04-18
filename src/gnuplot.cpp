//
// Created by morteza on 18-04-2018.
//

#include <iostream>
#include "gnuplot.hpp"
using std::string;
using std::cerr;

Gnuplot::Gnuplot () {
  terminal = "pdfcairo";
  
  gnuplotpipe = popen("gnuplot -persist", "w");
  if (!gnuplotpipe)
    cerr << "gnuplot not found!";
}

Gnuplot::~Gnuplot () {
  fprintf(gnuplotpipe, "exit\n");
  pclose(gnuplotpipe);
}

void Gnuplot::operator<< (const string& cmd) {
  fprintf(gnuplotpipe, "%s\n", cmd.c_str());
  fflush(gnuplotpipe);
}