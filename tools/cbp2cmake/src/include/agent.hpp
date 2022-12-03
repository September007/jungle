#pragma once
#include <boost/describe.hpp>
#include <memory>
#include <string>
namespace CTC{
namespace Detail{

struct Agent{
    virtual ~Agent(){}
    /* return 0 if success */
    virtual int Init()=0;
    /* return 0 if handled */
    virtual int Dispatcher()=0;
    std::string name;
};

BOOST_DESCRIBE_STRUCT(Agent,(),(name,Init,Dispatcher));
}
using Agent=Detail::Agent;
}
