#include <thread>
#include <iostream>

using namespace std;

void my_print()
{
	cout << "线程开始启动" << endl;
	//...
	cout << "线程结束运行" << endl;
}

int main()
{
	thread myobj(my_print);
	myobj.detach(); // 或者myobj.detach()
	if (myobj.joinable())
		cout << "joinable() == true" << endl;
	else
		cout << "joinable() == false" << endl;
	cout << "i love china" << endl;
	cin.ignore();
	return 0;
}