#pragma once
#include <data_convert.hpp>
#include <functional>
#include <map>
#include <struct.hpp>
#include <vector>
namespace CTC
{

using PackType = struct _PackType
{
  cpp_interface::Data::DataItem in, out;
}; // cpp_interface::Data::DataItem;

BOOST_DESCRIBE_STRUCT (_PackType, (), (in, out));
BOOST_DEFINE_ENUM_CLASS (DisPatcher_Msg, Add_Node, Analyze, Connect, /* Construct, */ Convert, Extract,
                         /* Upload ,*/ Dump);

Status_t defaultDispathcerMsgHandler(DisPatcher_Msg msg,PackType &);

namespace PackContent_
{

using CTC::Status_t;
using std::false_type;
using std::string;
struct ContentBase
{
  int CTC_ID;
};
BOOST_DESCRIBE_STRUCT (ContentBase, (), (CTC_ID));

template <DisPatcher_Msg msg> struct PackContent
{
};

template <> struct PackContent<DisPatcher_Msg::Add_Node>
{
  using Content = struct ContentImpl : ContentBase
  {
    Node node;
  };
  using Return = struct
  {
    Status_t status;
  };
};
BOOST_DESCRIBE_STRUCT (PackContent<DisPatcher_Msg::Add_Node>::Content, (ContentBase), (node));
BOOST_DESCRIBE_STRUCT (PackContent<DisPatcher_Msg::Add_Node>::Return, (), (status));

template <> struct PackContent<DisPatcher_Msg::Analyze>
{

  using Content = struct _Anom: ContentBase
  {

    string node_name;
  };
  using Return = struct
  {
    CTC::Cbp::Project cbp_proj;
  };

};
  BOOST_DESCRIBE_STRUCT (PackContent<DisPatcher_Msg::Analyze>::Content, (ContentBase), (node_name));
  BOOST_DESCRIBE_STRUCT (PackContent<DisPatcher_Msg::Analyze>::Return, (), (cbp_proj));

template <> struct PackContent<DisPatcher_Msg::Connect>
{
  struct ContentImpl : ContentBase
  {
    string node_name;
    string parent_node_name;
    Connect_Type connect_type;
  };
  using Content = std::vector<ContentImpl>; /* pass connect in one time */
  using Return = Status_t;
};
  BOOST_DESCRIBE_STRUCT (PackContent<DisPatcher_Msg::Connect>::ContentImpl, (ContentBase), (node_name, parent_node_name, connect_type));

template <> struct PackContent<DisPatcher_Msg::Convert>
{

  struct ContentImpl0: ContentBase
  {
    std::vector<PackContent<DisPatcher_Msg::Analyze>::Return> Analyzed_cbp_projects;
    PackContent<DisPatcher_Msg::Connect>::Content connects;
    string root_name;
  };
  struct ReturnImpl0
  {
    std::map<Node, Cmake::Project> cmakeProjects;
  };
  struct ContentImpl1 : ContentBase
  {
    std::string node_name;
  };
  struct ReturnImpl1
  {
    Cmake::Project cmakeProject;
  };
  using Return =ReturnImpl1;
  using Content=ContentImpl1;
};
  BOOST_DESCRIBE_STRUCT (PackContent<DisPatcher_Msg::Convert>::ContentImpl0, (ContentBase), (connects, root_name));
  BOOST_DESCRIBE_STRUCT (PackContent<DisPatcher_Msg::Convert>::ContentImpl1, (ContentBase), (node_name));
  BOOST_DESCRIBE_STRUCT (PackContent<DisPatcher_Msg::Convert>::ReturnImpl0, (), (cmakeProjects));
  BOOST_DESCRIBE_STRUCT (PackContent<DisPatcher_Msg::Convert>::ReturnImpl1, (), (cmakeProject));

template <> struct PackContent<DisPatcher_Msg::Extract>
{

  using Content = struct _Anom: ContentBase
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
};
  BOOST_DESCRIBE_STRUCT (PackContent<DisPatcher_Msg::Extract>::Content, (ContentBase), (connects, cmakeProjects));
  BOOST_DESCRIBE_STRUCT (PackContent<DisPatcher_Msg::Extract>::Return, (), (connects, cmakeProjects));

template <> struct PackContent<DisPatcher_Msg::Dump>
{
  using Content = struct _Anom: ContentBase
  {
    std::string node_name;
    Cmake::Project cmakeProject;
  };
  using Return = struct
  {
    std::vector<Node> nodes;
  };
};
  BOOST_DESCRIBE_STRUCT (PackContent<DisPatcher_Msg::Dump>::Content, (ContentBase), (node_name,cmakeProject));
  BOOST_DESCRIBE_STRUCT (PackContent<DisPatcher_Msg::Dump>::Return, (), (nodes));

} // namespace PackContent
using PackContent_::PackContent;
} // namespace CTC
