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
template <typename T>
inline constexpr int
boost_public_member_descriptor_fn (T **)
{
  return 0;
}
template <typename T>
constexpr bool is_described = std::is_same_v<decltype (boost_public_member_descriptor_fn ((T **)0)), int>;

// template<typename T>
// void DataTo<T>(T&t,Table&table){
//     using DT = decltype ((std::decay_t<T> *)0->*D.pointer);
//     if constexpr (std::is_same_v<int, DT> || std::is_same_v<float, DT> || std::is_same_v<std::string, DT>) // base
//     type
//       table.insert (std::make_pair<WT, WT> (D.name, t.*D.pointer));
// }

template <typename T>
inline WT
DataTo (T &t)
{
  if constexpr (is_described<T>) // described class
    {
      WT ret (std::make_shared<Table> ());
      Table &table = *std::get<std::shared_ptr<Table> > (ret).get ();
      using D1 = boost::describe::describe_members<T, boost::describe::mod_public | boost::describe::mod_protected>;

      boost::mp11::mp_for_each<D1> ([&] (auto D) {
        using DT = decltype ((std::decay_t<T> *)0->*D.pointer);
        if constexpr (std::is_same_v<int, DT> || std::is_same_v<float, DT>
                      || std::is_same_v<std::string, DT>) // base type
          table.insert (std::make_pair<WT, WT> (D.name, t.*D.pointer));
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
            table.insert (std::make_shared<WT, WT> (D.name, childTable));
          }
        else
            static_assert (std::is_same_v<T, T>, "not supported base type");
      });
      return ret;
    }
  else // base type
    {
      if constexpr (std::is_same_v<int, T> || std::is_same_v<float, T> || std::is_same_v<std::string, T>) // base type
        return WT (t);
      else
        static_assert (std::is_same_v<T, T>, "not supported base type");
    }
  return WT (); // this just to erase warning
}
} // namespace Detail
using Detail::DataTo;
} // namespace CTC
