//
// Created by morteza on 19-09-2018.
//
#include <cmath>
#include "filter.hpp"

Filter::Filter (const Param& p) : buffSize(DEF_FIL_BUF) {
  config(p);
}

inline void Filter::config (const Param& p) {
  config_wtype(p.wtype);
  wsize = is_odd(p.wsize) ? p.wsize : p.wsize+1;
  for (auto i=static_cast<u64>(log2(DEF_FIL_BUF)); wsize > buffSize;)
    buffSize = pow2(++i);
  window.resize(wsize);
}

inline void Filter::config_wtype (const string& t) {
  if      (t=="0" || t=="hamming")       wtype = WType::HAMMING;
  else if (t=="1" || t=="hann")          wtype = WType::HANN;
  else if (t=="2" || t=="blackman")      wtype = WType::BLACKMAN;
  else if (t=="3" || t=="triangular")    wtype = WType::TRIANGULAR;
  else if (t=="4" || t=="welch")         wtype = WType::WELCH;
  else if (t=="5" || t=="sine")          wtype = WType::SINE;
  else if (t=="6" || t=="nuttall")       wtype = WType::NUTTALL;
}

void Filter::smooth (const Param& p) {
  make_window();
  for(auto i:window) cerr<<i<<' ';//todo

  ifstream pf(PROFILE_LBL+p.tar);
  vector<float> seq;

  while (!pf.eof()) {  // pf.peek() != EOF
    seq.clear();
    seq.reserve(buffSize);
    string num;
    for (auto i=buffSize; i-- && getline(pf,num);)
      seq.emplace_back(stof(num));
    for (auto i :seq)cerr << '\n' << i;//todo
    cerr << "\n---------\n";

    const auto sumWeight = accumulate(window.begin(), window.end(), 0.0f);
    if (seq.size() >= wsize) {
      for (auto i = (wsize + 1) >> 1u; i--;)
        cerr <<"1-> "<< inner_product(window.begin() + i, window.end(), seq.begin(), 0.0f) / sumWeight << '\n';
      for (auto i = 1; i < seq.size() - wsize + 1; ++i)
        cerr <<"2-> "<< inner_product(window.begin(), window.end(), seq.begin() + i, 0.0f) / sumWeight << '\n';
      for (auto i = wsize - 1; i != wsize >> 1u; --i)
        cerr <<"3-> "<< inner_product(seq.end() - i, seq.end(), window.begin(), 0.0f) / sumWeight << '\n';
    }
    else {
      //todo
      for (auto i = (wsize + 1) >> 1u; i--;)
        cerr <<"1-> "<< inner_product(window.begin() + i, window.end(), seq.begin(), 0.0f) / sumWeight << '\n';
    }
  }

  pf.close();
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