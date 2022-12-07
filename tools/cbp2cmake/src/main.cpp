/***/
#include <ctc.hpp>
#include <iostream>
#include <msg.hpp>
#include <struct.hpp>
#include <vector>
int
main ()
{
  static_assert (CTC::Detail::is_described<CTC::CTC_Task_Raw>, "");
  CTC::CTC_Task_Raw raw;
  using intAlias = int;
  static_assert (std::is_same_v<int, int>);
  
  static_assert (std::is_same<intAlias, int>::value);
  static_assert(std::is_enum_v<CTC::Detail::CTC_Task_Raw::Type>,"dsa");

  CTC::CTC_Task_Raw task_raw;
  // clang-format off
  task_raw.conf = R"(s)";
  // clang-format on
  task_raw.type = CTC::CTC_Task_Raw::Type::json;
  std::shared_ptr<CTC::CTC_Task> task = task_raw.Parse ();
  // return task->Run () == CTC::CTC_Running_State::SUCCESS;
  using CoutTestType = CTC::CTC_Task_Raw;
  boost::mp11::mp_for_each<
      boost::describe::describe_members<CoutTestType, boost::describe::mod_public | boost::describe::mod_protected> > (
      [&] (auto D) { std::cout << typeid ((CoutTestType *)0->*D.pointer).name () << std::endl; });
  using CTC_Task_
      = boost::describe::describe_members<CTC::CTC_Task, boost::describe::mod_public | boost::describe::mod_protected>;
  // using CTC_Task__=boost::describe::describe_members<int, boost::describe::mod_public |
  // boost::describe::mod_protected>;

  CTC::CTC_Task tsk;
  static_assert (CTC::Detail::ComesFrom<std::vector, std::vector<int> >::value, "");
  static_assert (!CTC::Detail::ComesFrom<std::map, std::vector<int> >::value, "");
  boost_public_member_descriptor_fn ((CTC::CTC_Task **)0);

  raw.conf = "conf";
  raw.type = raw.json;
  raw.vi = { 1, 2, 3 };
  auto p = CTC::DataTo (raw);

  auto praw=CTC::DataFrom<CTC::CTC_Task_Raw>(p);
  cpp_interface::Data::Table t;
  CTC::Detail::_External<CTC::CTC_Task_Raw> e{raw,t};
  using members=boost::describe::describe_members<CoutTestType, boost::describe::mod_public | boost::describe::mod_protected>;
  //e()

  return 0;
}
