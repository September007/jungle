#pragma once
#include <codeblocks_to_cmake.h>
#include <log.h>
#include <map>
#include <regex>
struct DepsNode
{
    //default for libs like stdc++
    codeblocks::E_Build_Type node_type=codeblocks::E_Build_Type::NATIVE;
    string name;
    string SrcProjPath;
    vector<string> deps_names;
};
 
namespace
{
    static vector<string> ignoredBeginWith = {
        "lib"};

    static vector<string> ignoredEndWith = {
        ".a", ".so",
        ".lib", ".dll"};
    //
    static vector<string> ignoredTarget = {
        "(.*32)",
        
    };
};

string Trim_Target_Name(string orig)
{
    string ret=orig;
    if(ret.find('/')!=string::npos)
        ret=ret.substr(ret.find_last_of('/')+1);
    if(ret.find('\\')!=string::npos)
        ret=ret.substr(ret.find_last_of('\\')+1);
    
    for(auto &beg:ignoredBeginWith){
        if(ret.compare(0,beg.size(),beg)==0){
            ret=ret.substr(beg.size());
            break; // once matched, just break, do not try to match next
        }
    }
    for(auto &end:ignoredEndWith){
        if(ret.compare(ret.size()-end.size(),end.size(),end)==0){
            ret=ret.substr(0,ret.size()- end.size());
            break; // once matched, just break, do not try to match next
        }
    }
    return ret;
}

DepsNode fromCBPFile(string cbp_file)
{
    DepsNode ret;
    pugi::xml_document cbp;
    codeblocks::Project proj;
    codeblocks::Context cbCx;
    trans::Context cx;
    try
    {

        string target_cmake_dir = fs::path(cbp_file).parent_path().string();
        pugi::xml_parse_result result = cbp.load_file(cbp_file.c_str());
        cx.cmCtx = {target_cmake_dir, target_cmake_dir};
        auto cmake_output_path = cx.cmCtx.cmake_current_dir + "/CMakeLists.txt";
        std::tie(proj, cbCx) = ParseCodeBlocksProject(cbp_file);
        cx.cbCtxs = {{fs::canonical(cbp_file).parent_path().string(), cbCx}};
        auto cmake_target = cbProj_to_cmakTarget(proj, cx);

        ret.node_type = proj.build_set.configuration_targets.back().type;
        ret.name = Trim_Target_Name( cmake_target.OUTPUT_NAME);
        ret.SrcProjPath=proj.cbp_source_path;
        for(auto lib:cmake_target.LINK_LIBRARIES)
            ret.deps_names.push_back(Trim_Target_Name(lib));
        // some lib is pass by compiler-option -lxxxx
        for(auto opt:cmake_target.LINK_OPTIONS)
            if(opt.substr(0,2)=="-l")
                ret.deps_names.push_back(Trim_Target_Name(opt.substr(2)));
        
    }
    catch (std::exception &e)
    {
        LOG(fmt::format(" catch error: {}", e.what()));
    }
    return ret;
}
