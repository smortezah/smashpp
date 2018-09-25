//
// Created by morteza on 19-09-2018.
//
#include <cmath>
#include <cstring>//todo
#include "filter.hpp"

Filter::Filter (const Param& p) {
  config(p);
}

inline void Filter::config (const Param& p) {
  config_wtype(p.wtype);
  wsize = is_odd(p.wsize) ? p.wsize : p.wsize+1;
  window.resize(wsize);
  thresh = p.thresh;
}

inline void Filter::config_wtype (const string& t) {
  if      (t=="0" || t=="rectangular")   wtype = WType::RECTANGULAR;
  else if (t=="1" || t=="hamming")       wtype = WType::HAMMING;
  else if (t=="2" || t=="hann")          wtype = WType::HANN;
  else if (t=="3" || t=="blackman")      wtype = WType::BLACKMAN;
  else if (t=="4" || t=="triangular")    wtype = WType::TRIANGULAR;
  else if (t=="5" || t=="welch")         wtype = WType::WELCH;
  else if (t=="6" || t=="sine")          wtype = WType::SINE;
  else if (t=="7" || t=="nuttall")       wtype = WType::NUTTALL;
}

void Filter::smooth (const Param& p) {
  if (wtype == WType::RECTANGULAR) {
    smooth_rect(p);
  }
  else {
    make_window();
    smooth_non_rect(p);
  }
}

inline void Filter::make_window () {
  switch (wtype) {
    case WType::HAMMING:     hamming();      break;
    case WType::HANN:        hann();         break;
    case WType::BLACKMAN:    blackman();     break;
    case WType::TRIANGULAR:  triangular();   break;
    case WType::WELCH:       welch();        break;
    case WType::SINE:        sine();         break;
    case WType::NUTTALL:     nuttall();      break;
    default:                                 break;
  }
}

inline void Filter::hamming () {
  if (wsize == 1)
    error("The size of Hamming window must be greater than 1.");
  float num = 0.0f;
  u32   den = 0;
  if (is_odd(wsize)) { num=PI;      den=(wsize-1)>>1u; }
  else               { num=2*PI;    den=wsize-1;       }

  for (auto n=(wsize+1)>>1u, last=wsize-1; n--;)
    window[n] = window[last-n] = static_cast<float>(0.54 - 0.46*cos(n*num/den));
}

inline void Filter::hann () {
  if (wsize == 1)
    error("The size of Hann window must be greater than 1.");
  float num = 0.0f;
  u32   den = 0;
  if (is_odd(wsize)) { num=PI;      den=(wsize-1)>>1u; }
  else               { num=2*PI;    den=wsize-1;       }

  for (auto n=(wsize+1)>>1u, last=wsize-1; n--;)
    window[n] = window[last-n] = static_cast<float>(0.5 * (1 - cos(n*num/den)));
}

inline void Filter::blackman () {
  if (wsize == 1)
    error("The size of Blackman window must be greater than 1.");
  float num1=0.0f, num2=0.0f;
  u32   den = 0;
  if (is_odd(wsize)) { num1=PI;      num2=2*PI;    den=(wsize-1)>>1u; }
  else               { num1=2*PI;    num2=4*PI;    den=wsize-1;       }

  for (auto n=(wsize+1)>>1u, last=wsize-1; n--;)
    window[n] = window[last-n] =
      static_cast<float>(0.42 - 0.5*cos(n*num1/den) + 0.08*cos(n*num2/den));
  if (window.front() < 0)    window.front() = 0.0; // Because of low precision
  if (window.back()  < 0)    window.back()  = 0.0; // Because of low precision
}

// Bartlett window:  w(n) = 1 - |(n - (N-1)/2) / (N-1)/2|
inline void Filter::triangular () {
  if (wsize == 1)
    error("The size of triangular window must be greater than 1.");

  if (is_odd(wsize)) {
    const u32 den = (wsize-1) >> 1u;
    for (auto n=(wsize+1)>>1u, last=wsize-1; n--;)
      window[n] = window[last-n] = 1 - fabs(static_cast<float>(n)/den - 1);
  }
  else {
    const auto num = 2.0f;
    const u32  den = wsize - 1;
    for (auto n=(wsize+1)>>1u, last=wsize-1; n--;)
      window[n] = window[last-n] = 1 - fabs(n*num/den - 1);
  }
}

inline void Filter::welch () { // w(n) = 1 - ((n - (N-1)/2) / (N-1)/2)^2
  if (wsize == 1)
    error("The size of Welch window must be greater than 1.");

  if (is_odd(wsize)) {
    const u32 den = (wsize-1) >> 1u;
    for (auto n=(wsize+1)>>1u, last=wsize-1; n--;)
      window[n] = window[last-n] = 1 - pow2(static_cast<float>(n)/den - 1);
  }
  else {
    const auto num = 2.0f;
    const u32  den = wsize - 1;
    for (auto n=(wsize+1)>>1u, last=wsize-1; n--;)
      window[n] = window[last-n] = 1 - pow2(n*num/den - 1);
  }
}

inline void Filter::sine () {
  if (wsize == 1)
    error("The size of sine window must be greater than 1.");
  const float num = PI;
  const u32   den = wsize - 1;

  for (auto n=(wsize+1)>>1u, last=wsize-1; n--;)
    window[n] = window[last-n] = sin(n*num/den);
}

inline void Filter::nuttall () {
  if (wsize == 1)
    error("The size of Nuttall window must be greater than 1.");
  float num1=0.0f, num2=0.0f, num3=0.0f;
  u32   den = 0;
  if (is_odd(wsize)) { num1=PI;    num2=2*PI;  num3=3*PI;  den=(wsize-1)>>1u; }
  else               { num1=2*PI;  num2=4*PI;  num3=6*PI;  den=wsize-1;       }

  for (auto n=(wsize+1)>>1u, last=wsize-1; n--;)
    window[n] = window[last-n] =
      static_cast<float>(0.36 - 0.49*cos(n*num1/den) + 0.14*cos(n*num2/den)
                              - 0.01*cos(n*num3/den));
  window.front() = window.back() = 0.0;
}

void Filter::smooth_rect (const Param& p) {cerr<<thresh;
//  const auto sumWeight = wsize;
  ifstream pf(PROFILE_LBL+p.tar);
  vector<float> seq;    seq.reserve(wsize);
  string num;
  u64    pos = 0;
  float  sum = 0.0f;

  // First value
  for (auto i=(wsize>>1u)+1; i-- && getline(pf,num); ++pos) {
    const auto val = stof(num);
    seq.emplace_back(val);
    sum += val;
  }
  /// if sum > wsize*thresh

  // Next wsize>>1 values
  for (auto i=(wsize>>1u); i-- && getline(pf,num); ++pos) {
    const auto val = stof(num);
    seq.emplace_back(val);
    sum += val;
    /// if sum > wsize*thresh
  }

  // The rest
  u32 idx = 0;
  for (; getline(pf,num); ++pos) {
    const auto val = stof(num);
    sum = sum - seq[idx] + val;
    /// if sum > wsize*thresh
    seq[idx] = val;
    idx = (idx+1) % wsize;
  }

  // Up to when half of the window goes outside the array
  for (auto i=(wsize>>1u); i--;) {
    sum -= seq[idx];
    /// if sum > wsize*thresh
    idx = (idx+1) % wsize;
  }

  pf.close();
}

void Filter::smooth_non_rect (const Param& p) {
  const auto sumWeight = accumulate(window.begin(), window.end(), 0.0f);
  ifstream pf(PROFILE_LBL+p.tar);
  vector<float> seq;    seq.reserve(wsize);
  string num;

  for (auto i=(wsize>>1u)+1; i-- && getline(pf,num);)
    seq.emplace_back(stof(num));

  u64 pos = 0;//todo
  cerr <<
       inner_product(window.begin()+(wsize>>1u), window.end(), seq.begin(),
                     0.0f) / sumWeight << '\n';  // The first number //todo

  for (auto i=(wsize>>1u); i-- && getline(pf,num);) {
    seq.emplace_back(stof(num));
    cerr << inner_product(window.begin()+i, window.end(), seq.begin(), 0.0f) /
            sumWeight << '\n';//todo
  }

  u32 idx = 0;
  for(auto seqBeg=seq.begin(); getline(pf,num);) {   // pf.peek() != EOF
    *(seqBeg+idx) = stof(num); // or seq[idx] = stof(num);
    idx = (idx+1) % wsize;

    cerr <<
    (inner_product(window.begin(), window.end()-idx, seq.begin()+idx, 0.0f) +
    inner_product(window.end()-idx, window.end(), seq.begin(), 0.0f)) /
         sumWeight << '\n';//todo
  }

  const auto offset = idx;
  for (auto i=wsize>>1u; i--;) {
    if (++idx < wsize+1) {
      cerr <<
           (inner_product(seq.begin()+idx, seq.end(), window.begin(), 0.0f) +
            inner_product(seq.begin(), seq.begin()+offset, window.end()-idx,
                          0.0f)
           ) / sumWeight << '\n';//todo
    }
    else {
      cerr << inner_product(seq.begin()+(idx%wsize), seq.begin()+offset,
                            window.begin(), 0.0f) / sumWeight << '\n';//todo
    }
  }

  pf.close();
}

#ifdef BENCH
template <typename Iter, typename Value>
inline void Filter::shift_left_insert (Iter first, Value v) {
  copy(first+1, first+wsize, first);
  *(first+wsize-1) = v;
}
#endif