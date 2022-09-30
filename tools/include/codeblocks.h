#pragma once
#include <fmt/format.h>

#include <filesystem>
#include <functional>
#include <pugixml.hpp>
#include <string>
#include <vector>
#include <tuple>

#include <fstream>
#include <sstream>
using fmt::format;
using pugi::xml_attribute;
using pugi::xml_node;
using std::function;
using std::string;
using std::vector;
namespace fs = std::filesystem;
#include <log.h>
struct scope_modify_Current_Path
{
    std::string old_path;
    std::string new_path;
    scope_modify_Current_Path(std::string new_path_) : new_path(new_path_)
    {
        old_path = fs::current_path();
        fs::current_path(new_path);
    }
    ~scope_modify_Current_Path()
    {
        fs::current_path(old_path);
    }
};

namespace codeblocks
{
enum E_Build_Type
{
    GUI_APP,
    CONSOLE_APP,
    STATIC_APP,
    DYNAMIC_APP,
    COMMANDS_ONLY,
    NATIVE,
    numOfBuild_Type
};
 enum [[maybe_unused]] E_PCH_Mode
{
};
struct Compiler_Option
{
    vector<string> options;
    vector<string> directorys;
};
struct Linker_Option
{
    vector<string> library;
    vector<string> directorys;
    vector<string> options;
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
struct Unit
{
    string file_name;
    // todo more filter
    vector<string> bundled_targets;
    string compilerVar;
    bool compiler;
    bool link;
};
struct Build_Set
{
    vector<Configuration_Target> configuration_targets;
};
struct [[maybe_unused]]  Extensions
{
};

// path in this are all absolute-path
struct Project
{
    string title;
    E_PCH_Mode pch_mode;
    string compiler;
    Extensions extensions;

    Build_Set build_set;
    Compiler_Option compiler_option;
    Linker_Option linker_Option;
    vector<Unit> units;

    // trace cbp sourcefile
    string cbp_source_path; 
};
struct Context
{
    string cbp_path;
};
} // namespace codeblocks

namespace Parser
{
auto for_each_child_who_have_attr(xml_node &node, string attr, function<void(xml_node &child)> handler)
{
    for (auto child = node.first_child(); child; child = child.next_sibling())
    {
        if (!child.attribute(attr.c_str()).empty())
            handler(child);
    }
}
std::pair<bool, xml_attribute> get_attrVal_of_only_once_founded_child(xml_node &root_node, string required_attrname, bool if_warn = true)
{
    auto p = root_node.find_child([&](pugi::xml_node &child) { return !child.attribute(required_attrname.c_str()).empty(); });
    if (auto attr = p.attribute(required_attrname.c_str()); !attr.empty())
        return {true, attr};
    if (if_warn)
        LOG(format("can't find attr [{}] of node \n\t content of node\n{}", required_attrname, root_node.name(), root_node.value()));
    return {false, xml_attribute{}};
}
string get_child_attr_val(xml_node &node, string required_child_attr, string default_val)
{
    auto p = get_attrVal_of_only_once_founded_child(node, required_child_attr, false);
    if (p.first)
        return p.second.as_string();
    else
        return default_val;
}
vector<xml_node> get_all_child_which_have_attr(xml_node &node, string require_attr)
{
    vector<xml_node> ret;
    for (auto i = node.first_child(); i; i = i.next_sibling())
    {
        if (!i.attribute(require_attr.c_str()).empty())
            ret.push_back(i);
    }
    return ret;
}

/* ret always store absoulte path and will check its'existence */
auto parse_Compiler_Option(pugi::xml_node node, codeblocks::Context &cx)
{
    // set start-point of relative-path
    auto &cbp_path                                = cx.cbp_path;
    scope_modify_Current_Path modify_current_path = cbp_path;
    codeblocks::Compiler_Option ret;
    // for_each_child_who_have_attr(node, "option",
    //                              [&](xml_node &child)
    //                              {
    //                                  auto str =
    //                                  child.attribute("option").as_string(); if
    //                                  (std::find(ret.options.begin(),
    //                                  ret.options.end(), str) ==
    //                                  ret.options.end())
    //                                      ret.options.push_back(str);
    //                              });
    for (auto i = node.first_child(); i; i = i.next_sibling())
    {
        if (!i.attribute("option").empty())
        {
            auto str = i.attribute("option").as_string();
            if (std::find(ret.options.begin(), ret.options.end(), str) == ret.options.end())
                ret.options.push_back(str);
        }
        else if (!i.attribute("directory").empty())
        {
            auto str = i.attribute("directory").as_string();
            // always store complete path
            std::error_code ec;
            auto absolute_path = fs::canonical(str, ec);
            if (ec)
            {
                LOG(format("path [{}] not exist, cbp in [{}] ", str, cbp_path));
                LOG(ec.message());
                // optional behaviour
                continue;
            }
            if (std::find(ret.directorys.begin(), ret.directorys.end(), absolute_path) == ret.directorys.end())
                ret.directorys.push_back(absolute_path);
        }
    }
    return ret;
}

auto parse_Linker_Option(xml_node node, codeblocks::Context &cx)
{
    string cbp_path                               = cx.cbp_path;
    scope_modify_Current_Path modify_current_path = cbp_path;
    codeblocks::Linker_Option ret;

    for_each_child_who_have_attr(node, "library", [&](xml_node &child) {
        string lib = child.attribute("library").as_string();
        if (std::find(ret.library.begin(), ret.library.end(), lib) == ret.library.end())
            ret.library.push_back(lib);
    });

    for_each_child_who_have_attr(node, "option", [&](xml_node &child) {
        string lib = child.attribute("option").as_string();
        if (std::find(ret.options.begin(), ret.options.end(), lib) == ret.options.end())
            ret.options.push_back(lib);
    });

    for_each_child_who_have_attr(node, "directory", [&](xml_node &child) {
        string lib = child.attribute("directory").as_string();
        std::error_code ec;
        ec.clear();
        fs::canonical(lib, ec);
        if (ec)
        {
            LOG(format("path [{}] not exist, cbp in [{}] ", lib, cbp_path));
            LOG(ec.message());
            return;
        }
        if (std::find(ret.directorys.begin(), ret.directorys.end(), lib) == ret.directorys.end())
            ret.directorys.push_back(lib);
    });

    return ret;
}

auto parse_Configuration_Target(xml_node &node, codeblocks::Context &cx)
{
    string cbp_path = cx.cbp_path;
    codeblocks::Configuration_Target ret;
    ret.title             = node.attribute("title").as_string();
    ret.output_name       = get_attrVal_of_only_once_founded_child(node, "output").second.as_string();
    ret.object_output_dir = get_attrVal_of_only_once_founded_child(node, "object_output").second.as_string();
    ret.compiler          = get_attrVal_of_only_once_founded_child(node, "compiler").second.as_string();
    ret.type              = decltype(ret.type)(get_attrVal_of_only_once_founded_child(node, "type").second.as_int());
    ret.compile_option    = parse_Compiler_Option(node.child("Compiler"), cx);
    ret.linker_option     = parse_Linker_Option(node.child("Linker"), cx);
    return ret;
}
auto parse_Unit(xml_node &node, codeblocks::Context &cx)
{
    codeblocks::Unit ret;
    std::error_code ec;
    string relative_unit_file=node.attribute("filename").as_string();
    {
        scope_modify_Current_Path modi=cx.cbp_path;
        ret.file_name = fs::canonical(relative_unit_file,ec).string();
    }
    if(ec){
        LOG(format("unit file [{}] based on [{}] is unreachable",relative_unit_file,cx.cbp_path));
        LOG(ec.message());
    }
    for_each_child_who_have_attr(node, "target", [&](xml_node &child) {
        auto targetName = child.attribute("target").as_string();
        if (std::find(ret.bundled_targets.begin(), ret.bundled_targets.end(), targetName) == ret.bundled_targets.end())
            ret.bundled_targets.push_back(targetName);
    });
    ret.compilerVar = get_child_attr_val(node, "compilerVar", "CPP");
    ret.compiler    = std::stoi(get_child_attr_val(node, "compiler", "1"));
    ret.link        = std::stoi(get_child_attr_val(node, "link", "1"));
    return ret;
}
auto parse_Build_Set(xml_node node, codeblocks::Context &cx)
{
    codeblocks::Build_Set ret;
    for (auto target_ = node.first_child(); target_; target_ = target_.next_sibling())
    {
        ret.configuration_targets.push_back(parse_Configuration_Target(target_, cx));
    }
    return ret;
}
auto parse_Extensions(xml_node node, codeblocks::Context &cx)
{
    codeblocks::Extensions ret;
    return ret;
}
auto parse_Project(xml_node &node, codeblocks::Context &cx)
{
    codeblocks::Project ret;
    ret.title    = get_attrVal_of_only_once_founded_child(node, "title").second.as_string();
    ret.pch_mode = decltype(ret.pch_mode)(std::stoi(get_child_attr_val(node, "pch_mode", "1")));
    ret.compiler = get_attrVal_of_only_once_founded_child(node, "compiler").second.as_string();

    ret.extensions      = parse_Extensions(node.child("extension"), cx);
    ret.build_set       = parse_Build_Set(node.child("Build"), cx);
    ret.compiler_option = parse_Compiler_Option(node.child("Compiler"), cx);
    ret.linker_Option   = parse_Linker_Option(node.child("Linker"), cx);
    for_each_child_who_have_attr(node, "filename", [&](xml_node &unit) { ret.units.push_back(parse_Unit(unit, cx)); });
    ret.cbp_source_path=cx.cbp_path;
    return ret;
}
} // namespace Parser

inline std::tuple< codeblocks::Project,codeblocks::Context> ParseCodeBlocksProject(string cbp_file_path)
{
    pugi::xml_document cbp;
    if (auto load_result = cbp.load_file(cbp_file_path.c_str()); !load_result)
        {
            // std::ifstream in(cbp_file_path);
            // std::stringstream oo;
            // oo<<in.rdbuf();
            // auto s=oo.str();
            // auto p = cbp.load_string(s.c_str());
            // if(!p)
                return {};
        }
    auto node_proj = cbp.child("CodeBlocks_project_file").child("Project");

    codeblocks::Context cx;
    cx.cbp_path = fs::canonical(cbp_file_path).parent_path();

    codeblocks::Project ret = Parser::parse_Project(node_proj, cx);
    return {ret,cx};
}