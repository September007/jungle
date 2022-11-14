#pragma once
#include "cmake.h"
#include "codeblocks.h"
#include <pugixml.hpp>
#include <filesystem>
#include <functional>
#include <map>
using std::map;

namespace trans
{
    // context of transing lots codeblocks project to one cmake
    struct Context
    {
        // case insensitive and ignore blankspace
        vector<string> prior_Configuration_Target = {"Release", "Release x64"};
        // key: cbp_source_path , val : corresponding context which makes codeblocks::Project
        map<string, codeblocks::Context> cbCtxs;
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
    auto &currentCtx = cx.cbCtxs[fs::canonical(proj.cbp_source_path)];
    auto &targetCmakeCx = cx.cmCtx;
    if (proj.build_set.configuration_targets.size() == 0)
    {
        LOG(format("proj:[{}] doesn't have one target configuration", proj.title));
        return ret;
    }
    codeblocks::Configuration_Target ct = proj.build_set.configuration_targets[0];
    // 1. choose one configuration target
    for (auto &prior : cx.prior_Configuration_Target)
        for (auto &ctar : proj.build_set.configuration_targets)
            if (trans::prior_Configuration_Target_compare(prior, ctar.title))
            {
                ct = ctar;
                goto choose_out;
            }
choose_out:

    // 2. parse cb.output_name(maybe absolute path) to cm.OUTPUT_NAME
    // and cm.RUNTIME_OUTPUT_DIRECTORY (relative path on cmCtx.cmake_current_dir!)
    ret.OUTPUT_NAME = fs::path(ct.output_name).filename();
    {
        scope_modify_Current_Path __(proj.cbp_source_path);
        auto abs_output_dir=fs::absolute(fs::path(ct.output_name).parent_path());
        ret.RUNTIME_OUTPUT_DIRECTORY= fs::proximate(abs_output_dir,targetCmakeCx.cmake_current_dir);
    }

    // 3. compiling attributes: SOURCES INCLUDE_DIRECTORIES COMPILE_OPTIONS COMPILE_DEFINITIONS
    // note: inlcude_dirs and options and definition are all added in order of {target,project}
    // note: for now,here not separate macro definition from COMPILE_OPTIONS to COMPILE_DEFINITIONS.
    for (auto &unit : proj.units)
    {
        if (unit.bundled_targets.size() == 0 || std::find(unit.bundled_targets.begin(), unit.bundled_targets.end(), ct.title) != unit.bundled_targets.end())
            ret.SOURCES.insert(fs::relative(unit.file_name, targetCmakeCx.cmake_current_dir).string());
    };
    for (auto &target_include_dir : ct.compile_option.directorys)
        ret.INCLUDE_DIRECTORIES.push_back(fs::relative(target_include_dir, targetCmakeCx.cmake_current_dir));
    for (auto &target_include_dir : proj.compiler_option.directorys)
        ret.INCLUDE_DIRECTORIES.push_back(fs::relative(target_include_dir, targetCmakeCx.cmake_current_dir));

    for (auto &compile_option : ct.compile_option.options)
        // not check repeatness
        ret.COMPILE_OPTIONS.push_back(compile_option);
    for (auto &compile_option : proj.compiler_option.options)
        // not check repeatness
        ret.COMPILE_OPTIONS.push_back(compile_option);

    // 4. linker attributes :LINK_LIBRARIES LINK_DIRECTORIES LINK_OPTIONS
    for (auto &lib : ct.linker_option.library)
        ret.LINK_LIBRARIES.push_back(lib);
    for (auto &lib : proj.linker_Option.library)
        ret.LINK_LIBRARIES.push_back(lib);

    for (auto &lib_dir : ct.linker_option.directorys)
        ret.LINK_DIRECTORIES.push_back(lib_dir);
    for (auto &lib_dir : proj.linker_Option.directorys)
        ret.LINK_DIRECTORIES.push_back(lib_dir);

    for (auto &link_option : ct.linker_option.options)
        ret.LINK_OPTIONS.push_back(link_option);
    for (auto &link_option : proj.linker_Option.options)
        ret.LINK_OPTIONS.push_back(link_option);
    // 5. target type
    ret.target_tpye = decltype(ret.target_tpye)(int(ct.type));

    ret.cbCtx = currentCtx;
    ret.cmCtx = targetCmakeCx;

    return ret;
}

// return one single-level project, with selecting some same attr to directory attributes
// note: for now, this is not  work
// todo: extracting COMPILE_DEFINITIONS & COMPILE_OPTIONS
inline cmake::project cmakTargets_to_cmakeProj(vector<cmake::PROPERTY::Target> targets, string cmake_path)
{
    cmake::project ret;
    // vector<string> compile
    ret.targets = targets;
    ret.cmake_source_path = cmake_path;
    return ret;
}

// trans all projs to one multi-level cmake proj
// todo: support config options by pass Context::cmake_generate_configuration
inline cmake::project sCmake_to_Mcmake(vector<cmake::PROPERTY::Target> targets, vector<cmake::project> projs, trans::Context &cx)
{
    cmake::project ret;
    // vector<cmake::PROPERTY::Target> cmake_projs;
    // for(auto & cxProj:projs)
    //     cmake_projs.push_back(cbProj_to_cmakTarget(cxProj,cx));
    ret.child_dirs.resize(projs.size());
    for (int i = 0; i < projs.size(); ++i)
        ret.child_dirs[i].reset(new cmake::project(projs[i].deep_copy()));
    ret.targets = targets;
    return ret;
}
