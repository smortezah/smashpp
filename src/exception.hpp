#ifndef PROJECT_EXCEPTION_HPP
#define PROJECT_EXCEPTION_HPP

#include "def.hpp"
#include "string.hpp"

namespace smashpp {
// "inline" is a MUST -- not to get "multiple definition of `now()'" error
inline static void error (string&& msg) {
  string message = "Error: " + std::move(msg);
  wrap_text(message);
  throw runtime_error (bold_red(message.substr(0,6)) + message.substr(6) +"\n");
}

inline static void err (string&& msg) {
  string message = "Error: " + std::move(msg);
  wrap_text(message);
  cerr << bold_red(message.substr(0,6)) << message.substr(6) << '\n';
}

inline static void warning (string&& msg) {
  string message = "Warning: " + std::move(msg);
  wrap_text(message);
  cerr << bold(message.substr(0,8)) << message.substr(8) << '\n';
}
}

#endif //PROJECT_EXCEPTION_HPP