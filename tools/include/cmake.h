#pragma once
#include <string>
#include <vector>
#include <set>
#include <pugixml.hpp>
#include <memory>
#include <version.h>
#include <filesystem>
#include <log.h>
#include <codeblocks.h>
using std::set;
using std::string;
using std::vector;
namespace fs = std::filesystem;
namespace cmake
{
    struct Context
    {
        // top level cmake file dir
        string cmake_top_level_dir;
        // current cmake file dir
        string cmake_current_dir;
    };
    namespace PROPERTY
    {
        struct Directory
        {
            vector<string> INCLUDE_DIRECTORIES;
            vector<string> LINK_OPTIONS;
            vector<string> LINK_DIRECTORIES;
            vector<string> COMPILE_DEFINITIONS;
            vector<string> COMPILE_OPTIONS;
        };
        // path is this are all relative-path base on cmCtx.cmake_current_dir
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
            vector<string> LINK_LIBRARIES;
            vector<string> LINK_DIRECTORIES;
            vector<string> LINK_OPTIONS;

            //
            struct codeblocks::Context cbCtx;
            struct cmake::Context cmCtx;

            // take cbpFile.filename
            auto get_tarName()
            {
                return fs::path(cbCtx.cbp_path).filename().string();
            }
            // copy from codeblocks.h :: E_Buil_Type
            enum E_Target_Type
            {
                GUI_APP,
                CONSOLE_APP,
                STATIC_APP,
                DYNAMIC_APP,
                COMMANDS_ONLY,
                NATIVE,
                // more
                MODULE,
                numOfBuild_Type
            } target_tpye;
        };
    } // namespace PROPERTY
    struct project
    {
        PROPERTY::Directory directory_property;
        vector<PROPERTY::Target> targets;
        vector<std::shared_ptr<project>> child_dirs;

        // cmake source path
        string cmake_source_path;
        string required_cmake_version = "3.20";
        // generate project(***), if this is null,will take cmake_source_path.filename
        string cmake_project_name;
        // trace version
        string project_version = "1.1";
        string project_description = "none";
        // cause this have a child_dirs which is array of unique_ptrs
        project deep_copy()
        {
            project ret;
            ret.directory_property = this->directory_property;
            ret.targets = this->targets;
            ret.child_dirs.reserve(this->child_dirs.size());
            for (int i = 0; i < this->child_dirs.size(); ++i)
                ret.child_dirs.emplace_back(new cmake::project(this->child_dirs[i]->deep_copy()));
            return ret;
        }
    };

    namespace Helper
    {
        string PropertyToStr(string prefix, string property)
        {
            return format("set_property({} {})\n", prefix, property);
        }
        string PropertyToStr(string prefix, vector<string> properties,string property_prefix)
        {
            string ret;
            if (properties.size() == 0)
                return ret;
            ret = format("set_property({}", prefix);
            for (auto &pro : properties)
                ret += format("\n\t{}{}",property_prefix, pro);
            ret += ")\n";
            return ret;
        }

        // dump cmake project to string
        string dump_Project(project proj)
        {
            string ret;
            // clang-format off
        // 1. dump generator tag
        string genrator_tag = format(
R"(# this is generator by {}
# description:  {}
# version:      {}
)",project_name, project_DESCRIPTION, project_VERSION);

        // 2. dump require_vesion and project 
        // note: project name will take cmake_source_path.filename in case if proj.cmake_project_name is empty
        string cmake_source_path_filename=fs::path(proj.cmake_source_path).filename();
        string vesion_plus_project = format(
            R"(cmake_minimum_required (VERSION {})
project({} 
    VERSION "{}"
    DESCRIPTION "{}" )
)"
    ,proj.required_cmake_version 
    ,(proj.cmake_project_name.size() != 0) ? proj.cmake_project_name : cmake_source_path_filename
    ,proj.project_version
    ,proj.project_description);

            // clang-format on
            // 3. sub_directorys
            string sub_directorys;
            for (auto &dir : proj.child_dirs)
                sub_directorys += format("add_subdirectory({})\n",
                                         fs::relative(
                                             fs::path(dir->cmake_source_path).parent_path(),
                                             fs::path(proj.cmake_source_path).parent_path())
                                             .string());

            // 4. add targets declaration not lib dependencies and sources dependencies
            // note here will sort targets by its name in alphabtize order
            auto tars = proj.targets;
            sort(tars.begin(), tars.end(), [](cmake::PROPERTY::Target const &l, cmake::PROPERTY::Target const &r)
                 { return l.OUTPUT_NAME < r.OUTPUT_NAME; });
            string targets_delcaration;
            for (auto &tar : tars)
            {
                string part;
                switch (tar.target_tpye)
                {
                case cmake::PROPERTY::Target::E_Target_Type::STATIC_APP:
                    part = format("add_library({} STATIC)\n", tar.get_tarName());
                    break;
                case cmake::PROPERTY::Target::E_Target_Type::DYNAMIC_APP:
                    part = format("add_library({} SHARED)\n", tar.get_tarName());
                    break;
                case cmake::PROPERTY::Target::E_Target_Type::MODULE:
                    part = format("add_library({}} MODULE)\n", tar.get_tarName());
                    break;
                case cmake::PROPERTY::Target::E_Target_Type::GUI_APP:
                case cmake::PROPERTY::Target::E_Target_Type::CONSOLE_APP:
                    part = format("add_executable({})\n", tar.get_tarName());
                    break;
                default:
                    LOG(format("unsupported target_type value: {} ", int(tar.target_tpye)));
                    break;
                }
                targets_delcaration += part;
            }

            // 5. targets output-dir & dependencies. inlcuding sources and libs,
            // and compile options and linker options
            string target_dependencies;
            
            for (auto &tar : tars)
            {
                string target_outputdir;
                switch (tar.target_tpye)
                {
                case cmake::PROPERTY::Target::E_Target_Type::STATIC_APP:
                case cmake::PROPERTY::Target::E_Target_Type::DYNAMIC_APP:
                    target_outputdir = format("set_property(TARGET {} PROPERTY LIBRARY_OUTPUT_DIRECTORY\n\t\t\t {}/{}) \n", tar.get_tarName(),"${CMAKE_CURRENT_LIST_DIR}",tar.RUNTIME_OUTPUT_DIRECTORY);
                    break;
                case cmake::PROPERTY::Target::E_Target_Type::GUI_APP:
                case cmake::PROPERTY::Target::E_Target_Type::CONSOLE_APP:
                    target_outputdir = format("set_property(TARGET {} PROPERTY RUNTIME_OUTPUT_DIRECTORY\n\t\t\t {}/{}) \n", tar.get_tarName(),"${CMAKE_CURRENT_LIST_DIR}",tar.RUNTIME_OUTPUT_DIRECTORY);
                    break;
                default:
                    LOG(format("unsupported target_type value: {} ", int(tar.target_tpye)));
                    break;
                }
                string part;
                part += format("\n#-----------{}\n", tar.OUTPUT_NAME);
                part += target_outputdir;
                // here assume source are all  proj.cmake_source_path -based relative path
                if (tar.SOURCES.size())
                {
                    part += format("set_property(TARGET {} PROPERTY SOURCES",tar.get_tarName());
                    for (auto &source : tar.SOURCES)
                        part += format("\n\t{}", source);
                    part += format(")\n");
                }
                else
                {
                    LOG(format("target [{}] have none source file", tar.OUTPUT_NAME));
                }
                #define add_property(property,pre) \
                auto property##_str = PropertyToStr(format("TARGET {} PROPERTY "#property, tar.get_tarName()), tar.property,pre);\
                if (property##_str.empty())\
                    LOG(format("target [{}] have none "#property, tar.get_tarName()));\
                else\
                    part += property##_str;
                add_property(INCLUDE_DIRECTORIES,"${CMAKE_CURRENT_LIST_DIR}/");
                add_property(COMPILE_OPTIONS,"");
                add_property(COMPILE_DEFINITIONS,"");

                add_property(LINK_LIBRARIES,"");
                add_property(LINK_DIRECTORIES,"${CMAKE_CURRENT_LIST_DIR}/");
                add_property(LINK_OPTIONS,"");
                target_dependencies += part;
            }
            ret = genrator_tag +
                  vesion_plus_project +
                  sub_directorys +
                  targets_delcaration +
                  target_dependencies;
            return ret;
        }
    } // namespace  helper

} // namespace cmake
