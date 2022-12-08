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

template <DisPatcher_Msg msg> struct PackContent : false_type
{
};
template <> struct PackContent<DisPatcher_Msg::Add_Node>
{
  struct ContentImpl
  {
    std::string name;
    Node_Type type;
    std::string content;
  };
  using Content = ContentImpl;
  BOOST_DESCRIBE_STRUCT (ContentImpl, (), (name, type, content));
};
template <> struct PackContent<DisPatcher_Msg::AnaLyze>
{
  struct ContentImpl
  {
    std::string node_name;
  };
  using Content = ContentImpl;
  //BOOST_DESCRIBE_STRUCT (ContentImpl, (), (name, type, content));
};
}
}
