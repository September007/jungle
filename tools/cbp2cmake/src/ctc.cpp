#include <agent/cpp/null_agent.hpp>
#include <ctc.hpp>
#include <io.hpp>
#include <jungle/core/log.h>
#include <msg.hpp>
#include <msg_handler.hpp>
namespace CTC
{

std::map<int, std::shared_ptr<CTC_Task> > CTC_Task::_id_to_task;

std::shared_ptr<CTC_Task>
Detail::CTC_Task_Raw::Parse ()
{
  std::shared_ptr<CTC_Task> ret = CTC_Task::CreateTask ();
  ret->agent = std::make_shared<Null_Agent> ();

  return ret;
}

CTC_Running_State
CTC_Task::Init ()
{
  return CTC_Running_State::SUCCESS;
}
CTC_Running_State
CTC_Task::Run ()
{
  if (auto init = agent->Init (); init != 0)
    {
      LOG (format ("agent [{}] init failed,returning", agent->name));
      return CTC_Running_State::AGENT_INIT_FAILED;
    }
  auto DeliverMsg = [&] (DisPatcher_Msg msg, PackType &pack) -> bool {
    auto ret = agent->Dispatcher (msg, pack);
    if (ret == Status_t::UnHandled)
      {
        LOG (format ("agent not handled DisPatcher_Msg [{}]", int (msg))); // todo: try describe
        ret = defaultDispathcerMsgHandler (msg, pack);
        if (ret != Status_t::UnHandled)
          LOG (format ("defaultHandler cannot handle DisPatcher_Msg [{}]", int (msg)));
        else
          LOG (format ("defaultHandler also cannot handle DisPatcher_Msg [{}]", int (msg)));
      }
    return ret == Status_t::SUCCESS;
  };

  // Add_Node
  for (auto cbp : cbpfiles)
    {
      PackType pack{ DataTo (PackContent<DisPatcher_Msg::Add_Node>::Content{
                         CTC_ID, Node{ .name = cbp, .type = Node_Type::Target, .content = "", .location = "" } }),
                     DataTo (PackContent<DisPatcher_Msg::Add_Node>::Return{}) };
      DeliverMsg (DisPatcher_Msg::Add_Node, pack);
      auto in = DataFrom<PackContent<DisPatcher_Msg::Add_Node>::Content> (pack.in);
      auto out = DataFrom<PackContent<DisPatcher_Msg::Add_Node>::Return> (pack.out);
      if (out.status == Status_t::SUCCESS)
        this->cache.nodes[in.node.name] = std::make_shared<CTC::Node> (in.node);
      else
        LOG (fmt::format ("Add_Node failed,status:[{}],node:[{}]", int (out.status), in.node.name));
    }

  // Analyze
  for (auto &[_, node] : this->cache.nodes)
    {
      PackType pack{ DataTo (PackContent<DisPatcher_Msg::Analyze>::Content{ CTC_ID, node->name }),
                     DataTo (PackContent<DisPatcher_Msg::Analyze>::Return{}) };
      DeliverMsg (DisPatcher_Msg::Analyze, pack);
      auto in = DataFrom<PackContent<DisPatcher_Msg::Analyze>::Content> (pack.in);
      auto out = DataFrom<PackContent<DisPatcher_Msg::Analyze>::Return> (pack.out);
      this->cache.cbp_Projects[in.node_name] = out.cbp_proj;
    }

  // todo : Connect

  // Convert
  for (auto &[_, node] : this->cache.nodes)
    {
      PackType pack{ DataTo (PackContent<DisPatcher_Msg::Convert>::Content{ CTC_ID, node->name }),
                     DataTo (PackContent<DisPatcher_Msg::Convert>::Return{}) };
      DeliverMsg (DisPatcher_Msg::Convert, pack);
      auto in = DataFrom<PackContent<DisPatcher_Msg::Convert>::Content> (pack.in);
      auto out = DataFrom<PackContent<DisPatcher_Msg::Convert>::Return> (pack.out);
      this->cache.cmake_Projects[in.node_name] = out.cmakeProject;
    }

  // todo : Extract

  // Dump
  for (auto &[_, node] : this->cache.nodes)
    {
      PackType pack{ DataTo (PackContent<DisPatcher_Msg::Dump>::Content{ CTC_ID, node->name,
                                                                         cache.cmake_Projects[node->name] }),
                     DataTo (PackContent<DisPatcher_Msg::Dump>::Return{}) };
      DeliverMsg (DisPatcher_Msg::Dump, pack);
      auto in = DataFrom<PackContent<DisPatcher_Msg::Dump>::Content> (pack.in);
      auto out = DataFrom<PackContent<DisPatcher_Msg::Dump>::Return> (pack.out);
      this->cache.dumps[in.node_name] = out.nodes;
    }

  for (auto node : this->cache.dumps)
    {
      LOG (fmt::format ("Dump Install: {} at {}", node.first, this->cache.nodes[node.first]->location));
      for (auto comp : node.second)
        {
          LOG (fmt::format ("{:<4} {:<2} component {:<10} at {}", "\t", int (comp.type), comp.name, comp.location));
          IO::writeFile (comp.location, comp.content.c_str (), comp.content.size ());
        }
    }

  return CTC_Running_State::SUCCESS;
}
} // namespace CTC