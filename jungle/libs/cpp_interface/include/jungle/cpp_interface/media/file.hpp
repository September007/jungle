#pragma once
#include "media_base.hpp"
#include <boost/describe.hpp>
#include <fstream>
#include <map>
#include <string>
namespace cpp_interface {
namespace Data {

namespace file_interface {

template <template <typename... cls> typename Container>
struct Data_Group_impl : cpp_interface::Data::Data_Group {
  DataItem GetData(std::string const &key) {
    if (auto p = kvs.find(key); p != kvs.end())
      return p->second;
    else
      return DataItem::InvalidInstance();
  }
  bool SetData(std::string const &key, DataItem &val) {
    return kvs.insert({key, val}).second;
  }
  Container<std::string, DataItem> kvs;
};

using Data_Group = Data_Group_impl<std::map>;
} // namespace file_interface
} // namespace Data
} // namespace cpp_interface