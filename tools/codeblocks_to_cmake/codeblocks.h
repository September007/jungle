#pragma once
#include <string>
#include <vector>
using std::string;
using std::vector;

namespace codeblocks
{
    enum E_Build_Type{
        GUI_APP,
        CONSOLE_APP,
        STATIC_APP,
        DYNAMIC_APP,
        COMMANDS_ONLY,
        NATIVE,
        numOfBuild_Type
    };
    [[maybe_unused]]
    enum E_PCH_Mode{

    };
    struct Compiler_Option
    {
        vector<string> options;
        vector<string> directorys;
    };
    struct Linker_Option{
        vector<string> library;
        vector<string> directorys;
    };
    struct Configuration_Target
    {
        string title;
        string output_name;
        string object_output_dir;
        E_Build_Type type;
        string compiler;
        
        Compiler_Option compile_option; 
        Linker_Option linker_option;
    };
    struct Unit{
        string file_name;
        //todo filter
    };
    struct Build_Set{
        vector<Configuration_Target> configuration_targets;
        Compiler_Option compiler_option;
        Linker_Option linker_Option;
        vector<Unit> units;
    };
    [[maybe_unused]]
    struct Extensions{};
    struct Project{
        string title;
        E_PCH_Mode pch_mode;
        string compiler;
        Extensions extensions;
    };
}