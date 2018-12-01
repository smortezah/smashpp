#include <cmath>
#include "filter.hpp"
#include "segment.hpp"
#include "string.hpp"
using namespace smashpp;

Filter::Filter (const Param& p) {
  config(p);
}

inline void Filter::config (const Param& p) {
  wtype = p.wtype;
  config_wsize(p);
  if ((p.filter || p.segment) && p.verbose)
    show_info(p);
}

inline void Filter::config_wsize (const Param& p) {
  if (p.manFilterScale) {
    const auto biggest = min(file_size(p.tar), file_size(p.ref));
    const auto lg      = log10(biggest/p.sampleStep);

    switch (p.filterScale) {
    case FilterScale::S:  wsize=pow(2, 2*lg)   + 1;  break;
    case FilterScale::M:  wsize=pow(2, 2*lg+1) + 1;  break;
    case FilterScale::L:  wsize=pow(2, 2*lg+2) + 1;  break;
    }
  }
  else {
    const auto size = p.wsize/p.sampleStep;
    wsize = is_odd(size) ? size : size+1;
  }

  window.resize(wsize);
}

inline void Filter::show_info (const Param& p) const {
  const u8 lblWidth=19, colWidth=8,
           tblWidth=60;//static_cast<u8>(lblWidth+Ms.size()*colWidth);

  const auto rule = [](u8 n, const string& s) {
    for (auto i=n/s.size(); i--;) { cerr<<s; }    cerr<<'\n';
  };
  const auto toprule = [&]() { rule(tblWidth, "~"); };
  const auto midrule = [&]() { rule(tblWidth, "~"); };
  const auto botrule = [&]() { rule(tblWidth, " "); };
  const auto label   = [&](const string& s){ cerr<<setw(lblWidth) <<left<<s;  };
  const auto header  = [&](const string& s){ cerr<<setw(2*colWidth)<<left<<s; };
  const auto filter_vals = [&](char c) {
    cerr << setw(colWidth) << left;
    switch (c) {
    case 'f':  cerr<<p.print_win_type();      break;
    case 's':  cerr<<p.print_filter_scale();  break;
    case 'w':  cerr<<wsize;                   break;
    case 't':  cerr<<p.thresh;                break;
    default:                                  break;
    }
    cerr << '\n';
  };
  const auto file_vals = [&](char c) {
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
  if (p.manFilterScale || !p.manWSize) {
  label("Filter scale");              filter_vals('s');
  }
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
  message = "Filtering & segmenting " + italic(p.tar) + " ";

  if (wtype == WType::RECTANGULAR) {
    p.saveFilter ? smooth_seg_rect<true>(p) : smooth_seg_rect<false>(p);
  }
  else {
    make_window();
    p.saveFilter ? smooth_seg_non_rect<true>(p) : smooth_seg_non_rect<false>(p);
  }

  if (!p.saveAll && !p.saveProfile)
    remove((gen_name(p.ID, p.ref,p.tar,Format::PROFILE)).c_str());
  if (!p.saveAll && !p.saveFilter)
    remove((gen_name(p.ID, p.ref,p.tar,Format::FILTER)).c_str());

  cerr << message << "finished. "
       << "Detected " << nSegs << " segment" << (nSegs==1 ? "" : "s") << ".\n";
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
  if (wsize==1)  error("The size of Hamming window must be greater than 1.");
  float num = 0.f;
  u32   den = 0;
  if (is_odd(wsize)) { num=PI;      den=(wsize-1)>>1u; }
  else               { num=2*PI;    den=wsize-1;       }

  for (auto n=(wsize+1)>>1u, last=wsize-1; n--;)
    window[n] = window[last-n] = static_cast<float>(0.54 - 0.46*cos(n*num/den));
}

inline void Filter::hann () {
  if (wsize==1)  error("The size of Hann window must be greater than 1.");
  float num = 0.f;
  u32   den = 0;
  if (is_odd(wsize)) { num=PI;      den=(wsize-1)>>1u; }
  else               { num=2*PI;    den=wsize-1;       }

  for (auto n=(wsize+1)>>1u, last=wsize-1; n--;)
    window[n] = window[last-n] = static_cast<float>(0.5 * (1 - cos(n*num/den)));
}

inline void Filter::blackman () {
  if (wsize==1)  error("The size of Blackman window must be greater than 1.");
  float num1=0.f, num2=0.f;
  u32   den = 0;
  if (is_odd(wsize)) { num1=PI;      num2=2*PI;    den=(wsize-1)>>1u; }
  else               { num1=2*PI;    num2=4*PI;    den=wsize-1;       }

  for (auto n=(wsize+1)>>1u, last=wsize-1; n--;)
    window[n] = window[last-n] =
      static_cast<float>(0.42 - 0.5*cos(n*num1/den) + 0.08*cos(n*num2/den));
  if (window.front()<0)    window.front()=0.0; // Because of low precision
  if (window.back()<0)     window.back() =0.0; // Because of low precision
}

// Bartlett window:  w(n) = 1 - |(n - (N-1)/2) / (N-1)/2|
inline void Filter::triangular () {
  if (wsize==1)  error("The size of triangular window must be greater than 1.");

  if (is_odd(wsize)) {
    const u32 den = (wsize-1)>>1u;
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
  if (wsize==1)  error("The size of Welch window must be greater than 1.");

  if (is_odd(wsize)) {
    const u32 den = (wsize-1) >> 1u;
    for (auto n=(wsize+1)>>1u, last=wsize-1; n--;)
      window[n] = window[last-n] = float(1 - Power(float(n)/den-1, 2));
  }
  else {
    const auto num = 2.0f;
    const u32  den = wsize - 1;
    for (auto n=(wsize+1)>>1u, last=wsize-1; n--;)
      window[n] = window[last-n] = float(1 - Power(n*num/den - 1, 2));
  }
}

inline void Filter::sine () {
  if (wsize==1)  error("The size of sine window must be greater than 1.");
  const float num = PI;
  const u32   den = wsize - 1;

  for (auto n=(wsize+1)>>1u, last=wsize-1; n--;)
    window[n] = window[last-n] = sin(n*num/den);
}

inline void Filter::nuttall () {
  if (wsize==1)  error("The size of Nuttall window must be greater than 1.");
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
  const auto profileName  = gen_name(p.ID, p.ref, p.tar, Format::PROFILE),
             filterName   = gen_name(p.ID, p.ref, p.tar, Format::FILTER),
             positionName = gen_name(p.ID, p.ref, p.tar, Format::POSITION);
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
  const auto jump_lines = [&]() {
    for (u64 i=p.sampleStep-1; i--;)  ignore_this_line(prfF);
  };

  // First value
  for (auto i=(wsize>>1u)+1; i-- && getline(prfF,num);) {
    const auto val = stof(num);
    seq.emplace_back(val);
    sum += val;
    show_progress(++symsNo, totalSize, message);
    jump_lines();
  }
  filtered = sum / seq.size();
  if (SaveFilter)
    filF /*<< std::fixed*/ << setprecision(FIL_PREC) << filtered << '\n';
  seg->partition(posF, filtered);

  // Next wsize>>1 values
  for (auto i=(wsize>>1u); i-- && getline(prfF,num);) {
    const auto val = stof(num);
    seq.emplace_back(val);
    sum += val;
    ++seg->pos;
    filtered = sum / seq.size();
    if (SaveFilter)
      filF /*<< std::fixed*/<< setprecision(FIL_PREC) << filtered << '\n';
    seg->partition(posF, filtered);
    show_progress(++symsNo, totalSize, message);
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
      filF /*<< std::fixed*/<< setprecision(FIL_PREC) << filtered << '\n';
    seg->partition(posF, filtered);
    seq[idx] = val;
    idx = (idx+1) % wsize;
    show_progress(++symsNo, totalSize, message);
    jump_lines();
  }
  prfF.close();

  // Until half of the window goes outside the array
  for (auto i=1u; i!=(wsize>>1u)+1; ++i) {
    sum -= seq[idx];
    ++seg->pos;
    filtered = sum / (wsize-i);
    if (SaveFilter)
      filF /*<< std::fixed*/<< setprecision(FIL_PREC) << filtered << '\n';
    seg->partition(posF, filtered);
    idx = (idx+1) % wsize;
    show_progress(++symsNo, totalSize, message);
  }
  seg->partition_last(posF);
  show_progress(++symsNo, totalSize, message);

  posF.close();  filF.close();
  if (!SaveFilter)  remove(filterName.c_str());
  nSegs = seg->nSegs;
}

template <bool SaveFilter>
inline void Filter::smooth_seg_non_rect (const Param& p) {
  const auto profileName  = gen_name(p.ID, p.ref, p.tar, Format::PROFILE),
             filterName   = gen_name(p.ID, p.ref, p.tar, Format::FILTER),
             positionName = gen_name(p.ID, p.ref, p.tar, Format::POSITION);
  check_file(profileName);
  ifstream prfF(profileName);
  ofstream filF(filterName);
  ofstream posF(positionName);
  vector<float> seq;    seq.reserve(wsize);
  auto seg = make_shared<Segment>();
  seg->thresh = p.thresh;
  const auto winBeg=window.begin(), winEnd=window.end();
  auto sWeight = accumulate(winBeg+(wsize>>1u), winEnd, 0.f);
  string num;
  auto sum = 0.f;
  auto filtered = 0.f;
  u64 symsNo = 0;
  const auto totalSize = (file_lines(profileName) / p.sampleStep) + 1;
  const auto jump_lines = [&]() {
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
    filF /*<< std::fixed*/ << setprecision(FIL_PREC) << filtered << '\n';
  seg->partition(posF, filtered);
  show_progress(++symsNo, totalSize, message);

  // Next wsize>>1 values
  for (auto i=(wsize>>1u); i-- && getline(prfF,num);) {
    seq.emplace_back(stof(num));
    sum = inner_product(winBeg+i, winEnd, seq.begin(), 0.f);
    ++seg->pos;
    sWeight += window[i];
    filtered = sum / sWeight;
    if (SaveFilter)
      filF /*<< std::fixed*/<< setprecision(FIL_PREC) << filtered << '\n';
    seg->partition(posF, filtered);
    show_progress(++symsNo, totalSize, message);
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
      filF /*<< std::fixed*/<< setprecision(FIL_PREC) << filtered << '\n';
    seg->partition(posF, filtered);
    show_progress(++symsNo, totalSize, message);
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
      filF /*<< std::fixed*/<< setprecision(FIL_PREC) << filtered << '\n';
    seg->partition(posF, filtered);
    show_progress(++symsNo, totalSize, message);
  }
  seg->partition_last(posF);
  show_progress(++symsNo, totalSize, message);

  posF.close();  filF.close();
  if (!SaveFilter)  remove(filterName.c_str());
  nSegs = seg->nSegs;
}

void Filter::extract_seg (u32 ID, const string& ref, const string& tar) const {
  check_file(gen_name(ID, ref, tar, Format::POSITION));
  ifstream ff(gen_name(ID, ref, tar, Format::POSITION));
  const auto segName = gen_name(ID, ref,tar,Format::SEGMENT);
  auto subseq = make_unique<SubSeq>();
  subseq->inName = tar;
  u64 i = 0;
  for (string beg,end,ent; ff>>beg>>end>>ent; ++i) {
    subseq->outName = segName+to_string(i);
    subseq->begPos  = stoull(beg);
    subseq->size    = static_cast<streamsize>(stoull(end)-subseq->begPos+1);
    extract_subseq(subseq);
  }
  ff.close();
}

void Filter::aggregate_mid_pos (u32 ID, const string& origin, 
const string& dest) const {
  ifstream fDirect(gen_name(ID, origin, dest, Format::POSITION));
  ifstream fReverse;
  ofstream fmid(LBL_MID+"-"+gen_name(ID, origin, dest, Format::POSITION));
  int i = 0;
  for (string begDir, endDir, entDir, selfEntDir; 
       fDirect>>begDir>>endDir>>entDir>>selfEntDir; ++i) {
    const string refRev = 
      gen_name(ID, origin, dest, Format::SEGMENT)+to_string(i);
    fReverse.open(gen_name(ID, refRev, origin, Format::POSITION));
    for (string begRev, endRev, entRev, selfEntRev;
         fReverse>>begRev>>endRev>>entRev>>selfEntRev;) {
      fmid << begRev <<'\t'<< endRev <<'\t'<< entRev <<'\t'<< selfEntRev <<'\t';
      if      (ID==0)  fmid << begDir <<'\t'<< endDir;
      else if (ID==1)  fmid << endDir <<'\t'<< begDir;
      fmid <<'\t'<< entDir <<'\t'<< selfEntDir <<'\n';
    }
    fReverse.close();
    remove((gen_name(ID, refRev, origin, Format::POSITION)).c_str());
  }
  fDirect.close();
  remove((gen_name(ID, origin, dest, Format::POSITION)).c_str());
  fmid.close();
}

void Filter::aggregate_final_pos (const string& ref, const string& tar) const {
  const auto midf0Name = LBL_MID+"-"+gen_name(0, ref, tar, Format::POSITION),
             midf1Name = LBL_MID+"-"+gen_name(1, ref, tar, Format::POSITION);
  ifstream midf0(midf0Name), midf1(midf1Name);
  ofstream finf(ref+"-"+tar+"."+FMT_POS);

  finf << POS_HDR <<'\t'<< ref <<'\t'<< to_string(file_size(ref))
                  <<'\t'<< tar <<'\t'<< to_string(file_size(tar)) << '\n';
  {
  const u64 size = file_size(midf0Name);
  vector<char> buffer(size, 0);
  midf0.read (buffer.data(), size);
  finf.write(buffer.data(), size);
  }
  {
  const u64 size = file_size(midf1Name);
  vector<char> buffer(size, 0);
  midf1.read (buffer.data(), size);
  finf.write(buffer.data(), size);
  }

  midf0.close();  remove(midf0Name.c_str());
  midf1.close();  remove(midf1Name.c_str());
  finf.close();
}

#ifdef BENCH
template <typename Iter, typename Value>
inline void Filter::shift_left_insert (Iter first, Value v) {
  copy(first+1, first+wsize, first);
  *(first+wsize-1) = v;
}
#endif