#include <iostream>
#include <type_traits>
#include <variant>
#include <string>
using namespace std;

template <typename T>
void
f (T t)
{
  [] (auto _t) {
    if constexpr (std::is_same_v<decltype (_t), int>)
      cout << "int " << _t << endl;
    else if constexpr (std::is_same_v<decltype (_t), string>)
      cout << "string " << _t << endl;
  }(t);
}

int
main ()
{
  f (1);
  f (std::string ("str"));
  return 0;
}
