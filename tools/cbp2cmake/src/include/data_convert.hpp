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

// template<typename T>
// void DataTo<T>(T&t,Table&table){
//     using DT = decltype ((std::decay_t<T> *)0->*D.pointer);
//     if constexpr (std::is_same_v<int, DT> || std::is_same_v<float, DT> || std::is_same_v<std::string, DT>) // base
//     type
//       table.insert (std::make_pair<WT, WT> (D.name, t.*D.pointer));
// }

template <typename T> inline WT DataTo (T &origin_t);
template <typename T> struct _External
{
  T &t;
  Table &table;
  template <typename TD>
  void
  operator() (TD D)
  {
    LOG (fmt::format ("D.pointer: {}", typeid (D.pointer).name ()));
    using DT = std::decay_t<decltype ((std::decay_t<T> *)0->*D.pointer)>;
    LOG (fmt::format ("DT: {}, std::string:: {},val: {}", typeid (DT).name (), typeid (std::string).name (),
                      std::is_same<std::string, DT>::value));
    LOG (fmt::format ("DT: {}, int: {},val: {},DThash:{},inthash:{}\n", typeid (DT).name (), typeid (int).name (),
                      std::is_same<int, DT>::value, typeid (DT).hash_code (), typeid (int).hash_code ()));

    if constexpr (std::is_integral_v<DT> || std::is_enum_v<DT>)
      {
        table.insert (std::make_pair<WT, WT> (D.name, int (t.*D.pointer)));
      }
    else if constexpr (std::is_floating_point_v<DT>)
      {
        table.insert (std::make_pair<WT, WT> (D.name, float (t.*D.pointer)));
      }
    else if constexpr (std::is_same<std::string, DT>::value)
      {
        table.insert (std::make_pair<WT, WT> (D.name, std::string (t.*D.pointer)));
      }
    else if constexpr (ComesFrom<std::map, DT>::value)
      {
        auto childTable = WT (std::make_shared<Table> ());
        auto key = WT (D.name);
        DataTo (t.*D.pointer);
      }
    else if constexpr (ComesFrom<std::vector, DT>::value)
      { // vector and map both store in table
        auto childTable = std::make_shared<Table> ();
        auto key = WT (D.name);
        auto &vec = t.*D.pointer;
        for (int i = 0; i < int (vec.size ()); ++i)
          childTable->insert (std::make_pair<WT, WT> (i, DataTo (vec[i])));
        table.insert (std::make_pair<WT, WT> (D.name, childTable));
      }
    else
      {
        static_assert (!std::is_same_v<DT, DT>, "not supported base type");
      }
  };
};
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
            table.insert (std::make_pair<WT, WT> (D.name, int (t.*D.pointer)));
          }
        else if constexpr (std::is_floating_point_v<DT>)
          {
            table.insert (std::make_pair<WT, WT> (D.name, float (t.*D.pointer)));
          }
        else if constexpr (std::is_same<std::string, DT>::value)
          {
            table.insert (std::make_pair<WT, WT> (D.name, std::string (t.*D.pointer)));
          }
        else if constexpr (ComesFrom<std::map, DT>::value)
          {
            auto childTable = WT (std::make_shared<Table> ());
            auto key = WT (D.name);
            DataTo (t.*D.pointer);
          }
        else if constexpr (ComesFrom<std::vector, DT>::value)
          { // vector and map both store in table
            auto childTable = std::make_shared<Table> ();
            auto key = WT (D.name);
            auto &vec = t.*D.pointer;
            for (int i = 0; i < int (vec.size ()); ++i)
              childTable->insert (std::make_pair<WT, WT> (i, DataTo (vec[i])));
            childTable->insert (std::make_pair<WT, WT> ("size", cpp_interface::Data::Int (vec.size ())));
            table.insert (std::make_pair<WT, WT> (D.name, childTable));
          }
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
      if constexpr (std::is_integral_v<T> || std::is_enum_v<T> || std::is_same_v<float, T>
                    || std::is_same_v<std::string, T>) // base type
        return WT (t);
      else
        static_assert (!std::is_same_v<T, T>, "not supported base type");
    }
  return WT (); // this just to erase warning
}

template <typename T>
inline std::decay_t<T>
DataFrom (WT &wt)
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
      boost::mp11::mp_for_each<D1> ([&] (auto D)->void {
        using DT = std::decay_t<decltype ((std::decay_t<T> *)0->*D.pointer)>;
        auto &val = table[WT (D.name)];
        if constexpr (std::is_integral_v<DT> || std::is_enum_v<DT>)
          {
            ret.*D.pointer = DT(val.safe_access<cpp_interface::Data::Int> ());
          }
        else if constexpr (std::is_floating_point_v<DT>)
          {
            ret.*D.pointer = val.safe_access<cpp_interface::Data::Float> ();
          }
        else if constexpr (std::is_same<std::string, DT>::value)
          {
            ret.*D.pointer = val.safe_access<cpp_interface::Data::String> ();
          }
        else if constexpr (ComesFrom<std::map, DT>::value)
          {
            auto childTable = WT (std::make_shared<Table> ());
            auto key = WT (D.name);
            DataTo (ret.*D.pointer);
          }
        else if constexpr (ComesFrom<std::vector, DT>::value)
          {
            using VecValueType = DT::value_type;
            auto shared_childTable = val.safe_access<std::shared_ptr<cpp_interface::Data::Table> > ();
            if (shared_childTable == nullptr)
              {
                LOG ("childTable is null");
                return ;
              }
            auto &childTable = *shared_childTable.get ();

            auto &vec = ret.*D.pointer;
            int size = childTable[WT ("size")].safe_access<cpp_interface::Data::Int>();
            vec.resize (size);
            WT tempIndex = WT (0);
            for (int i = 0; i < size; ++i)
              {
                tempIndex = i;
                if (auto p = childTable.find (tempIndex); p != childTable.end ())
                  vec[i] = DataFrom<VecValueType> (p->second);
              }
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
      else
        static_assert (!std::is_same_v<retT, retT>, "not supported base type");
    }
    return ret;
}
} // namespace Detail
using Detail::DataTo;
using Detail::DataFrom;
} // namespace CTC
