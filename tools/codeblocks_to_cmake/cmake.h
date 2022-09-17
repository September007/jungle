#pragma once
#include <string>
#include <vector>
#include <set>
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
            // compiling
            set<string> SOURCES;
            set<string> INCLUDE_DIRECTORIES;
            set<string> COMPILE_OPTIONS;
            set<string> COMPILE_DEFINITIONS;

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
} // namespace  cmake
