/***/
#include <ctc.hpp>
#include <iostream>
#include <struct.hpp>
#include <msg.hpp>
#include <vector>
int main ()
{
  CTC::CTC_Task_Raw task_raw;
  // clang-format off
  task_raw.conf = R"(s)";
  // clang-format on
  task_raw.type = CTC::CTC_Task_Raw::Type::json;
  std::shared_ptr<CTC::CTC_Task> task = task_raw.Parse ();
  // return task->Run () == CTC::CTC_Running_State::SUCCESS;
  boost::mp11::mp_for_each<
      boost::describe::describe_members<CTC::CTC_Task, boost::describe::mod_public | boost::describe::mod_protected> > (
      [&] (auto D) { std::cout << typeid ((CTC::CTC_Task*)0->*D.pointer).name () << std::endl; });
  using CTC_Task_=boost::describe::describe_members<CTC::CTC_Task, boost::describe::mod_public | boost::describe::mod_protected>;
  //using CTC_Task__=boost::describe::describe_members<int, boost::describe::mod_public | boost::describe::mod_protected>;

  CTC::CTC_Task tsk;
  static_assert(CTC::Detail::ComesFrom<std::vector,std::vector<int>>::value,"");
  static_assert(!CTC::Detail::ComesFrom<std::map,std::vector<int>>::value,"");
  boost_public_member_descriptor_fn((CTC::CTC_Task**)0);
  
    static_assert(CTC::Detail::is_described<CTC::CTC_Task_Raw>,"");
    CTC::CTC_Task_Raw raw;
    raw.conf="conf";
    raw.type=raw.json;
    raw.vi={1,2,3};
    auto p=CTC::DataTo(raw);

  return 0;
}
