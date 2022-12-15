#include "handler_base.hpp"

#define curmsg DisPatcher_Msg::Dump
namespace CTC
{
template <>
inline Status_t
DHandler<curmsg> (WT &in_, WT &out_)
{
  static_assert (Detail::is_described<PackContent<curmsg>::Content>, "");
  static_assert (Detail::is_described<CTC::Node>, "");
  Node node;
  auto nodeto = CTC::DataTo (node);
  auto nodefrom = CTC::DataFrom<Node> (nodeto);
  auto in = CTC::DataFrom<PackContent<curmsg>::Content> (in_);
  auto out = CTC::DataFrom<PackContent<curmsg>::Return> (out_);

  

  in_ = CTC::DataTo (in);
  out_ = CTC::DataTo (out);
  return Status_t::SUCCESS;
}
}

#undef curmsg