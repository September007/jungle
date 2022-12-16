#pragma once
#include <agent.hpp>
#include <boost/describe.hpp>
#include <jungle/cpp_interface.hpp>
#include <map>
#include <memory>
#include <msg.hpp>
#include <mutex>
#include <string>

/* cbp file to cmake */
namespace CTC
{
BOOST_DEFINE_ENUM (CTC_Running_State, SUCCESS, AGENT_INIT_FAILED);
namespace Detail
{

struct CTC_Task
{
  CTC_Running_State Init ();
  CTC_Running_State Run ();
  /* set when Parse do */
  std::vector<std::string> cbpfiles;
  std::shared_ptr<CTC::Agent> agent;
  int CTC_ID = 1;

  /* Cache data */
  struct Cache
  {
    std::map<string, std::shared_ptr<Node> > nodes; // Add_Node returns
    std::map<string, Cbp::Project> cbp_Projects;    // Analyze returns
    std::multimap<string, std::pair<string, Connect_Type> >
        connects;                                              // Connects returns: node_name=>{pnode,connect type}
    std::map<string, Cmake::Project> cmake_Projects;           // Convert returns
    std::map<string, Cmake::Project> extracted_cmake_Projects; // Extract returns
    std::map<string, std::vector<Node> > dumps;                // Dump returns: node_name==>[node component]...

  } cache;
  std::mutex cache_m;

  /* manage CTC_Task */
  static std::shared_ptr<CTC_Task>
  CreateTask ()
  {
    static std::mutex _m;
    std::lock_guard __ (_m);
    auto ret = std::shared_ptr<CTC_Task> (new CTC_Task);
    ret->CTC_ID = _id_to_task.size () + 1;
    return _id_to_task[ret->CTC_ID] = ret;
  }
  static std::shared_ptr<CTC_Task>
  AccessTask (int ctc_id)
  {
    return _id_to_task[ctc_id];
  }

private:
  static std::map<int, std::shared_ptr<CTC_Task> > _id_to_task;
  CTC_Task (){};
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
