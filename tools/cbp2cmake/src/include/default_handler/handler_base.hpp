#pragma once
#include <data_convert.hpp>
#include <jungle/core/log.h>
#include <msg.hpp>
namespace
{
using WT = cpp_interface::Data::DataItem; // weak type
using Table = cpp_interface::Data::Table; // table
}

namespace CTC
{
/* return Status_t::SUCCESS */
template <DisPatcher_Msg msg>
inline Status_t
DHandler (WT &in_, WT &out_)
{
  static_assert (msg == msg, "why did you get here, you shouldn't!!!!");

  auto in = CTC::DataFrom<typename PackContent<msg>::Content> (in_);
  auto out = CTC::DataFrom<typename PackContent<msg>::Return> (out_);

  in_ = CTC::DataTo (in);
  out_ = CTC::DataTo (out);
  return Status_t::SUCCESS;
};

#define UnPack(in, out)                                                                                                \
  auto in = CTC::DataFrom<PackContent<msg>::Content> (in_);                                                            \
  auto out = CTC::DataFrom<PackContent<msg>::Return> (out_);
#define Pack(in, out)                                                                                                  \
  in_ = CTC::DataTo (in);                                                                                              \
  out_ = CTC::DataTo (out);
};