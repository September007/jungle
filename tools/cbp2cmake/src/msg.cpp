#include <msg.hpp>
#include <msg_handler.hpp>
using namespace CTC;
Status_t defaultDispathcerMsgHandler (DisPatcher_Msg msg,PackType &pack) {
#define CALL(msg) \
        case DisPatcher_Msg::msg:\
        ret = DHandler<DisPatcher_Msg::msg>(pack.in,pack.out);\
        break;
    Status_t ret=Status_t::UnHandled;
    switch(msg){
        CALL(Add_Node);
        CALL(Analyze);
        CALL(Connect);
        CALL(Convert);
        CALL(Extract);
        CALL(Dump);
        default:
        LOG(fmt::format("unexpected msg [{}]",int(msg)));
    }
    return Status_t::SUCCESS;
};
