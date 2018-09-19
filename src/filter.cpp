//
// Created by morteza on 19-09-2018.
//
#include <cmath>
#include "filter.hpp"

Filter::Filter (const Param& p) {
  config(p);
}

inline void Filter::config (const Param& p) {
  wsize = p.wsize;
  wtype = p.wtype;
  window.reserve(wsize);
}

void Filter::smooth () {
  make_window();
}

inline void Filter::make_window () {
  if (wtype=="0" || wtype=="hamming") {
    hamming();
  }
  else if (wtype == "1" || wtype=="hann") {
  }
  else if (wtype == "2" || wtype=="blackman") {
  }
}

inline void Filter::hamming () {
  const auto cosNum = 2*PI;
  const auto cosDen = wsize - 1;
  for (auto n=0; n!=wsize; ++n) {
    window.emplace_back(0.54 - 0.46*cos(cosNum*n/cosDen));
  }
}