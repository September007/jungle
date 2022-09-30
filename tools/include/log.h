#pragma once
// log
#include <fmt/format.h>
#include <iostream>
#include <string>
inline std::ostream* &default_out(){
    static std::ostream* ret=&std::cout;
    return ret;
}
inline auto log(std::string const &s,std::ostream*out=default_out())
{
    (*out) << s << std::endl;
}
#define LOG(msg) log(format("-- {}; {}:{}", msg, fs::relative( __FILE__,CMAKE_CURRENT_LIST_DIR).string(), __LINE__))
#define SLOG(msg,_stream)log(format("-- {}; {}:{}", msg, fs::relative( __FILE__,CMAKE_CURRENT_LIST_DIR).string(), __LINE__),_stream)
