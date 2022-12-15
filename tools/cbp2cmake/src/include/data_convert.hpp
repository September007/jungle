/*
transform between
    boost_defined type,and
    cpp_interface::table
note： boost_defined type only support simple type like
    int,float,string,vector,map,
    and composite type with member just like before
    not support reference type
*/
#pragma once
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

/* todo: consider described enum as special type, presenting it in string not int  */
#if __cplusplus < 202002L
template <typename T>
constexpr bool is_described_impl = /* boost::describe::has_describe_enumerators<T>::value|| */
    boost::describe::has_describe_bases<T>::value || boost::describe::has_describe_members<T>::value;
#else
// clang-format off
template <typename T> concept is_described_impl = 
  requires (std::decay_t<T> *t) { boost_public_member_descriptor_fn (&t);} ||
  requires (std::decay_t<T> *t) { boost_base_descriptor_fn (&t); };
// clang-format on
#endif
template <typename T> constexpr bool is_described = is_described_impl<T>;

template <typename OT>
inline WT
DataTo (OT const &origin_t)
{
  using T = std::decay_t<OT>;
  auto &t = *(std::decay_t<T> *)&origin_t;
  if constexpr (is_described<T>) // described class
    {
      using Members = boost::describe::describe_members<T, boost::describe::mod_any_access>;
      using Bases = boost::describe::describe_bases<T, boost::describe::mod_any_access>;
      WT ret (std::make_shared<Table> ());
      Table &table = *std::get<std::shared_ptr<Table> > (ret).get ();

      boost::mp11::mp_for_each<Bases> ([&] (auto D) {
        constexpr int base_index = boost::mp11::mp_find<Bases, decltype (D)>::type::value;
        table.emplace (base_index, DataTo (*(std::add_pointer_t<std::decay_t<typename decltype (D)::type> >)(&t)));
      });
      boost::mp11::mp_for_each<Members> ([&] (auto D) {
        using DT = std::decay_t<decltype ((std::decay_t<T> *)0->*D.pointer)>;
        table.emplace (D.name, DataTo (t.*D.pointer));
      });
      return ret;
    }
  else // base type
    {
      using NakedT = std::decay_t<T>;
      if constexpr (std::is_integral_v<NakedT> || std::is_enum_v<NakedT> || std::is_floating_point_v<NakedT>
                    || std::is_same_v<float, NakedT> || std::is_same_v<std::string, NakedT>) // base type
        return WT (t);
      else if constexpr (ComesFrom<std::map, NakedT>::value)
        {
          auto &map = t;
          auto childTable = Table ();
          for (auto &pair : map)
            childTable.emplace (DataTo (pair.first), DataTo (pair.second));
          // childTable.emplace_hint (childTable.end (), DataTo (pair.first), DataTo (pair.second));
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
  LOG ("how did i got here");
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
      using Bases = boost::describe::describe_bases<T, boost::describe::mod_any_access>;
      using Members = boost::describe::describe_members<T, boost::describe::mod_any_access>;
      boost::mp11::mp_for_each<Bases> ([&] (auto D) {
        constexpr int base_index = boost::mp11::mp_find<Bases, decltype (D)>::type::value;
        *(std::add_pointer_t<std::decay_t<typename decltype (D)::type> >)(&ret)
            = DataFrom<typename decltype (D)::type> (table[base_index]);
      });
      boost::mp11::mp_for_each<Members> ([&] (auto D) -> void {
        using DT = std::decay_t<decltype ((std::decay_t<T> *)0->*D.pointer)>;
        auto &val = table[WT (D.name)];
        ret.*D.pointer = DataFrom<DT> (val);
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
      else if constexpr (ComesFrom<std::map, retT>::value)
        {
          using KT = typename retT::key_type;
          using VT = typename retT::mapped_type;
          auto &map = ret;

          auto table = *(wt.safe_access<std::shared_ptr<Table> > ().get ());
          for (auto &pair : table)
            map.insert (std::make_pair (DataFrom<KT> (pair.first), DataFrom<VT> (pair.second)));
        }
      else if constexpr (ComesFrom<std::set, retT>::value)
        {
          using KT = typename retT::key_type;
          auto &set = ret;

          auto table = *(wt.safe_access<std::shared_ptr<Table> > ().get ());
          for (auto &k : table)
            {
              auto s = table.size ();
              set.insert (std::move (DataFrom<KT> (k.first)));
            }
        }
      else if constexpr (ComesFrom<std::vector, retT>::value)
        {
          using VecValueType = typename retT::value_type;
          auto &vec = ret;

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
                LOG (fmt::format ("data at index {} not found", i));
            }
        }
      else
        static_assert (!std::is_same_v<retT, retT>, "not supported base type");
    }
  return ret;
}




// template<typename T>using _Data_To=decltype(DataTo<T>);
// template<typename T,typename En=void>struct is_DataTo:std::false_type{};
// template<typename T >struct is_DataTo<T,_Data_To<T>>:std::true_type{};


// template <typename T, typename _ = void> constexpr bool is_DataTo_able = false;
// template <typename T, typename _ = void> constexpr bool is_DataFrom_able = false;
// template <typename T> constexpr bool is_DataTo_able < T, decltype (DataTo (*(T*)0))> = true;
// template <typename T> constexpr bool is_DataFrom_able < T, decltype (DataFrom<T> (WT ()))> = true;
// template <typename T> constexpr bool is_Data_Supported = is_DataTo_able<T> && is_DataFrom_able<T>;

template <typename T>
constexpr bool is_Data_Supported_impl
    = std::is_integral_v<T> || std::is_floating_point_v<T> || std::is_enum_v<T> || std::is_same_v<T, std::string> || is_described<T>
      || ComesFrom<std::map,  T>::value || ComesFrom<std::set, T>::value || ComesFrom<std::vector,  T>::value;

template<typename T>
constexpr bool is_Data_Supported=is_Data_Supported_impl<std::decay_t<T>>;
} // namespace Detail
using Detail::DataFrom;
using Detail::DataTo;
} // namespace CTC
