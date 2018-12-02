#ifndef PROJECT_NAMING_HPP
#define PROJECT_NAMING_HPP

#include "def.hpp"

namespace smashpp {
inline static string gen_name (u32 ID, const string& ref, const string& tar,
const Format& frmt) {
  switch (frmt) {
  case Format::PROFILE:   return to_string(ID)+"-"+ref+"_"+tar+"."+FMT_PRF;
  case Format::FILTER:    return to_string(ID)+"-"+ref+"_"+tar+"."+FMT_FIL;
  case Format::POSITION:  return to_string(ID)+"-"+ref+"_"+tar+"."+FMT_POS;
  case Format::SEGMENT:   return to_string(ID)+"-"+ref+"_"+tar+"-"+LBL_SEG;
  default:                return "";
  }
}
}

#endif //PROJECT_NAMING_HPP