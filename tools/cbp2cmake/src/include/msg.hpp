#pragma once
#include <data_convert.hpp>
#include <functional>
#include <map>
#include <struct.hpp>
namespace CTC
{

using PackType = cpp_interface::Data::DataItem;
BOOST_DEFINE_ENUM_CLASS (DisPatcher_Msg, Add_Node, AnaLyze, Connect, Construct, Convert, Extract, Upload, Dump);

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
  struct ContentImpl
  {
    string name;
    Node_Type type;
    string content;
  };
  using Content = ContentImpl;
  using Return = Status_t;
  BOOST_DESCRIBE_STRUCT (ContentImpl, (), (name, type, content));
};
template <> struct PackContent<DisPatcher_Msg::AnaLyze>
{
  struct ContentImpl
  {
    string node_name;
  };
  using Content = ContentImpl;
  using Return = CTC::Cbp::Project;

  //BOOST_DESCRIBE_STRUCT (ContentImpl, (), (name, type, content));
};

template<> struct PackContent<DisPatcher_Msg::Connect>
{
  struct ContentImpl{
    string node_name;
    string parent_node_name;
    Connect_Type connect_type;
  };
  using Content = ContentImpl;
  using Return = Status_t;
};

} // namespace PackContent
}// namespace CTC
