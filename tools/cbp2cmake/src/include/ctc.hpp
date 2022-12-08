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
  CTC_Running_State Run ();
  std::vector<std::string> cbpfiles;
  std::shared_ptr<CTC::Agent> agent;
  
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
};

BOOST_DESCRIBE_STRUCT (CTC_Task_Raw, (), (conf, type));
BOOST_DESCRIBE_STRUCT (CTC_Task, (), (cbpfiles, agent));
} // namespace Detail
using CTC_Task_Raw = Detail::CTC_Task_Raw;
using CTC_Task = Detail::CTC_Task;
} // namespace CTC
