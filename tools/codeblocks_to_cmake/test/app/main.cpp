#include <iostream>

#ifdef DEBUG
#include <version_debug.h>
#else
#include <version_release.h>
#endif
#include <3rd.h>
using namespace std;

int main()
{
    cout << "Hello world in " <<__version<< endl;
    cout << str_3rd<<endl;
    cout << str_3rd_c<<endl;
    return 0;
}
