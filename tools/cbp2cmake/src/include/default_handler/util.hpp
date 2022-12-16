#pragma once
#include <string>
#include <boost/filesystem.hpp>

namespace fs=boost::filesystem;
struct CurrentPathOnce{
    boost::filesystem::path oldpath;
    CurrentPathOnce(const std::string  &path){
        oldpath=boost::filesystem::current_path();
        boost::filesystem::current_path(path);
    }
    ~CurrentPathOnce(){
        boost::filesystem::current_path(oldpath);
    }
};
