#pragma once 
#include <string>
#include <vector>
#include <sstream>
namespace string_util
{
    inline std::vector<std::string> split(std::string in,char delim){
        std::stringstream ss(in);
        std::vector<std::string> ret;
        std::string tmp;
        while(std::getline(ss,tmp,delim))
            ret.push_back(tmp);
        return ret;
    }
} // namespace string_util
