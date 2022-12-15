#pragma once
#include <data_convert.hpp>
#include <functional>
#include <map>
#include <struct.hpp>
#include <vector>
#include <map>
namespace CTC
{

using PackType = cpp_interface::Data::DataItem;
BOOST_DEFINE_ENUM_CLASS (DisPatcher_Msg, Add_Node, AnaLyze, Connect,/* Construct, */ Convert, Extract ,/* Upload ,*/ Dump);

extern std::map<DisPatcher_Msg, std::function<PackType (PackType)> > defaultDispathcerMsgHandler;

namespace PackContent
{

using std::false_type;
using std::string;
using CTC::Status_t;
template <DisPatcher_Msg msg> struct PackContent
{
};
template <> struct PackContent<DisPatcher_Msg::Add_Node>
{
  using Content = Node;
  using Return = Status_t;
};

template <> struct PackContent<DisPatcher_Msg::AnaLyze>
{
  
  using Content = struct 
  {
    string node_name;
  }; 
  using Return = struct {
    CTC::Cbp::Project cbp_proj;
  };

  BOOST_DESCRIBE_STRUCT (Content, (), (node_name));
  BOOST_DESCRIBE_STRUCT (Return, (), (cbp_proj));
};

template<> struct PackContent<DisPatcher_Msg::Connect>
{
  struct ContentImpl{
    string node_name;
    string parent_node_name;
    Connect_Type connect_type;
  };
  using Content = std::vector<ContentImpl>; /* pass connect in one time */
  using Return = Status_t;
  BOOST_DESCRIBE_STRUCT (ContentImpl, (), (node_name,parent_node_name,connect_type));
};

template<> struct PackContent<DisPatcher_Msg::Convert>
{
  struct ContentImpl{
    std::vector<PackContent<DisPatcher_Msg::AnaLyze>::Return> Analyzed_cbp_projects;
    PackContent<DisPatcher_Msg::Connect>::Content connects;
    string root_name;
  };
  struct ReturnImpl{
    std::map<Node,Cmake::project> cmakeProjects;
  };
  using Content=ContentImpl;
  using Return=ReturnImpl;
  BOOST_DESCRIBE_STRUCT (ContentImpl, (), (connects, root_name));
  BOOST_DESCRIBE_STRUCT (ReturnImpl, (), (cmakeProjects));
};

template <> struct PackContent<DisPatcher_Msg::Extract>
{
  using Content = struct
  {
    PackContent<DisPatcher_Msg::Connect>::Content connects;
    PackContent<DisPatcher_Msg::Convert>::Return cmakeProjects;
    string root_name;
  };
  using Return = struct
  {
    PackContent<DisPatcher_Msg::Connect>::Content connects;
    PackContent<DisPatcher_Msg::Convert>::Return cmakeProjects;
  };
  BOOST_DESCRIBE_STRUCT (Content, (), (connects, cmakeProjects));
  BOOST_DESCRIBE_STRUCT (Return, (), (connects, cmakeProjects));
};

template<> struct PackContent<DisPatcher_Msg::Dump>{
  using Content=  struct{
    Cmake::project cmakeProject;
  };
  using Return =struct{
    std::vector<Node> nodes;
  };
  BOOST_DESCRIBE_STRUCT (Content, (), (cmakeProject));
  BOOST_DESCRIBE_STRUCT (Return, (), (nodes));
  
};

} // namespace PackContent
}// namespace CTC
