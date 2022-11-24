#include <jungle/cpp_interface.hpp>

int main() {
  using T = cpp_interface::Data::DataItem;
  using DataItem = cpp_interface::Data::DataItem;
  using Ta = cpp_interface::Data::Table;
  using Table = Ta;
  using STa = std::shared_ptr<Ta>;
  using Int = cpp_interface::Data::Int;
  T t(1);
  T ts[] = {T(Int(1))};
  STa root =
      std::make_shared<Ta>(std::initializer_list<STa::element_type::value_type>{
          {T(1), T(12345)},
          {T(2.f), T(2.345f)},
          {T("str_index"), T(12345)},
          {T("child"), std::make_shared<Ta>()}});
  std::map<T, T> m = {

      {T(1), T(12345)},
      {T(2.f), T(2.345f)},
      {T("str_index"), T(12345)},
      {T("child"), root}};

  Table t_from_map = m;
  DataItem from_map = m;
  DataItem from_initlizelist=std::initializer_list<Table::value_type>{{}};
  auto found_child_map = *(std::get<STa>(t_from_map[T("child")]));
  auto val1= found_child_map[T(1)];
  return 0;
}