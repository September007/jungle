#pragma once

#include <string>
#include <fstream>
#include <jungle/core/log.h>
#include <sstream>
namespace IO{

/* return true if success */
template<typename Ch>
inline int writeFile(std::string const &filename,Ch *data,int size){
    auto fout=std::fstream(filename);
    if(!fout.good())
    {
        LOG(fmt::format("open file {} failed",filename));
        return false;
    }
    auto ret=fout.write(data,size).good();
    if(!ret)
        LOG(fmt::format("write file {} failed",filename));
    return ret==true;
}

/* return true if success */
inline std::string readFile(std::string const &filename){
    std::ifstream in(filename);
    std::stringstream ss;
    ss<<in.rdbuf();
    return ss.str();
}



}

