#pragma once
#include "cmake.h"
#include "codeblocks.h"
#include <pugixml.hpp>
#include <filesystem>
#include <functional>

namespace trans
{
    struct Context
    {
        // case insensitive and ignore blankspace
        vector<string> prior_Configuration_Target = {"Release", "Release x64"};
        codeblocks::Context cbCtx;
        cmake::Context cmCtx;
    };
    string tolower(std::string s)
    {
        std::string ret;
        for (auto c : s)
            if (isblank(c))
                continue;
            else
                ret.push_back(std::tolower(c));
        return ret;
    }
    bool prior_Configuration_Target_compare(std::string l, std::string &r)
    {
        auto ll = tolower(l), rr = tolower(r);
        return ll == rr;
    }
}

// only take one,
inline cmake::PROPERTY::Target cbProj_to_cmakTarget(codeblocks::Project proj, trans::Context &cx)
{
    cmake::PROPERTY::Target ret;
    if (proj.build_set.configuration_targets.size() == 0)
    {
        LOG(format("proj:[{}] doesn't have one target configuration", proj.title));
        return ret;
    }
    codeblocks::Configuration_Target ct = proj.build_set.configuration_targets[0];
    // 1. choose one configuration target
    for (auto &prior : cx.prior_Configuration_Target)
        for (auto &ctar : proj.build_set.configuration_targets)
            if (trans::prior_Configuration_Target_compare(prior, ct.title))
            {
                ct = ctar;
                goto choose_out;
            }
choose_out:

    // 2. parse cb.output_name(maybe absolute path) to cm.OUTPUT_NAME
    // and cm.RUNTIME_OUTPUT_DIRECTORY (relative path on cmCtx.cmake_current_dir!)
    ret.OUTPUT_NAME = fs::path(ct.output_name).filename();
    ret.RUNTIME_OUTPUT_DIRECTORY = fs::proximate(fs::path(ct.output_name).parent_path(), cx.cmCtx.cmake_current_dir);

    // 3. compiling attributes: SOURCES INCLUDE_DIRECTORIES COMPILE_OPTIONS COMPILE_DEFINITIONS
    // note: inlcude_dirs and options and definition are all added in order of {target,project}
    // note: for now,here not separate macro definition from COMPILE_OPTIONS to COMPILE_DEFINITIONS.
    for (auto &unit : proj.units)
    {
        if (unit.bundled_targets.size() == 0 || std::find(unit.bundled_targets.begin(), unit.bundled_targets.end(), ct.title) != unit.bundled_targets.end())
            ret.SOURCES.insert(fs::relative(unit.file_name, cx.cbCtx.cbp_path).string());
    };
    for (auto &target_include_dir : ct.compile_option.directorys)
        ret.INCLUDE_DIRECTORIES.push_back(fs::relative(target_include_dir, cx.cbCtx.cbp_path));
    for (auto &target_include_dir : proj.compiler_option.directorys)
        ret.INCLUDE_DIRECTORIES.push_back(fs::relative(target_include_dir, cx.cbCtx.cbp_path));
    
    for(auto &compile_option: ct.compile_option.options)
        // not check repeatness
        ret.COMPILE_OPTIONS.push_back(compile_option);
    for(auto &compile_option: proj.compiler_option.options)
        // not check repeatness
        ret.COMPILE_OPTIONS.push_back(compile_option);

    // 4. linker attributes :LINK_LIBRARIES LINK_DIRECTORIES LINK_OPTIONS 
    for (auto &lib:ct.linker_option.library)
        ret.LINK_LIBRARIES.insert(lib);
    for (auto &lib:proj.linker_Option.library)
        ret.LINK_LIBRARIES.insert(lib);
    
    for(auto &lib_dir:ct.linker_option.directorys)
        ret.LINK_DIRECTORIES.insert(lib_dir);
    for(auto &lib_dir:proj.linker_Option.directorys)
        ret.LINK_DIRECTORIES.insert(lib_dir);

    for(auto &link_option:ct.linker_option.options)
        ret.LINK_OPTIONS.insert(link_option);
    for(auto &link_option:proj.linker_Option.options)
        ret.LINK_OPTIONS.insert(link_option);     
    return ret;
}

inline cmake::project cbp2cmake(codeblocks::Project proj)
{
}