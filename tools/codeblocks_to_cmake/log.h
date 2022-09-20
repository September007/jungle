#pragma once
// log
#include <fmt/format.h>
#include <iostream>
#include <string>
inline auto log(std::string const &s)
{
    std::cout << s << std::endl;
}
#define LOG(msg) log(format("-- {}; {}:{}", msg, __FILE__, __LINE__))