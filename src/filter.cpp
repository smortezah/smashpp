//
// Created by morteza on 19-09-2018.
//

#include <cmath>
#include "filter.hpp"
#include "segment.hpp"

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

void Filter::smooth_seg (const Param& p) {
  if (p.saveFilter) {
    const auto t0{now()};
    cerr << OUT_SEP << "Filtering \"" << p.tar << "\"...\n";
    if (wtype == WType::RECTANGULAR) {
      smooth_rect(p);
      cerr << "Finished in ";
      const auto t1{now()};
      cerr << hms(t1-t0);
    }
    else {
      make_window();
      smooth_non_rect(p);
      cerr << "Finished in ";
      const auto t1{now()};
      cerr << hms(t1-t0);
    }
  }
  else {
    const auto t0{now()};
    cerr << OUT_SEP << "Filtering and segmenting \"" << p.tar << "\"...\n";
    if (wtype == WType::RECTANGULAR)
      smooth_seg_rect(p);
    else {
      make_window();
      smooth_seg_non_rect(p);
    }
    cerr << "Finished in ";
    const auto t1{now()};
    cerr << hms(t1-t0);
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
  float num = 0.f;
  u32   den = 0;
  if (is_odd(wsize)) { num=PI;      den=(wsize-1)>>1u; }
  else               { num=2*PI;    den=wsize-1;       }

  for (auto n=(wsize+1)>>1u, last=wsize-1; n--;)
    window[n] = window[last-n] = static_cast<float>(0.54 - 0.46*cos(n*num/den));
//  for (auto n=(wsize+1)>>1u, last=wsize-1; n--;)
//    window[n] = window[last-n] = static_cast<float>(0.54 + 0.46*cos(2*PI*n/wsize));
}

inline void Filter::hann () {
  if (wsize == 1)
    error("The size of Hann window must be greater than 1.");
  float num = 0.f;
  u32   den = 0;
  if (is_odd(wsize)) { num=PI;      den=(wsize-1)>>1u; }
  else               { num=2*PI;    den=wsize-1;       }

  for (auto n=(wsize+1)>>1u, last=wsize-1; n--;)
    window[n] = window[last-n] = static_cast<float>(0.5 * (1 - cos(n*num/den)));
}

inline void Filter::blackman () {
  if (wsize == 1)
    error("The size of Blackman window must be greater than 1.");
  float num1=0.f, num2=0.f;
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
  float num1=0.f, num2=0.f, num3=0.f;
  u32   den = 0;
  if (is_odd(wsize)) { num1=PI;    num2=2*PI;  num3=3*PI;  den=(wsize-1)>>1u; }
  else               { num1=2*PI;  num2=4*PI;  num3=6*PI;  den=wsize-1;       }

  for (auto n=(wsize+1)>>1u, last=wsize-1; n--;)
    window[n] = window[last-n] =
      static_cast<float>(0.36 - 0.49*cos(n*num1/den) + 0.14*cos(n*num2/den)
                              - 0.01*cos(n*num3/den));
  window.front() = window.back() = 0.0;
}

inline void Filter::smooth_rect (const Param& p) {
  const string fName = p.ref + "_" + p.tar;
  check_file(fName+PROFILE_FMT);
  ifstream pf(fName+PROFILE_FMT);
  ofstream ff(fName+FIL_FMT);
  string num;
  vector<float> seq;    seq.reserve(wsize);
  float sum = 0.f;

  // First value
  for (auto i=(wsize>>1u)+1; i-- && getline(pf,num);) {
    const auto val = stof(num);
    seq.emplace_back(val);
    sum += val;
  }
  ff /*<< std::fixed*/ << setprecision(DEF_FIL_PREC) << sum/wsize <<'\n';

  // Next wsize>>1 values
  for (auto i=(wsize>>1u); i-- && getline(pf,num);) {
    const auto val = stof(num);
    seq.emplace_back(val);
    sum += val;
    ff /*<< std::fixed*/ << setprecision(DEF_FIL_PREC) << sum/wsize <<'\n';
  }

  // The rest
  u32 idx = 0;
  for (; getline(pf,num);) {
    const auto val = stof(num);
    sum += val - seq[idx];
    ff /*<< std::fixed*/ << setprecision(DEF_FIL_PREC) << sum/wsize <<'\n';
    seq[idx] = val;
    idx = (idx+1) % wsize;
  }
  pf.close();

  // Until half of the window goes outside the array
  for (auto i=(wsize>>1u); i--;) {
    sum -= seq[idx];
    ff /*<< std::fixed*/ << setprecision(DEF_FIL_PREC) << sum/wsize <<'\n';
    idx = (idx+1) % wsize;
  }

  ff.close();
}

inline void Filter::smooth_seg_rect (const Param& p) {
  const string fName = p.ref + "_" + p.tar;
  check_file(fName+PROFILE_FMT);
  ifstream pf(fName+PROFILE_FMT);
  ofstream ff(fName+POS_FMT);
  string num;
  vector<float> seq;    seq.reserve(wsize);
  auto seg = make_shared<Segment>();
  seg->cut = wsize * thresh;  // Sum of weights=wsize. All coeffs of win are 1

  // First value
  for (auto i=(wsize>>1u)+1; i-- && getline(pf,num);) {
    const auto val = stof(num);
    seq.emplace_back(val);
    seg->sum += val;
  }
  seg->partition(ff);

  // Next wsize>>1 values
  for (auto i=(wsize>>1u); i-- && getline(pf,num);) {
    const auto val = stof(num);
    seq.emplace_back(val);
    seg->sum += val;
    ++seg->pos;
    seg->partition(ff);
  }

  // The rest
  u32 idx = 0;
  for (; getline(pf,num);) {
    const auto val = stof(num);
    seg->sum += val - seq[idx];
    ++seg->pos;
    seg->partition(ff);
    seq[idx] = val;
    idx = (idx+1) % wsize;
  }
  pf.close();

  // Until half of the window goes outside the array
  for (auto i=(wsize>>1u); i--;) {
    seg->sum -= seq[idx];
    ++seg->pos;
    seg->partition(ff);
    idx = (idx+1) % wsize;
  }
  seg->partition_last(ff);

  ff.close();
  nSegs = seg->nSegs;
  if (p.verbose)    cerr << "Detected " << nSegs << " segments.\n";
}

inline void Filter::smooth_non_rect (const Param& p) {
  const string fName = p.ref + "_" + p.tar;
  check_file(fName+PROFILE_FMT);
  ifstream pf(fName+PROFILE_FMT);
  ofstream ff(fName+FIL_FMT);
  string num;
  vector<float> seq;    seq.reserve(wsize);
  auto winBeg=window.begin(), winEnd=window.end();
  float sum = 0.f;
  float sWeight = accumulate(winBeg,winEnd,0.0f) * thresh;

  // First value
  for (auto i=(wsize>>1u)+1; i-- && getline(pf,num);)
    seq.emplace_back(stof(num));
  sum = inner_product(winBeg+(wsize>>1u), winEnd, seq.begin(), 0.f);
  ff /*<< std::fixed*/ << setprecision(DEF_FIL_PREC) << sum/sWeight <<'\n';

  // Next wsize>>1 values
  for (auto i=(wsize>>1u); i-- && getline(pf,num);) {
    seq.emplace_back(stof(num));
    sum = inner_product(winBeg+i, winEnd, seq.begin(), 0.f);
    ff /*<< std::fixed*/ << setprecision(DEF_FIL_PREC) << sum/sWeight <<'\n';
  }

  // The rest
  u32 idx = 0;
  for(auto seqBeg=seq.begin(); getline(pf,num);) {
    seq[idx] = stof(num);
    idx = (idx+1) % wsize;
    sum = (inner_product(winBeg,     winEnd-idx, seqBeg+idx, 0.f) +
           inner_product(winEnd-idx, winEnd,     seqBeg,     0.f));
    ff /*<< std::fixed*/ << setprecision(DEF_FIL_PREC) << sum/sWeight <<'\n';
  }
  pf.close();

  // Until half of the window goes outside the array
  const auto offset = idx;
  for (auto i=wsize>>1u; i--;) {
    auto seqBeg=seq.begin(), seqEnd=seq.end();
    if (++idx < wsize+1) {
      sum = (inner_product(seqBeg+idx, seqEnd,        winBeg,     0.f) +
             inner_product(seqBeg,     seqBeg+offset, winEnd-idx, 0.f));
      ff /*<< std::fixed*/ << setprecision(DEF_FIL_PREC) << sum/sWeight <<'\n';
    }
    else {
      sum = inner_product(seqBeg+(idx%wsize), seqBeg+offset, winBeg,0.f);
      ff /*<< std::fixed*/ << setprecision(DEF_FIL_PREC) << sum/sWeight <<'\n';
    }
  }

  ff.close();
}

inline void Filter::smooth_seg_non_rect (const Param& p) {
  const string fName = p.ref + "_" + p.tar;
  check_file(fName+PROFILE_FMT);
  ifstream pf(fName+PROFILE_FMT);
  ofstream ff(fName+POS_FMT);
  string num;
  vector<float> seq;    seq.reserve(wsize);
  auto seg = make_shared<Segment>();
  auto winBeg=window.begin(), winEnd=window.end();
  seg->cut = accumulate(winBeg,winEnd,0.0f) * thresh;

  // First value
  for (auto i=(wsize>>1u)+1; i-- && getline(pf,num);)
    seq.emplace_back(stof(num));
  seg->sum = inner_product(winBeg+(wsize>>1u), winEnd, seq.begin(), 0.0f);
  seg->partition(ff);

  // Next wsize>>1 values
  for (auto i=(wsize>>1u); i-- && getline(pf,num);) {
    seq.emplace_back(stof(num));
    seg->sum = inner_product(winBeg+i, winEnd, seq.begin(), 0.0f);
    ++seg->pos;
    seg->partition(ff);
  }

  // The rest
  u32 idx = 0;
  for(auto seqBeg=seq.begin(); getline(pf,num);) {
    seq[idx] = stof(num);
    idx = (idx+1) % wsize;
    seg->sum = (inner_product(winBeg,     winEnd-idx, seqBeg+idx, 0.0f) +
                inner_product(winEnd-idx, winEnd,     seqBeg,     0.0f));
    ++seg->pos;
    seg->partition(ff);
  }
  pf.close();

  // Until half of the window goes outside the array
  const auto offset = idx;
  for (auto i=wsize>>1u; i--;) {
    auto seqBeg=seq.begin(), seqEnd=seq.end();
    if (++idx < wsize+1)
      seg->sum = (inner_product(seqBeg+idx, seqEnd,        winBeg,     0.0f) +
                  inner_product(seqBeg,     seqBeg+offset, winEnd-idx, 0.0f));
    else
      seg->sum = inner_product(seqBeg+(idx%wsize), seqBeg+offset, winBeg,0.0f);
    ++seg->pos;
    seg->partition(ff);
  }
  seg->partition_last(ff);

  ff.close();
  nSegs = seg->nSegs;
  if (p.verbose)    cerr << "Detected " << nSegs << " segments.\n";
}

void Filter::extract_seg (const string& tar, const string& ref) const {
  const string fName = ref + "_" + tar;
  check_file(fName+POS_FMT);
  ifstream ff(fName+POS_FMT);
  string posStr;

  for (u64 i=0; getline(ff,posStr); ++i) {
    vector<string> posVec;    posVec.reserve(2);
    split(posStr.begin(), posStr.end(), '\t', posVec);
    extract_subseq(tar, fName+SEG_LBL+to_string(i),
                   stoull(posVec[0]), stoull(posVec[1]));
  }

  ff.close();
}

#ifdef BENCH
template <typename Iter, typename Value>
inline void Filter::shift_left_insert (Iter first, Value v) {
  copy(first+1, first+wsize, first);
  *(first+wsize-1) = v;
}
#endif