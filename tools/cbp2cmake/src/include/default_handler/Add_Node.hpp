#include "handler_base.hpp"

namespace CTC
{
template <>
inline Status_t
DHandler<DisPatcher_Msg::Add_Node> (WT &in_, WT &out_)
{
  static_assert (Detail::is_described<PackContent<DisPatcher_Msg::Add_Node>::Content>, "");
  static_assert (Detail::is_described<CTC::Node>, "");
  using T=decltype (DataTo (std::declval<PackContent<DisPatcher_Msg::Add_Node>::Content> ()));
  // static_assert(std::is_function_v<decltype(DataTo<int>)>,"");
  // static_assert(!std::is_function_v<decltype(DataTo<WT>)>,"");
  // static_assert(!std::is_function<DataTo<std::wstring>>,"");
  // static_assert (Detail::is_DataTo<PackContent<DisPatcher_Msg::Add_Node>::Content >::value, "");
  // static_assert (Detail::is_DataTo_able<PackContent<DisPatcher_Msg::Add_Node>::Content >, "");
  // static_assert (Detail::is_DataFrom_able<PackContent<DisPatcher_Msg::Add_Node>::Content >, "");
  static_assert (Detail::is_Data_Supported<PackContent<DisPatcher_Msg::Add_Node>::Content >, "");

  Node node;
  auto nodeto = CTC::DataTo (node);
  auto nodefrom = CTC::DataFrom<Node> (nodeto);
  auto in = CTC::DataFrom<PackContent<DisPatcher_Msg::Add_Node>::Content> (in_);
  auto out = CTC::DataFrom<PackContent<DisPatcher_Msg::Add_Node>::Return> (out_);

  out.status = Status_t::SUCCESS;

  in_ = CTC::DataTo (in);
  out_ = CTC::DataTo (out);
  return Status_t::SUCCESS;
}
}

#undef curmsg