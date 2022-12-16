#pragma once
#include <agent.hpp>

namespace CTC{

struct Null_Agent:public Agent{
    Null_Agent(){
        name="Nul_agent";
    }
    ~Null_Agent(){}
    int Init(){return 0;}
    virtual Status_t Dispatcher(DisPatcher_Msg,PackType){return Status_t::UnHandled;}
};
}