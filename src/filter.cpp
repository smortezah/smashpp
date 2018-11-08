//
// Created by morteza on 19-09-2018.
//

#include <cmath>
#include "filter.hpp"
#include "segment.hpp"
using namespace smashpp;

Filter::Filter (const Param& p) {
  config(p);
}

inline void Filter::config (const Param& p) {
  config_wtype(p.wtype);
  wsize = is_odd(p.wsize) ? p.wsize : p.wsize+1;
  window.resize(wsize);
  if ((p.filter || p.segment) && p.verbose)  show_info(p);
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

inline void Filter::show_info (const Param& p) const {
  const u8 lblWidth=19, colWidth=8,
    tblWidth=60;//static_cast<u8>(lblWidth+Ms.size()*colWidth);

  const auto rule = [] (u8 n, const string& s) {
    for (auto i=n/s.size(); i--;) { cerr<<s; }    cerr<<'\n';
  };
  const auto toprule  = [&] () { rule(tblWidth, "~"); };
  const auto midrule  = [&] () { rule(tblWidth, "~"); };
  const auto botrule  = [&] () { rule(tblWidth, " "); };
  const auto label    = [&] (const string& s) {cerr<<setw(lblWidth) <<left<<s;};
  const auto header   = [&] (const string& s){cerr<<setw(2*colWidth)<<left<<s;};
  const auto filter_vals = [&] (char c) {
    cerr << setw(colWidth) << left;
    switch (c) {
    case 'f':  cerr<<p.print_win_type();  break;
    case 'w':  cerr<<p.wsize;             break;
    case 't':  cerr<<p.thresh;            break;
    default:                              break;
    }
    cerr << '\n';
  };
  const auto file_vals = [&] (char c) {
    cerr << setw(2*colWidth) << left;
    switch (c) {
    case '1':  cerr.imbue(locale("en_US.UTF8")); cerr<<file_size(p.ref);  break;
    case 'r':  cerr<<p.ref;                                               break;
    case '2':  cerr.imbue(locale("en_US.UTF8")); cerr<<file_size(p.tar);  break;
    case 't':  cerr<<p.tar;                                               break;
    default:                                                              break;
    }
  };

  toprule();
  label("Filter & Segment");          cerr<<'\n';
  midrule();
  label("Window function");           filter_vals('f');
  label("Window size");               filter_vals('w');
  if (p.manThresh) {
  label("Threshold");                 filter_vals('t');
  }
  botrule();  //cerr << '\n';

  toprule();
  label("Files");        header("Size (B)");    header("Name");      cerr<<'\n';
  midrule();
  label("Reference");    file_vals('1');        file_vals('r');      cerr<<'\n';
  label("Target");       file_vals('2');        file_vals('t');      cerr<<'\n';
  botrule();
}

void Filter::smooth_seg (const Param& p) {
  cerr << OUT_SEP << "Filtering and segmenting \"" << p.tar << "\"...\n";

  if (wtype == WType::RECTANGULAR) {
    p.saveFilter ? smooth_seg_rect<true>(p) : smooth_seg_rect<false>(p);
  }
  else {
    make_window();
    p.saveFilter ? smooth_seg_non_rect<true>(p) : smooth_seg_non_rect<false>(p);
  }
  if (!p.saveAll && !p.saveProfile)
    remove((gen_name(p.ref,p.tar,Format::PROFILE)).c_str());
  if (!p.saveAll && !p.saveFilter)
    remove((gen_name(p.ref,p.tar,Format::FILTER)).c_str());

  cerr << "Done!\n";
//  if (p.verbose)
    cerr << "Detected " << nSegs << " segment" << (nSegs==1 ? "" : "s") <<".\n";
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
  // todoo. Remove. Just for test. Compatible with Diogo's version
//    for (auto n=(wsize+1)>>1u, last=wsize-1; n--;)
//    window[n] = window[last-n]
//              = static_cast<float>(0.54 + 0.46*cos(2*PI*n/wsize));
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
      window[n] = window[last-n]
                = static_cast<float>(1 - pow(static_cast<float>(n)/den - 1, 2));
  }
  else {
    const auto num = 2.0f;
    const u32  den = wsize - 1;
    for (auto n=(wsize+1)>>1u, last=wsize-1; n--;)
      window[n] = window[last-n]
                = static_cast<float>(1 - pow(n*num/den - 1, 2));
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

template <bool SaveFilter>
inline void Filter::smooth_seg_rect (const Param& p) {
  const auto profileName  = gen_name(p.ref, p.tar, Format::PROFILE),
             filterName   = gen_name(p.ref, p.tar, Format::FILTER),
             positionName = gen_name(p.ref, p.tar, Format::POSITION);
  check_file(profileName);
  ifstream prfF(profileName);
  ofstream filF(filterName);
  ofstream posF(positionName);
  vector<float> seq;    seq.reserve(wsize);
  auto seg = make_shared<Segment>();
  seg->thresh = p.thresh;
  string num;
  auto sum = 0.f;
  auto filtered = 0.f;
  u64 symsNo = 0;
  const auto totalSize = (file_lines(profileName) / p.sampleStep) + 1;
  const auto jump_lines = [&] () {
    for (u64 i=p.sampleStep-1; i--;)  ignore_this_line(prfF);
  };

  // First value
  for (auto i=(wsize>>1u)+1; i-- && getline(prfF,num);) {
    const auto val = stof(num);
    seq.emplace_back(val);
    sum += val;
    show_progress(++symsNo, totalSize);
    jump_lines();
  }
  filtered = sum / seq.size();
  if (SaveFilter)
    filF /*<< std::fixed*/ << setprecision(DEF_FIL_PREC) << filtered << '\n';
  seg->partition(posF, filtered);

  // Next wsize>>1 values
  for (auto i=(wsize>>1u); i-- && getline(prfF,num);) {
    const auto val = stof(num);
    seq.emplace_back(val);
    sum += val;
    ++seg->pos;
    filtered = sum / seq.size();
    if (SaveFilter)
      filF /*<< std::fixed*/<< setprecision(DEF_FIL_PREC) << filtered << '\n';
    seg->partition(posF, filtered);
    show_progress(++symsNo, totalSize);
    jump_lines();
  }

  // The rest
  u32 idx = 0;
  while (getline(prfF,num)) {
    const auto val = stof(num);
    sum += val - seq[idx];
    ++seg->pos;
    filtered = sum / wsize;
    if (SaveFilter)
      filF /*<< std::fixed*/<< setprecision(DEF_FIL_PREC) << filtered << '\n';
    seg->partition(posF, filtered);
    seq[idx] = val;
    idx = (idx+1) % wsize;
    show_progress(++symsNo, totalSize);
    jump_lines();
  }
  prfF.close();

  // Until half of the window goes outside the array
  for (auto i=1u; i!=(wsize>>1u)+1; ++i) {
    sum -= seq[idx];
    ++seg->pos;
    filtered = sum / (wsize-i);
    if (SaveFilter)
      filF /*<< std::fixed*/<< setprecision(DEF_FIL_PREC) << filtered << '\n';
    seg->partition(posF, filtered);
    idx = (idx+1) % wsize;
    show_progress(++symsNo, totalSize);
  }
  seg->partition_last(posF);
  show_progress(++symsNo, totalSize);

  remove_progress_trace();
  posF.close();  filF.close();
  if (!SaveFilter)  remove(filterName.c_str());
  nSegs = seg->nSegs;
}

template <bool SaveFilter>
inline void Filter::smooth_seg_non_rect (const Param& p) {
  const auto profileName  = gen_name(p.ref, p.tar, Format::PROFILE),
             filterName   = gen_name(p.ref, p.tar, Format::FILTER),
             positionName = gen_name(p.ref, p.tar, Format::POSITION);
  check_file(profileName);
  ifstream prfF(profileName);
  ofstream filF(filterName);
  ofstream posF(positionName);
  vector<float> seq;    seq.reserve(wsize);
  auto seg = make_shared<Segment>();
  seg->thresh = p.thresh;
  auto winBeg=window.begin(), winEnd=window.end();
  auto sWeight = accumulate(winBeg+(wsize>>1u), winEnd, 0.f);
  string num;
  auto sum = 0.f;
  auto filtered = 0.f;
  u64 symsNo = 0;
  const auto totalSize = (file_lines(profileName) / p.sampleStep) + 1;
  const auto jump_lines = [&] () {
    for (u64 i=p.sampleStep-1; i--;)  ignore_this_line(prfF);
  };

  // First value
  for (auto i=(wsize>>1u)+1; i-- && getline(prfF,num); jump_lines()) {
    seq.emplace_back(stof(num));
    jump_lines();
  }
  sum = inner_product(winBeg+(wsize>>1u), winEnd, seq.begin(), 0.f);
  filtered = sum / sWeight;
  if (SaveFilter)
    filF /*<< std::fixed*/ << setprecision(DEF_FIL_PREC) << filtered << '\n';
  seg->partition(posF, filtered);
  show_progress(++symsNo, totalSize);

  // Next wsize>>1 values
  for (auto i=(wsize>>1u); i-- && getline(prfF,num);) {
    seq.emplace_back(stof(num));
    sum = inner_product(winBeg+i, winEnd, seq.begin(), 0.f);
    ++seg->pos;
    sWeight += window[i];
    filtered = sum / sWeight;
    if (SaveFilter)
      filF /*<< std::fixed*/<< setprecision(DEF_FIL_PREC) << filtered << '\n';
    seg->partition(posF, filtered);
    show_progress(++symsNo, totalSize);
    jump_lines();
  }

  // The rest
  u32 idx = 0;
  for(auto seqBeg=seq.begin(); getline(prfF,num);) {
    seq[idx] = stof(num);
    idx = (idx+1) % wsize;
    sum = (inner_product(winBeg,     winEnd-idx, seqBeg+idx, 0.f) +
           inner_product(winEnd-idx, winEnd,     seqBeg,     0.f));
    ++seg->pos;
    filtered = sum / sWeight;
    if (SaveFilter)
      filF /*<< std::fixed*/<< setprecision(DEF_FIL_PREC) << filtered << '\n';
    seg->partition(posF, filtered);
    show_progress(++symsNo, totalSize);
    jump_lines();
  }
  prfF.close();

  // Until half of the window goes outside the array
  const auto offset = idx;
  for (auto i=1u; i!=(wsize>>1u)+1; ++i) {
    auto seqBeg=seq.begin(), seqEnd=seq.end();
    if (++idx < wsize+1)
      sum = (inner_product(seqBeg+idx, seqEnd,        winBeg,     0.f) +
             inner_product(seqBeg,     seqBeg+offset, winEnd-idx, 0.f));
    else
      sum = inner_product(seqBeg+(idx%wsize), seqBeg+offset, winBeg, 0.f);
    ++seg->pos;
    sWeight -= window[wsize-i];
    filtered = sum / sWeight;
    if (SaveFilter)
      filF /*<< std::fixed*/<< setprecision(DEF_FIL_PREC) << filtered << '\n';
    seg->partition(posF, filtered);
    show_progress(++symsNo, totalSize);
  }
  seg->partition_last(posF);
  show_progress(++symsNo, totalSize);

  remove_progress_trace();
  posF.close();  filF.close();
  if (!SaveFilter)  remove(filterName.c_str());
  nSegs = seg->nSegs;
}

void Filter::extract_seg (const string& ref, const string& tar) const {
  check_file(gen_name(ref,tar,Format::POSITION));
  ifstream ff(gen_name(ref,tar,Format::POSITION));
  const auto segName = gen_name(ref,tar,Format::SEGMENT);
  auto subseq = make_shared<SubSeq>();
  subseq->inName = tar;
  u64 i = 0;
  for (string beg, end, ent; ff>>beg>>end>>ent; ++i) {
    subseq->outName = segName+to_string(i);
    subseq->begPos  = stoull(beg);
    subseq->size    = static_cast<streamsize>(stoull(end)-subseq->begPos+1);
    extract_subseq(subseq);
  }
  ff.close();
}

void Filter::aggregate_pos (const string& origin, const string& dest) const {
  ifstream fDirect(gen_name(origin, dest, Format::POSITION));
  ofstream ffinal("final-"+gen_name(origin, dest, Format::POSITION));
  ffinal << POS_HDR <<'\t'<< origin <<'\t'<< to_string(file_size(origin))
                    <<'\t'<< dest   <<'\t'<< to_string(file_size(dest)) << '\n';
  int i = 0;
  for (string begDir, endDir, entDir; fDirect>>begDir>>endDir>>entDir; ++i) {
    const string refRev = gen_name(origin, dest, Format::SEGMENT)+to_string(i);
    ifstream fReverse(gen_name(refRev, origin, Format::POSITION));
    for (string begRev, endRev, entRev; fReverse>>begRev>>endRev>>entRev;) {
      ffinal << begRev <<'\t'<< endRev <<'\t'<< entRev <<'\t'<< 'A' <<'\t'
             << begDir <<'\t'<< endDir <<'\t'<< entDir <<'\t'<< 'A' << '\n';
    }
    fReverse.close();
  }
  fDirect.close();  ffinal.close();
}

#ifdef BENCH
template <typename Iter, typename Value>
inline void Filter::shift_left_insert (Iter first, Value v) {
  copy(first+1, first+wsize, first);
  *(first+wsize-1) = v;
}
#endif