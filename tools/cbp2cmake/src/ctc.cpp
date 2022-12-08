#include <ctc.hpp>
#include <msg.hpp>
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
  auto DeliverMsg = [&] (DisPatcher_Msg msg, PackType pack) -> PackType {
    auto ret = agent->Dispatcher (msg, pack);
    if (ret == PackType::InvalidInstance ())
      {
        LOG (format ("agent not handled DisPatcher_Msg [{}]", int (msg))); // todo: try describe
        auto p = defaultDispathcerMsgHandler.find (msg);
        if (p != defaultDispathcerMsgHandler.end ())
          ret = p->second (pack);
        if (ret != PackType::InvalidInstance ())
          LOG (format ("defaultHandler cannot handle DisPatcher_Msg [{}]", int (msg)));
        else
          LOG (format ("defaultHandler also cannot handle DisPatcher_Msg [{}]", int (msg)));
      }
    return ret;
  };
  return CTC_Running_State::SUCCESS;
}