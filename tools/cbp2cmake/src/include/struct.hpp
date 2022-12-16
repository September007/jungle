#pragma once
#include <boost/describe.hpp>
#include <boost/json.hpp>
#include <boost/mp11.hpp>
#include <set>
#include <type_traits>
using std::set;
using std::string;
using std::vector;
namespace CTC
{
/* a node could be a target or a set of target(maybe zero target) */
BOOST_DEFINE_ENUM_CLASS (Node_Type, Target, Set);
BOOST_DEFINE_ENUM_CLASS (Target_Type, Unknown, Exe, Static, Dynamic, Object, Interface);
BOOST_DEFINE_ENUM_CLASS(Status_t,SUCCESS,FAILED,UnHandled);
BOOST_DEFINE_ENUM_CLASS(Connect_Type,Contains);

struct Node
{
  std::string name;
  Node_Type type;
  std::string content;
  std::string location;
  std::vector<string> targets;
  auto operator<=>(const Node&r)const=default;
};
BOOST_DESCRIBE_STRUCT(Node,(),(name,type,content,location,targets));


namespace Cbp
{
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

struct Unit
{
  string file_name;
  // todo more filter
  vector<string> bundled_targets;
  string compilerVar;
  bool compiler;
  bool link;
};
struct Cbp_Target
{
  string title;
  string output_name;
  string object_output_dir;
  Target_Type type;
  string compiler;

  Compiler_Option compile_option;
  Linker_Option linker_option;
};

BOOST_DEFINE_ENUM_CLASS (E_PCH_Mode);
struct [[maybe_unused]] Extensions
{
};
struct Build_Set
{
  vector<Cbp_Target> configuration_targets;
};
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
// clang-format off
BOOST_DESCRIBE_STRUCT (Compiler_Option, (), (options, directorys));
BOOST_DESCRIBE_STRUCT (Linker_Option, (), (library, directorys, options));
BOOST_DESCRIBE_STRUCT (Unit, (), (file_name, bundled_targets, compilerVar, compiler, link));
BOOST_DESCRIBE_STRUCT (Cbp_Target, (),(title, output_name, object_output_dir, type, compiler, compile_option, linker_option));
BOOST_DESCRIBE_STRUCT (Extensions, (), ());
BOOST_DESCRIBE_STRUCT (Build_Set, (), (configuration_targets));
BOOST_DESCRIBE_STRUCT (Project, (),(title, pch_mode, compiler, extensions, build_set, compiler_option, linker_Option, units,cbp_source_path));
BOOST_DESCRIBE_STRUCT (Context, (), (cbp_path));
// clang-format on
} // namespace Cbp

namespace Cmake
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
struct CommonProperty
{
  vector<string> INCLUDE_DIRECTORIES;
  vector<string> LINK_OPTIONS;
  vector<string> LINK_DIRECTORIES;
  vector<string> COMPILE_DEFINITIONS;
  vector<string> COMPILE_OPTIONS;
};
struct Directory : CommonProperty
{
};
// path is this are all relative-path base on cmCtx.cmake_current_dir
struct Target : CommonProperty
{
  string OUTPUT_NAME;
  string RUNTIME_OUTPUT_DIRECTORY;
  // compiling
  set<string> SOURCES;
  // linking
  vector<string> LINK_LIBRARIES; // link_library for directory is not recommand

  struct Cbp::Context cbCtx;
  struct Cmake::Context cmCtx;

  // take cbpFile.filename
  auto
  get_tarName ()
  {
    return fs::path (cbCtx.cbp_path).filename ().string ();
  }
  // copy from codeblocks.h :: E_Buil_Type
  Target_Type target_tpye;
};

// clang-format off
BOOST_DESCRIBE_STRUCT (CommonProperty, (), (INCLUDE_DIRECTORIES, LINK_OPTIONS, LINK_DIRECTORIES, COMPILE_DEFINITIONS, COMPILE_OPTIONS));
BOOST_DESCRIBE_STRUCT (Directory, (CommonProperty), ());
BOOST_DESCRIBE_STRUCT (Target, (CommonProperty), (OUTPUT_NAME, RUNTIME_OUTPUT_DIRECTORY, SOURCES, LINK_LIBRARIES, cbCtx, cmCtx, target_tpye));
// clang-format on

} // namespace PROPERTY
struct Project
{
  PROPERTY::Directory directory_property;
  vector<PROPERTY::Target> targets;
  vector<std::string> child_dirs; // Node.name

  // cmake source path
  string cmake_source_path;
  string required_cmake_version = "3.10";
  // generate project(***), if this is null,will take cmake_source_path.filename
  string cmake_project_name;
  // trace version
  string project_version = "1.1";
  string project_description = "none";
};

// clang-format off
BOOST_DESCRIBE_STRUCT (Context, (), (cmake_top_level_dir, cmake_current_dir));
BOOST_DESCRIBE_STRUCT (Project, (), (directory_property, targets, child_dirs,cmake_source_path,required_cmake_version,cmake_project_name,project_version,project_description));
//BOOST_DESCRIBE_STRUCT (, (), ());
// clang-format on

} // namespace Cmake
}
