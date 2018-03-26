//
// Created by morteza on 2/4/18.
//

#include <fstream>
#include <cmath>
#include <thread>
#include "fcm.hpp"
using std::ifstream;
using std::cout;
using std::array;
using std::thread;

FCM::FCM (const Param& p) {
  setModels(p);
  allocModels();
  setModesComb();
}

FCM::~FCM () {
  for (const auto& m : model) {
    if      (m.mode==MODE::TABLE_64)      delete tbl64;
    else if (m.mode==MODE::TABLE_32)      delete tbl32;
    else if (m.mode==MODE::LOG_TABLE_8)   delete logtbl8;
    else                                  delete sketch4;
  }
}

inline void FCM::setModels (const Param& p) {
  model.resize(LEVEL[p.level][0]);
  for (auto m=model.begin(); m!=model.end(); ++m) {
    auto i = m-model.begin();
    m->ir = LEVEL[p.level][5*i+1];
    m->k = LEVEL[p.level][5*i+2];
    m->alpha = static_cast<float>(LEVEL[p.level][5*i+3])/100;
    m->w = power(2, LEVEL[p.level][5*i+4]);
    m->d = LEVEL[p.level][5*i+5];
    if      (m->k>K_MAX_LGTBL8)   m->mode = MODE::SKETCH_8;
    else if (m->k>K_MAX_TBL32)    m->mode = MODE::LOG_TABLE_8;
    else if (m->k>K_MAX_TBL64)    m->mode = MODE::TABLE_32;
    else                          m->mode = MODE::TABLE_64;
  }
}

inline void FCM::allocModels () {
  for (const auto& m : model) {
    if      (m.mode==MODE::TABLE_64)        tbl64 = new Table64(m.k);
    else if (m.mode==MODE::TABLE_32)        tbl32 = new Table32(m.k);
    else if (m.mode==MODE::LOG_TABLE_8)   logtbl8 = new LogTable8(m.k);
    else                                  sketch4 = new CMLS4(m.w, m.d);
  }
}

inline void FCM::setModesComb () {
  u8 isT64=0, isT32=0, isLT=0, isSk=0;
  for (const auto& m : model) {
    if      (m.mode==MODE::TABLE_64)      isT64 = 1;
    else if (m.mode==MODE::TABLE_32)      isT32 = 1<<1;
    else if (m.mode==MODE::LOG_TABLE_8)   isLT  = 1<<2;
    else if (m.mode==MODE::SKETCH_8)      isSk  = 1<<3;
  }
  MODE_COMB = isSk | isLT | isT32 | isT64;
}

void FCM::buildModel (const Param& p) {
  cerr << "Building models...\n";
  for (const auto& m : model) {
    auto mask32 = static_cast<u32>((4<<(m.k<<1))-1);  // 4<<2k - 1 = 4^(k+1) - 1
    auto mask64 = static_cast<u64>((4<<(m.k<<1))-1);
    switch (m.mode) {
      case MODE::TABLE_64:     createDS(p.ref, mask32, tbl64);    break;
      case MODE::TABLE_32:     createDS(p.ref, mask32, tbl32);    break;
      case MODE::LOG_TABLE_8:  createDS(p.ref, mask32, logtbl8);  break;
      case MODE::SKETCH_8:     createDS(p.ref, mask64, sketch4);  break;
      default:                 cerr << "Error.";
    }
  }
  
//  //todo. test multithreading
//  vector<thread> thrd;  thrd.resize(4);
//  for (const auto& m : model) {
//    auto mask32 = static_cast<u32>((4<<(m.k<<1))-1);  // 4<<2k - 1 = 4^(k+1) - 1
//    auto mask64 = static_cast<u64>((4<<(m.k<<1))-1);
//    switch (m.mode) {
//      case MODE::TABLE_64:
//        thrd[0] = thread(&FCM::createDS<u32,Table64*>, this,
//                         p.ref, mask32, tbl64);
//        break;
//      case MODE::TABLE_32:
//        thrd[1] = thread(&FCM::createDS<u32,Table32*>, this,
//                         p.ref, mask32, tbl32);
//        break;
//      case MODE::LOG_TABLE_8:
//        thrd[2] = thread(&FCM::createDS<u32,LogTable8*>, this,
//                         p.ref, mask32, logtbl8);
//        break;
//      case MODE::SKETCH_8:
//        thrd[3] = thread(&FCM::createDS<u64,CMLS4*>, this,
//                         p.ref, mask64, sketch4);
//        break;
//      default:    cerr << "Error.";
//    }
//  }
//  for (u8 t=0; t!=4; ++t)  if (thrd[t].joinable()) thrd[t].join();
  cerr << "Models built ";
}

template <typename T, typename U>
inline void FCM::createDS (const string& ref, T mask, U& container) {
  ifstream rf(ref);
  char c;
  for (T ctx=0; rf.get(c);) {
    if (c != '\n') {
      ctx = ((ctx<<2) & mask) | NUM[c];  // Update ctx
      container->update(ctx);
    }
  }
  rf.close();
}

//void FCM::compress (const Param& p) const {
//  cerr << "Compressing...\n";
//  array<u64, 4> aN64{0};    // Array of number of elements
//  array<u32, 4> aN32{0};
//  for (auto m : model) {
//    auto mask32 = static_cast<u32>((1<<(m.k<<1))-1);  // 4<<2k - 1 = 4^(k+1) - 1
//    auto mask64 = static_cast<u64>((1<<(m.k<<1))-1);
//    switch (m.mode) {
//      case MODE::TABLE_64:    compressDS(p.tar, m, mask32, aN64, tbl64);  break;
//      case MODE::TABLE_32:    compressDS(p.tar, m, mask32, aN64, tbl32);  break;
//      case MODE::LOG_TABLE_8: compressDS(p.tar, m, mask32, aN64, logtbl8);break;
//      case MODE::SKETCH_8:    compressDS(p.tar, m, mask64, aN32, sketch4);break;
//      default:                cerr << "Error";
//    }
//  }
//
////  //todo. test multithreading
////  vector<thread> thrd;  thrd.resize(4);
////  array<u64, 4> aN64{0};    // Array of number of elements
////  array<u32, 4> aN32{0};
////  for (auto m : model) {
////    auto mask32 = static_cast<u32>((1<<(m.k<<1))-1);  // 4<<2k - 1 = 4^(k+1) - 1
////    auto mask64 = static_cast<u64>((1<<(m.k<<1))-1);
////    switch (m.mode) {
////      case MODE::TABLE_64:
////        thrd[0] = thread(&FCM::compressDS<u32,array<u64,4>,Table64*>, this,
////                         p.tar, m, mask32, aN64, tbl64);
////        break;
////      case MODE::TABLE_32:
////        thrd[1] = thread(&FCM::compressDS<u32,array<u64,4>,Table32*>, this,
////                         p.tar, m, mask32, aN64, tbl32);
////        break;
////      case MODE::LOG_TABLE_8:
////        thrd[2] = thread(&FCM::compressDS<u32,array<u64,4>,LogTable8*>, this,
////                         p.tar, m, mask32, aN64, logtbl8);
////        break;
////      case MODE::SKETCH_8:
////        thrd[3] = thread(&FCM::compressDS<u64,array<u32,4>,CMLS4*>, this,
////                         p.tar, m, mask64, aN32, sketch4);
////        break;
////      default:    cerr << "Error.";
////    }
////  }
////  for (u8 t=0; t!=4; ++t)  if (thrd[t].joinable()) thrd[t].join();
////  cerr << "Compression finished ";
//}

//void FCM::compress (const Param& p) const {
//  cerr << "Compressing...\n";
////  array<u64, 4> aN64{0};    // Array of number of elements
////  array<u32, 4> aN32{0};
////  for (const auto& m : model) {
////    auto mask32 = static_cast<u32>((1 << (m.k << 1))-1);  // 4<<2k - 1 = 4^(k+1) - 1
////    auto mask64 = static_cast<u64>((1 << (m.k << 1))-1);
//////    switch (m.mode) {
//////      case MODE::TABLE_64:    compressDS(p.tar, m, mask32, aN64, tbl64);  break;
//////      case MODE::TABLE_32:    compressDS(p.tar, m, mask32, aN64, tbl32);  break;
//////      case MODE::LOG_TABLE_8: compressDS(p.tar, m, mask32, aN64, logtbl8);break;
//////      case MODE::SKETCH_8:    compressDS(p.tar, m, mask64, aN32, sketch4);break;
//////      default:                cerr << "Error";
//////    }
////  }
////  if (model.size() == 1)
//
////  auto mask = 0;
////  if (typeid(sketch4)==typeid(CMLS4))
////    mask = const_cast<u64>((1 << (model[0].k << 1))-1);
////  else
////    mask = dynamic_cast<u32>((1 << (model[0].k << 1))-1);
////
////  cerr << typeid(mask).name();
//
//
////  compressDS1(p.tar);
////  cerr << "Compression finished ";
//}

void FCM::compress (const Param& p) const {
  cerr << "Compressing...\n";
  array<u32, 4> aN32{};    // Array of number of elements
  array<u64, 4> aN64_0{};
  array<u64, 4> aN64_1{};
  array<u64, 4> aN64_2{};
  vector<u32> mask32;  mask32.resize(model.size());
  u64 mask64 = 0;
  for (const auto& m : model) {
    mask32.emplace_back(static_cast<u32>((1<<(m.k<<1))-1));//1<<2k-1=4^k-1
    if (m.mode==MODE::SKETCH_8)  mask64=static_cast<u64>((1<<(m.k<<1))-1);
  }
  
  switch (MODE_COMB) {
    case 1:   compressDS1(p.tar, mask32[0], aN64_0, tbl64);               break;
    case 2:   compressDS1(p.tar, mask32[0], aN64_0, tbl32);               break;
    case 4:   compressDS1(p.tar, mask32[0], aN64_0, logtbl8);             break;
    case 8:   compressDS1(p.tar, mask64,    aN32,   sketch4);             break;
    case 3:   compressDS2(p.tar, mask32[0], mask32[1], aN64_0, aN64_1,    
                          tbl64, tbl32);                                  break;
    case 5:   compressDS2(p.tar, mask32[0], mask32[1], aN64_0, aN64_1,    
                          tbl64, logtbl8);                                break;
    case 9:   compressDS2(p.tar, mask32[0], mask64, aN64_0, aN32,         
                          tbl64, sketch4);                                break;
    case 6:   compressDS2(p.tar, mask32[0], mask32[1], aN64_0, aN64_1,    
                          tbl32, logtbl8);                                break;
    case 10:  compressDS2(p.tar, mask32[0], mask64, aN64_0, aN32,         
                          tbl32, sketch4);                                break;
    case 12:  compressDS2(p.tar, mask32[0], mask64, aN64_0, aN32,         
                          logtbl8, sketch4);                              break;
//    case 7:   compressDS3(p.tar, mask32[0], mask32[1], mask32[3], aN64_0,
//                          aN64_1, aN64_2, tbl64, tbl32, logtbl8);         break;
//    case 11:  compressDS3(p.tar, mask32[0], mask32[1], mask64, aN64_0,
//                          aN64_1, aN32, tbl64, tbl32, sketch4);           break;
//    case 13:  compressDS3(p.tar, mask32[0], mask32[1], mask64, aN64_0,
//                          aN64_1, aN32, tbl64, logtbl8, sketch4);         break;
//    case 14:  compressDS3(p.tar, mask32[0], mask32[1], mask64, aN64_0,
//                          aN64_1, aN32, tbl32, logtbl8, sketch4);         break;
//    case 15:  compressDS4(p.tar, tbl64, tbl32, logtbl8, sketch4);         break;
    default:  cerr << "Error";                                            break;
  }
  
//  cerr << "Compression finished ";
}

template <typename mask_t, typename cnt_t, typename ds_t>
inline void FCM::compressDS1 (const string& tar, mask_t mask, cnt_t& aN,
                              const ds_t& container) const {
  auto     shl    = model[0].k<<1;  // Shift left
  mask_t   ctx    = 0;         // Context(s) (integer) sliding through the dataset
  mask_t   ctxIR  = mask;      // Inverted repeat context (integer)
  u64      symsNo = 0;         // No. syms in target file, except \n
  float    alpha  = model[0].alpha;
  double   sAlpha = ALPH_SZ*alpha;  // Sum of alphas
  double   sEntr  = 0;         // Sum of entropies = sum( log_2 P(s|c^t) )
  ifstream tf(tar);
  char     c;
  while (tf.get(c)) {
    if (c != '\n') {
      ++symsNo;
      /*
       * double prob()
       */
      auto numSym = NUM[c];
      // Inverted repeat
      if (model[0].ir) {
        auto r = ctxIR>>2;
        for (u8 i=0; i!=ALPH_SZ; ++i)
          aN[i] = container->query((IRMAGIC-i)<<shl | r);
        ctxIR = REVNUM[c]<<shl | r;          // Update ctxIR
      }
      auto l = ctx<<2;
      for (u8 i=0; i!=ALPH_SZ; ++i)
        aN[i] += container->query(l | i);
      ctx = (l & mask) | numSym;       // Update ctx


      sEntr += log2((aN[0]+aN[1]+aN[2]+aN[3]+sAlpha) / (aN[numSym]+alpha));
    }
  }
  tf.close();
  double aveEntr = sEntr/symsNo;
  cerr << "Average Entropy (H) = " << aveEntr << '\n';
  cerr << "Compression finished ";
}

template <typename mask0_t, typename mask1_t, typename cnt0_t, typename cnt1_t,
          typename ds0_t, typename ds1_t>
inline void FCM::compressDS2 (const string& tar, mask0_t mask0, mask1_t mask1,
                              cnt0_t& aN0, cnt1_t& aN1, const ds0_t& container0,
                              const ds1_t& container1) const {
  const auto   shl0    {model[0].k<<1};   // Shift left
  const auto   shl1    {model[1].k<<1};
  mask0_t      ctx0    {0};      // Context(s) (integer) sliding through the dataset
  mask0_t      ctxIR0  {mask0};  // Inverted repeat context (integer)
  mask1_t      ctx1    {0};
  mask1_t      ctxIR1  {mask1};
  u64          symsNo  {0};               // No. syms in target file, except \n
  const float  alpha0  {model[0].alpha};
  const float  alpha1  {model[1].alpha};
  const double sAlpha0 {ALPH_SZ*alpha0};  // Sum of alphas
  const double sAlpha1 {ALPH_SZ*alpha1};
  double       w0      {0.5};
  double       w1      {0.5};
  double       Pm0     {};
  double       Pm1     {};
  double       P       {};
  double       sEnt    {0};              // Sum of entropies = sum(log_2 P(s|c^t))
  const bool   ir0     {model[0].ir};
  const bool   ir1     {model[1].ir};
  ifstream tf(tar);
  char     c;
  while (tf.get(c)) {
    if (c != '\n') {
      ++symsNo;
      /*
       * double prob()
       */
      // Inverted repeat
      if (ir0 && !ir1) {
        const auto r = ctxIR0>>2;
        for (u8 i=0; i!=ALPH_SZ; ++i)
          aN0[i] = container0->query((IRMAGIC-i)<<shl0 | r);
        ctxIR0 = REVNUM[c]<<shl0 | r;          // Update ctxIR
      }
      else if (!ir0 && ir1) {
        const auto r = ctxIR1>>2;
        for (u8 i=0; i!=ALPH_SZ; ++i)
          aN1[i] = container1->query((IRMAGIC-i)<<shl1 | r);
        ctxIR1 = REVNUM[c]<<shl1 | r;          // Update ctxIR
      }
      else if (ir0 && ir1) {
        const auto r0 = ctxIR0>>2;
        const auto r1 = ctxIR1>>2;
        for (u8 i=0; i!=ALPH_SZ; ++i) {
          aN0[i] = container0->query((IRMAGIC-i)<<shl0 | r0);
          aN1[i] = container1->query((IRMAGIC-i)<<shl1 | r1);
        }
        ctxIR0 = REVNUM[c]<<shl0 | r0;          // Update ctxIR
        ctxIR1 = REVNUM[c]<<shl1 | r1;
      }
      const auto l0 = ctx0<<2;
      const auto l1 = ctx1<<2;
      for (u8 i=0; i!=ALPH_SZ; ++i) {
        aN0[i] += container0->query(l0 | i);
        aN1[i] += container1->query(l1 | i);
      }
      const auto numSym = NUM[c];
      ctx0 = (l0 & mask0) | numSym;    // Update ctx
      ctx1 = (l1 & mask1) | numSym;

      Pm0 = (aN0[numSym]+alpha0) / (aN0[0]+aN0[1]+aN0[2]+aN0[3]+sAlpha0);
      Pm1 = (aN1[numSym]+alpha1) / (aN1[0]+aN1[1]+aN1[2]+aN1[3]+sAlpha1);
      P   = Pm0*w0 + Pm1*w1;
      const auto rawW0 = pow(w0, DEF_GAMMA) * Pm0;
      const auto rawW1 = pow(w1, DEF_GAMMA) * Pm1;
      w0  = rawW0 / (rawW0+rawW1);
      w1  = rawW1 / (rawW0+rawW1);
      sEnt += log2(1/P);
    }
  }
  tf.close();
  double aveEnt = sEnt/symsNo;
  cerr << "Average Entropy (H) = " << aveEnt << '\n';
  cerr << "Compression finished ";
}


//#include <tuple>
//#include <typeinfo>
//#include <variant>
////inline void FCM::compressDS1 (const string& tar) const {
//template <typename T, typename Y, typename U>
//inline void FCM::compressDS1 (const string& tar, const ModelPar& mdl, T mask,
//                              Y& aN,/*Y aN,*/ const U& container) const {
////std::variant<Table64*, Table32*, LogTable8*, CMLS4*> v[4];
////  v[0]=tbl64;
////  v[1]=sketch4;
////  v[2]=logtbl8;
////  v[3]=sketch4;
////  for (u8 i = 0; i<model.size(); ++i) {
//////  auto a=std::get<0>(v[0]);
//////  auto b=std::get<1>(v[1]);
////    cerr<<v[1].index();
//////  std::get<i>(v[i])->print();
//////    a->print();
////  }
//
//  //todo. if model.size=1 compressDS1(), else if 2 compressDS2(), ...
//
//
//  auto   shl    = mdl.k<<1;  // Shift left
//  T      ctx    = 0;         // Context(s) (integer) sliding through the dataset
//  T      ctxIR  = mask;      // Inverted repeat context (integer)
//  u64    symsNo = 0;         // No. syms in target file, except \n
//  float  alpha  = mdl.alpha;
//  double sAlpha = ALPH_SZ*alpha;  // Sum of alphas
//  double sEntr  = 0;         // Sum of entropies = sum( log_2 P(s|c^t) )
//  ifstream tf(tar);
//  char c;
//  while (tf.get(c)) {
//    if (c != '\n') {
//      ++symsNo;
//      /*
//       * double prob()
//       */
//      auto numSym = NUM[c];
//      // Inverted repeat
//      if (mdl.ir) {
//        auto r = ctxIR>>2;
//        for (u8 i=0; i!=ALPH_SZ; ++i)
//          aN[i] = container->query((IRMAGIC-i)<<shl | r);
//        ctxIR = REVNUM[c]<<shl | r;          // Update ctxIR
//      }
//      auto l = ctx<<2;
//      for (u8 i=0; i!=ALPH_SZ; ++i)
//        aN[i] += container->query(l | i);
//      ctx = (l & mask) | numSym;       // Update ctx
//
//
//
//      sEntr += log2((aN[0]+aN[1]+aN[2]+aN[3]+sAlpha) / (aN[numSym]+alpha));
//    }
//  }
//  tf.close();
//  double aveEntr = sEntr/symsNo;
//  cerr << "Average Entropy (H) = " << aveEntr << '\n';
//  cerr << "Compression finished ";
//}

////#include <typeinfo>
//template <typename T, typename Y, typename U>
//inline void FCM::compressDS (const string& tar, const ModelPar& mdl, T mask,
//                             Y& aN,/*Y aN,*/ const U& container) const {
//  auto   shl    = mdl.k<<1;  // Shift left
//  T      ctx    = 0;         // Context(s) (integer) sliding through the dataset
//  T      ctxIR  = mask;      // Inverted repeat context (integer)
//  u64    symsNo = 0;         // No. syms in target file, except \n
//  float  alpha  = mdl.alpha;
//  double sAlpha = ALPH_SZ*alpha;  // Sum of alphas
//  double sEntr  = 0;         // Sum of entropies = sum( log_2 P(s|c^t) )
//  ifstream tf(tar);
//  char c;
////  if (typeid(container) == typeid(Table64*)) {
//  while (tf.get(c)) {
//    if (c != '\n') {
//      ++symsNo;
//      auto numSym = NUM[c];
//      // Inverted repeat
//      if (mdl.ir) {
//        auto r = ctxIR>>2;
//        for (u8 i=0; i!=ALPH_SZ; ++i)
//          aN[i] = container->query((IRMAGIC-i)<<shl | r);
////
////        std::thread thr[4];
////        thr[0] = std::thread(&Table64::query, container, std::ref(aN), 0, 3<<shl | r);
////        thr[1] = std::thread(&Table64::query, container, std::ref(aN), 1, 2<<shl | r);
////        thr[2] = std::thread(&Table64::query, container, std::ref(aN), 2, 1<<shl | r);
////        thr[3] = std::thread(&Table64::query, container, std::ref(aN), 3, 0<<shl | r);
////        thr[0].join();
////        thr[1].join();
////        thr[2].join();
////        thr[3].join();
////
////        aN[0] = container->query(3<<shl | r);    // A
////        aN[1] = container->query(2<<shl | r);    // C
////        aN[2] = container->query(1<<shl | r);    // G
////        aN[3] = container->query(r);             // T
//        ctxIR = REVNUM[c]<<shl | r;          // Update ctxIR
//      }
//      auto l = ctx<<2;
//      for (u8 i=0; i!=ALPH_SZ; ++i)
//        aN[i] += container->query(l | i);
////
////      aN[0] += container->query(l);        // A
////      aN[1] += container->query(l | 1);    // C
////      aN[2] += container->query(l | 2);    // G
////      aN[3] += container->query(l | 3);    // T
//      ctx = (l & mask) | numSym;       // Update ctx
//
//      sEntr += log2((aN[0]+aN[1]+aN[2]+aN[3]+sAlpha) / (aN[numSym]+alpha));
//    }
//  }
////  }
//  tf.close();
//  double aveEntr = sEntr/symsNo;
//  cerr << "Average Entropy (H) = " << aveEntr << '\n';
//  cerr << "Compression finished ";
//}

//template <typename T>
//void FCM::prob (T ds) const {
//  cerr<<"hi";
////  ds.print();
//}