#pragma once
#include <boost/describe.hpp>
#include <jungle/core/log.h> //LOG
#include <map>
#include <memory>
#include <string>
#include <tuple> // for static_assert pack template params
#include <type_traits>
#include <variant>
#include <vector>
namespace cpp_interface
{
namespace Data
{

BOOST_DEFINE_ENUM_CLASS (Data_Type, Invalid, Integer, Floating, String, Table_ /*, Compose*/);

// using Int = int64_t;
using Int = int;
using Float = float;
using String = std::string;
template <typename T, typename... rest> struct FirstArg
{
  using type = T;
};
using InvalidType = struct InvalidType
{
  constexpr bool
  operator<(InvalidType const &)
  {
    return true;
  }
};

/*  Table is incomplete, but DataItem need its comparator */
struct Table;
extern "C" bool Table_Less (std::shared_ptr<Table> l, std::shared_ptr<Table> r);
template <typename... Args> inline std::shared_ptr<Table> Construct_Table (Args... ags);

using _DataItem_Variant = std::variant<InvalidType, Int, Float, std::string, std::shared_ptr<Table> >;
struct DataItem : public _DataItem_Variant
{
  // using std::variant<InvalidType, Int, Float, std::string,
  //                    std::shared_ptr<Table>>::variant;
  bool
  operator<(DataItem const &ot) const
  {
    if (type != ot.type)
      return type < ot.type;
    switch (type)
      {
      case Data_Type::Integer:
        return std::get<Int> (*this) < std::get<Int> (ot);
      case Data_Type::Floating:
        return std::get<Float> (*this) < std::get<Float> (ot);
      case Data_Type::String:
        return std::get<std::string> (*this) < std::get<std::string> (ot);
      case Data_Type::Table_:
        return Table_Less (std::get<std::shared_ptr<Table> > (*this), std::get<std::shared_ptr<Table> > (ot));
      case Data_Type::Invalid:
        return true;
      }
    LOG ("");
    return false;
  }
  // initializer
  DataItem (Int d) : variant (d), type (Data_Type::Integer) {}
  DataItem (Float d) : variant (d), type (Data_Type::Floating) {}
  DataItem (std::string d) : variant (d), type (Data_Type::String) {}
  DataItem (InvalidType d) : variant (d), type (Data_Type::Invalid) {}
  DataItem (std::shared_ptr<Table> d) : variant (d), type (Data_Type::Table_) {}
  // more constructor for string and table
  template <typename... Args> DataItem (Args... args) { DelayConstruct (std::forward<Args> (args)...); }

  static DataItem
  InvalidInstance ()
  {
    return DataItem{ InvalidType{} };
  }
  template <typename... Args>
  void
  DelayConstruct (Args... args)
  {
    if constexpr (std::is_constructible<std::string, Args...>::value)
      {
        new (this) DataItem (std::string (std::forward<Args> (args)...));
      }
    else if constexpr (std::is_constructible<Table, Args...>::value)
      {
        new (this) DataItem (std::make_shared<Table> (std::forward<Args> (args)...));
      }
    else if constexpr (sizeof...(Args) == 1)
      {
        using TheOneArg = typename FirstArg<Args...>::type;
        if constexpr (std::is_integral_v<std::decay_t<TheOneArg> > || std::is_enum_v<TheOneArg>)
          new (this) DataItem (Int (args...));
        else if constexpr (std::is_floating_point_v<TheOneArg> )
          new (this) DataItem (Float (args...));
        else
          static_assert (!std::is_same_v<std::tuple<Args...>, std::tuple<Args...> >,
                         "there is no suitable constructor");
      }
  }
  template<typename T>
  T safe_access(){
    try{
      return std::get<T>(*this);
    }catch( std::bad_variant_access &e){
      LOG(fmt::format("cpp_interface::Data::DateItem bad access: {}",e.what()));
      return T();
    }
  }

  Data_Type type = Data_Type::Invalid;
};
// BOOST_DESCRIBE_STRUCT(
//     DataItem,
//     (_DataItem_Variant),
//     (type));

struct Table : public std::map<DataItem, DataItem>
{
  // note: using does not inherit copy constructor
  using map::map;
  Table (const map &m) : map (m) {}
  friend bool
  Table_Less (std::shared_ptr<Table> l, std::shared_ptr<Table> r)
  {
    if (l == nullptr || r == nullptr)
      return l == nullptr;
    return true;
  };
  // items;
  // ItemType& get(ItemType const &){};
};

// this is just a proxy function to let DataItem can just construct Table by
// map<>,cause if we  just use Table's constructor in place, it will be told
// Table is incomplete type
template <>
inline std::shared_ptr<Table>
Construct_Table<const std::map<DataItem, DataItem> &> (std::map<DataItem, DataItem> const &m)
{
  return std::make_shared<Table> (m);
};
using VariantDataBase = std::variant<Int, Float, std::string>;
struct DataBase : public VariantDataBase
{
  Data_Type type;
  // cpp-end data access
#define ist(T)                                                                                                         \
  bool is##T ()                                                                                                        \
  {                                                                                                                    \
    return type == Data_Type::T;                                                                                       \
  }
  ist (Invalid);
  ist (Integer);
  ist (Floating);
  ist (String);
  ist (Table_);

#undef ist
};
// grouped data
struct Data_Group
{
  // cpp-end, to access data by key, nullptr if
  DataBase GetData (std::string const &key);
  // cpp-end, set data , return if set success
  bool SetData (std::string const &key, DataBase &val);
};
} // namespace Data
} // namespace cpp_interface
