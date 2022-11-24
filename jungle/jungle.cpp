#include <iostream>
#include <variant>
using namespace std;

struct B
{
	bool b;
	virtual void v()=0;
};
struct D : public B
{
	void v(){};
	string s;
};

int main()
{
	char buf[1024];
	cout << string(10, '1') << '\n';
	setbuf(stdout, buf);
	cout << string(10, '2') << '\n';
	setbuf(stdout, 0);
	cout << string(10, '3') << '\n';
	// D{{true},""};
	std::variant<int,string,D> d;
	d=1;
	d={};
	cout << endl;
	return 0;
}
