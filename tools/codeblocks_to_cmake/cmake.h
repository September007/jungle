#pragma once
#include <string>
#include <vector>
#include <set>
#include <pugixml.hpp>
using std::set;
using std::string;
using std::vector;

namespace cmake
{
    namespace PROPERTY
    {
        struct Directory
        {
            set<string> INCLUDE_DIRECTORIES;
            set<string> LINK_OPTIONS;
            set<string> LINK_DIRECTORIES;
            set<string> COMPILE_DEFINITIONS;
            set<string> COMPILE_OPTIONS;
        };
        struct Target
        {
            string OUTPUT_NAME;
            string RUNTIME_OUTPUT_DIRECTORY;
            // compiling
            set<string> SOURCES;
            vector<string> INCLUDE_DIRECTORIES;
            vector<string> COMPILE_OPTIONS;
            vector<string> COMPILE_DEFINITIONS;

            // linking
            set<string> LINK_LIBRARIES;
            set<string> LINK_DIRECTORIES;
            set<string> LINK_OPTIONS;
        };
    }
    struct project{
        PROPERTY::Directory directory_property;
        vector<PROPERTY::Target> targets;  
    };

    struct Context{
        // top level cmake file dir
        string cmake_top_level_dir;
        // current cmake file dir
        string cmake_current_dir;

    };
} // namespace  cmake
