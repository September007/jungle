#pragma once
#include <agent.hpp>
#include <boost/describe.hpp>
#include <jungle/cpp_interface.hpp>
#include <memory>
#include <string>

/* cbp file to cmake */
namespace CTC
{
BOOST_DEFINE_ENUM (CTC_Running_State, SUCCESS, AGENT_INIT_FAILED );
namespace Detail
{

struct CTC_Task
{
  std::vector<std::string> cbpfiles;
  std::shared_ptr<CTC::Agent> agent;
  std::shared_ptr<cpp_interface::Data::Table> data;
  CTC_Running_State Run ();
};

struct CTC_Task_Raw
{
  enum Type
  {
    json,
    describe
  };
  BOOST_DESCRIBE_NESTED_ENUM (Type, json, describe);
  std::shared_ptr<CTC_Task> Parse ();
  std::string conf;
  Type type;
  std::vector<int> vi;
};

BOOST_DESCRIBE_STRUCT (CTC_Task_Raw, (), (conf, type,vi));
BOOST_DESCRIBE_STRUCT (CTC_Task, (), (cbpfiles, agent,data));
} // namespace Detail
using CTC_Task_Raw = Detail::CTC_Task_Raw;
using CTC_Task = Detail::CTC_Task;
} // namespace CTC
