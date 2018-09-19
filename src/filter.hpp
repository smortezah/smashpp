//
// Created by morteza on 19-09-2018.
//

#ifndef PROJECT_FILTER_HPP
#define PROJECT_FILTER_HPP

#include "par.hpp"

class Filter {
 public:
//  Filter          () = default;
  explicit Filter (const Param&);
  void smooth     ();

 private:
  u32           wsize;
  WType         wtype;
  vector<float> window;

  void config       (const Param&);
  void config_wtype (const string&);
  void make_window  ();
  void hamming      ();
  void hann         ();
  void blackman     ();
  void triangular   ();  // Bartlett window
  void welch        ();
};

#endif //PROJECT_FILTER_HPP