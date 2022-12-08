#pragma once
#include <boost/describe.hpp>
#include <memory>
#include <string>
#include <msg.hpp>
namespace CTC{
namespace Detail{

struct Agent{
    virtual ~Agent(){}
    /* return 0 if success */
    virtual int Init()=0;
    /* return InvalidInstance if not successfully handled */
    virtual PackType Dispatcher(DisPatcher_Msg,PackType)=0;
    std::string name;
};

BOOST_DESCRIBE_STRUCT(Agent,(),(name,Init,Dispatcher));
}
using Agent=Detail::Agent;
}
