#include <iostream>
using namespace std;



int main(){
	char buf[1024];
	cout << string(10,'1')<<'\n';
	setbuf(stdout, buf);
	cout << string(10, '2') << '\n';
	setbuf(stdout, 0);
	cout << string(10, '3') << '\n';

	cout << endl;
	return 0;
}
