/*
transform between
    boost_defined type,and
    cpp_interface::table
note： boost_defined type only support simple type like
    int,float,string,vector,map,
    and composite type with member just like before
    not support reference type
*/
#include <boost/describe.hpp>
#include <jungle/cpp_interface.hpp>
#include <set>
namespace CTC
{
namespace Detail
{
using WT = cpp_interface::Data::DataItem; // weak type
using Table = cpp_interface::Data::Table; // table
/*** type detect ***/
template <template <typename... T> typename templateT, typename T> struct ComesFrom : std::false_type
{
};
template <template <typename... T> typename templateT, typename... ParamT>
struct ComesFrom<templateT, templateT<ParamT...> > : std::true_type
{
};

// template <typename T>
// constexpr bool is_described = std::is_same_v<decltype (boost_public_member_descriptor_fn ((T **)0)), int>;

template <typename T> concept is_described = requires (std::decay_t<T> * t) { boost_public_member_descriptor_fn (&t); };

template <typename T>
inline WT
DataTo (T &origin_t)
{
  auto &t = *(std::decay_t<T> *)&origin_t;
  if constexpr (is_described<T>) // described class
    {
      WT ret (std::make_shared<Table> ());
      Table &table = *std::get<std::shared_ptr<Table> > (ret).get ();
      using D1 = boost::describe::describe_members<T, boost::describe::mod_public | boost::describe::mod_protected>;
      boost::mp11::mp_for_each<D1> ([&] (auto D) {
        using DT = std::decay_t<decltype ((std::decay_t<T> *)0->*D.pointer)>;
        if constexpr (std::is_integral_v<DT> || std::is_enum_v<DT>)
          {
            table.emplace (std::make_pair<WT, WT> (D.name, int (t.*D.pointer)));
          }
        else if constexpr (std::is_floating_point_v<DT>)
          {
            table.emplace (std::make_pair<WT, WT> (D.name, float (t.*D.pointer)));
          }
        else if constexpr (std::is_same<std::string, DT>::value)
          {
            table.emplace (std::make_pair<WT, WT> (D.name, std::string (t.*D.pointer)));
          }
        else if constexpr (ComesFrom<std::map, DT>::value)
            table.emplace (WT (D.name),DataTo(t.*D.pointer));
        else if constexpr (ComesFrom<std::set, DT>::value)
            table.emplace (WT (D.name),DataTo(t.*D.pointer));
        else if constexpr (ComesFrom<std::vector, DT>::value)
            table.insert (std::make_pair<WT, WT> (D.name, DataTo(t.*D.pointer)));
        else
          {
            static_assert (!std::is_same_v<DT, DT>, "not supported base type");
          }
      });
      // _External<T> e{ origin_t, table };
      // boost::mp11::mp_for_each<D1> (e);
      return ret;
    }
  else // base type
    {
      using NakedT = std::decay_t<T>;
      if constexpr (std::is_integral_v<NakedT> || std::is_enum_v<NakedT> || std::is_same_v<float, NakedT>
                    || std::is_same_v<std::string, NakedT>) // base type
        return WT (t); 
      else if constexpr (ComesFrom<std::map, NakedT>::value)
        {
          auto &map = t;
          auto childTable = Table ();
          for (auto &pair : map)
            childTable.emplace(DataTo (pair.first), DataTo (pair.second));
            //childTable.emplace_hint (childTable.end (), DataTo (pair.first), DataTo (pair.second));
          return WT (std::make_shared<Table> (std::move (childTable)));
        }
      else if constexpr (ComesFrom<std::set, NakedT>::value)
        {
          auto childTable = Table ();
          WT invalid = WT::InvalidInstance ();
          for (auto &node : t)
            {
              childTable.emplace (DataTo (node), invalid);
              // auto e=DataTo(node);
              // childTable.emplace(e,invalid);
            }
          // childTable.emplace_hint (childTable.end (), DataTo (node), invalid);
          return WT (std::make_shared<Table> (std::move (childTable)));
        }
      else if constexpr (ComesFrom<std::vector, NakedT>::value)
        {
          auto &vec = t;
          auto childTable = Table ();
          for (int i = 0; i < int (vec.size ()); ++i)
            childTable.emplace (std::make_pair<WT, WT> (i, DataTo (vec[i])));
          childTable.emplace (std::make_pair<WT, WT> ("size", cpp_interface::Data::Int (vec.size ())));
          return std::make_shared<Table> (std::move (childTable));
        }
      else
        static_assert (!std::is_same_v<T, T>, "not supported base type");
    }
  return WT (); // this just to erase warning
}

template <typename T>
inline std::decay_t<T>
DataFrom (const WT &wt)
{
  using retT = std::decay_t<T>;
  retT ret;
  if constexpr (is_described<retT>) // described class
    {
      auto shared_table = wt.safe_access<std::shared_ptr<cpp_interface::Data::Table> > ();
      if (shared_table == nullptr)
        {
          LOG (fmt::format ("DataFrom<{}> access table failed", typeid (retT).name ()));
          return ret;
        }
      auto &table = *shared_table.get ();
      using D1 = boost::describe::describe_members<T, boost::describe::mod_public | boost::describe::mod_protected>;
      boost::mp11::mp_for_each<D1> ([&] (auto D) -> void {
        using DT = std::decay_t<decltype ((std::decay_t<T> *)0->*D.pointer)>;
        auto &val = table[WT (D.name)];
        if constexpr (std::is_integral_v<DT> || std::is_enum_v<DT>)
          {
            ret.*D.pointer = DT (val.safe_access<cpp_interface::Data::Int> ());
          }
        else if constexpr (std::is_floating_point_v<DT>)
          {
            ret.*D.pointer = val.safe_access<cpp_interface::Data::Float> ();
          }
        else if constexpr (std::is_same<std::string, DT>::value)
          {
            ret.*D.pointer = val.safe_access<cpp_interface::Data::String> ();
          }
        else if constexpr (ComesFrom<std::map, DT>::value||ComesFrom<std::set, DT>::value||ComesFrom<std::vector, DT>::value)
          {
            ret.*D.pointer=DataFrom<DT>(val);//DataFrom<DT>(val.safe_access<std::shared_ptr<Table> >().get()->operator[](WT(D.name)));
          }
        else
          {
            static_assert (!std::is_same_v<DT, DT>, "not supported base type");
          }
      });
    }
  else
    {
      if constexpr (std::is_integral_v<retT> || std::is_enum_v<retT>)
        ret = retT (wt.safe_access<cpp_interface::Data::Int> ());
      else if constexpr (std::is_floating_point_v<retT>)
        ret = retT (wt.safe_access<cpp_interface::Data::Float> ());
      else if constexpr (std::is_same_v<std::string, retT>)
        ret = wt.safe_access<std::string> ();
      else if constexpr (ComesFrom<std::map,retT>::value)
        {
          using KT=retT::key_type;
          using VT=retT::mapped_type;
          auto &map=ret;

          auto table = *(wt.safe_access<std::shared_ptr<Table> > ().get ());
          for(auto &pair:table)
            map.insert(std::make_pair(DataFrom<KT>(pair.first),DataFrom<VT>(pair.second)));
        }
      else if constexpr (ComesFrom<std::set,retT>::value)
        {
          using KT=retT::key_type;
          auto &set=ret;

          auto table = *(wt.safe_access<std::shared_ptr<Table> > ().get ());
          for(auto &k:table)
            {
              auto s=table.size();
              set.insert(std::move(DataFrom<KT>(k.first)));
            }
        }
      else if constexpr (ComesFrom<std::vector, retT>::value)
        {
          using VecValueType = retT::value_type;
          auto &vec=ret;

          auto table = *(wt.safe_access<std::shared_ptr<Table> > ().get ());
          int size = table[WT ("size")].safe_access<cpp_interface::Data::Int> ();
          vec.resize (size);

          WT tempIndex = WT (0);
          for (int i = 0; i < size; ++i)
            {
              tempIndex = i;
              if (auto p = table.find (tempIndex); p != table.end ())
                vec[i] = DataFrom<VecValueType> (p->second);
              else
                LOG(fmt::format("data at index {} not found",i));
            }
        }
      else
        static_assert (!std::is_same_v<retT, retT>, "not supported base type");
    }
  return ret;
}
} // namespace Detail
using Detail::DataFrom;
using Detail::DataTo;
} // namespace CTC
