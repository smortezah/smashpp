#ifndef PROJECT_NAMING_HPP
#define PROJECT_NAMING_HPP

#include "def.hpp"

namespace smashpp {
inline static string gen_name (u32 ID, const string& ref, const string& tar,
const Format& frmt) {
  const auto refName=file_name(ref), tarName=file_name(tar);
  switch (frmt) {
  case Format::PROFILE:   
    return to_string(ID)+"-"+refName+"_"+tarName+"."+FMT_PRF;
  case Format::FILTER:    
    return to_string(ID)+"-"+refName+"_"+tarName+"."+FMT_FIL;
  case Format::POSITION:  
    return to_string(ID)+"-"+refName+"_"+tarName+"."+FMT_POS;
  case Format::SEGMENT:   
    return to_string(ID)+"-"+refName+"_"+tarName+"-"+LBL_SEG;
  default:                
    return "";
  }
}
inline static string gen_name (const string& ref, const string& tar,
const Format& frmt) {
  const auto refName=file_name(ref), tarName=file_name(tar);
  switch (frmt) {
  case Format::PROFILE:   return refName+"-"+tarName+"."+FMT_PRF;
  case Format::FILTER:    return refName+"-"+tarName+"."+FMT_FIL;
  case Format::POSITION:  return refName+"-"+tarName+"."+FMT_POS;
  case Format::SEGMENT:   return refName+"-"+tarName+"-"+LBL_SEG;
  default:                return "";
  }
}
}

#endif //PROJECT_NAMING_HPP