//
// Created by morteza on 27-02-2018.
//

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <limits>
//using namespace std;




//#include <vector>
//using std::vector;
#include "cmls.hpp"

/*
 * Constructors
 * w=[e/eps].      0 < eps   = error factor      < 1
 * d=[ln 1/delta]. 0 < delta = error probability < 1
 */
CMLS::CMLS ()
{
  w   = DEF_W;
  d   = DEF_D;
  tot = 0;
  sk.resize(d, vector<u64>(w));
  ab.reserve(d);
  
  
//  // initialize d pairwise independent hashes
//  srand(time(NULL));
//  hashes = new int* [d];
//  for (i = 0; i < d; i++) {
//    hashes[i] = new int[2];
//    genajbj(hashes, i);
//  }
}

// CountMinSkectch destructor
CMLS::~CMLS() {
//  // free array of hash values
//  for (i = 0; i < d; i++) {
//    delete[] hashes[i];
//  }
//  delete[] hashes;
}

// generates a,b from field Z_p for use in hashing
void CMLS::genajbj(int** hashes, int i) {
  hashes[i][0] = int(float(rand())*float(LONG_PRIME)/float(RAND_MAX) + 1);
  hashes[i][1] = int(float(rand())*float(LONG_PRIME)/float(RAND_MAX) + 1);
}

// CMLS totalcount returns the
// tot count of all items in the sketch
unsigned int CMLS::totalcount() {
  return tot;
}

// countMinSketch update item count (int)
void CMLS::update(int item, int c) {
  tot = tot + c;
  unsigned int hashval = 0;
  for (unsigned int j = 0; j < d; j++) {
    hashval = ((long)ab[j][0]*item+ab[j][1])%LONG_PRIME%w;
    sk[j][hashval] = sk[j][hashval] + c;
  }
}

// countMinSketch update item count (string)
void CMLS::update(const char *str, int c) {
  int hashval = hashstr(str);
  update(hashval, c);
}

// CMLS estimate item count (int)
unsigned int CMLS::estimate(int item) {
  int minval = std::numeric_limits<int>::max();
  unsigned int hashval = 0;
  for (unsigned int j = 0; j < d; j++) {
    hashval = ((long)ab[j][0]*item+ab[j][1])%LONG_PRIME%w;
    minval = MIN(minval, sk[j][hashval]);
  }
  return minval;
}

// CMLS estimate item count (string)
unsigned int CMLS::estimate(const char *str) {
  int hashval = hashstr(str);
  return estimate(hashval);
}


// generates a hash value for a sting
// same as djb2 hash function
unsigned int CMLS::hashstr(const char *str) {
  unsigned long hash = 5381;
  int c;
  while (c = *str++) {
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  }
  return hash;
}