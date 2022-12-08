#pragma once
// log
#include <fmt/format.h>
#include <iostream>
#include <jungle/version.h>
#include <string>

#if defined(RELATIVE_LOG_PATH)
#if __cplusplus >= 201703L
#include <filesystem>
namespace fs = std::filesystem;
#else
#warning "Set RELATIVE_LOG_PATH but not with c++17"
#undef RELATIVE_LOG_PATH
#endif
#endif

inline std::ostream *&default_out() {
  static std::ostream *ret = &std::cout;
  return ret;
}
inline auto log(std::string const &s, std::ostream *out = default_out()) {
  (*out) << s << std::endl;
}
using fmt::format;
#ifdef RELATIVE_LOG_PATH
#define LOG(msg)                                                               \
  log(fmt::format("-- {}; {}:{}", msg,                                              \
             fs::relative(__FILE__, CMAKE_CURRENT_LIST_DIR).string(),          \
             __LINE__))
#define SLOG(msg, _stream)                                                     \
  log(fmt::format("-- {}; {}:{}", msg,                                              \
             fs::relative(__FILE__, CMAKE_CURRENT_LIST_DIR).string(),          \
             __LINE__),                                                        \
      _stream)
#else
#define LOG(msg) log(fmt::format("-- {}; {}:{}", msg, __FILE__, __LINE__))
#define SLOG(msg, _stream)                                                     \
  log(fmt::format("-- {}; {}:{}", msg, __FILE__, __LINE__, _stream))
#endif