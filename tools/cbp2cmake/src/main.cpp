/***/
#include <boost/filesystem.hpp>
#include <ctc.hpp>
#include <iostream>
#include <msg.hpp>
#include <struct.hpp>
#include <vector>

int
main ()
{
  CTC::CTC_Task_Raw raw;

  auto task = raw.Parse ();
  task->Init ();
#define prefix "../../../tools/codeblocks_to_cmake/test/app/app.cbp"
  task->cbpfiles = { prefix };
  for (auto &path : task->cbpfiles)
    path = boost::filesystem::canonical (path).generic_string ();
  task->Run ();

  return 0;
}
