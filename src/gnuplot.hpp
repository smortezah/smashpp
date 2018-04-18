//
// Created by morteza on 18-04-2018.
//

#ifndef PROJECT_GNUPLOT_HPP
#define PROJECT_GNUPLOT_HPP

using std::string;

class Gnuplot
{
 public:
  Gnuplot  ();
  ~Gnuplot ();
//  void config (const Gnuplot&);
  void operator<< (const std::string&);

 private:
  FILE*  gnuplotpipe;
  string terminal;
  string output;
  string title;
  string xlabel;
  string ylabel;
  string xtics;
};

#endif //PROJECT_GNUPLOT_HPP