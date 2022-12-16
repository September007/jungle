#include "handler_base.hpp"
#include "util.hpp"
#include <io.hpp>
#include <ctc.hpp>
namespace CTC
{
template <>
inline Status_t
DHandler<DisPatcher_Msg::Add_Node> (WT &in_, WT &out_)
{
  auto in = CTC::DataFrom<PackContent<DisPatcher_Msg::Add_Node>::Content> (in_);
  auto out = CTC::DataFrom<PackContent<DisPatcher_Msg::Add_Node>::Return> (out_);

  auto task = CTC_Task::AccessTask (in.CTC_ID);

  // assure in.node.name is location
  auto location = fs::canonical (in.node.name);
  in.node.location = location.generic_string ();
  in.node.content = IO::readFile (in.node.location);
  in.node.type = decltype (in.node.type)::Target;

  // find a proper name
  auto name = fs::path(in.node.name).filename().generic_string();
  auto &nodes = task->cache.nodes;
  if (nodes.find (name) != nodes.end ())
    {
      int cnt = 0;
      while (++cnt)
        {
          auto name_cnt = fmt::format ("{}.{}", name, cnt);
          if (nodes.find (name_cnt) == nodes.end ())
            {
              name = name_cnt;
              break;
            }
        }
    }
  in.node.name = name;

  out.status = Status_t::SUCCESS;

  in_ = CTC::DataTo (in);
  out_ = CTC::DataTo (out);
  return Status_t::SUCCESS;
}
}

#undef curmsg
