#include <ctc.hpp>
#include <msg.hpp>
using namespace CTC;

std::shared_ptr<CTC_Task> Detail::CTC_Task_Raw::Parse() {
    std::shared_ptr<CTC_Task> ret;

    return ret;
}

CTC_Running_State CTC_Task::Run(){
    if(auto init=agent->Init();init!=0){
        LOG(fmt::format("agent [{}] init failed,returning",agent->name));
        return CTC_Running_State::AGENT_INIT_FAILED;
    }

    return CTC_Running_State::SUCCESS;
}