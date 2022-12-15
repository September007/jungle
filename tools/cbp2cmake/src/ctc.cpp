#include <ctc.hpp>
#include <msg.hpp>
#include <msg_handler.hpp>
using namespace CTC;

std::shared_ptr<CTC_Task>
Detail::CTC_Task_Raw::Parse ()
{
  std::shared_ptr<CTC_Task> ret;

  return ret;
}

CTC_Running_State
CTC_Task::Run ()
{
  if (auto init = agent->Init (); init != 0)
    {
      LOG (format ("agent [{}] init failed,returning", agent->name));
      return CTC_Running_State::AGENT_INIT_FAILED;
    }
  auto DeliverMsg = [&] (DisPatcher_Msg msg, PackType & pack) -> bool {
    auto ret = agent->Dispatcher (msg, pack);
    if (ret == Status_t::UnHandled)
      {
        LOG (format ("agent not handled DisPatcher_Msg [{}]", int (msg))); // todo: try describe
        ret = defaultDispathcerMsgHandler(msg,pack);
        if (ret != Status_t::UnHandled)
          LOG (format ("defaultHandler cannot handle DisPatcher_Msg [{}]", int (msg)));
        else
          LOG (format ("defaultHandler also cannot handle DisPatcher_Msg [{}]", int (msg)));
      }
    return ret == Status_t::SUCCESS;
  };
  return CTC_Running_State::SUCCESS;
}