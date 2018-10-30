#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include "defs.h"
#include "mem.h"
#include "common.h"
#include "msg.h"
#include <fstream>
#include <iostream>
#include "../fn.hpp"
using namespace std;
using namespace smashpp;

u32 smashpp::ArgsNum (u32 d, char* a[], int n, string s, u32 l, u32 u) {
  u32 x;
  for (; --n;)
    if (s == string(a[n])) {
      if ((x=atol(a[n+1])) < l || x > u)
        error("invalid number! Interval: ["+to_string(l)+";"+to_string(u)+"].");
      return x;
    }
  return d;
}

bool smashpp::ArgsState (bool def, char* arg[], int n, string str) {
  for (; --n;)
    if (str == string(arg[n]))
      return !def;
  return def;
}

string smashpp::ArgsFilesImg (char* arg[], int argc, string str) {
  for (i32 n=argc; --n;)
    if (str == string(arg[n]))
      return string(arg[n+1]);
  return "map.svg";
}