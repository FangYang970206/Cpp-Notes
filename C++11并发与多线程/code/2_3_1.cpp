#include <thread>
#include <iostream>

using namespace std;

class TA
{
public:
	void operator()()
	{
		cout << "线程1开始启动" << endl;
		//...
		cout << "线程1结束运行" << endl;
	}

	void my_print()
	{
		cout << "线程2开始启动" << endl;
		//...
		cout << "线程2结束运行" << endl;
	}
};

int main()
{
	TA ta;
	thread myobj1(ta);
	myobj1.join();
	thread myobj2(&TA::my_print, ta);
	myobj2.join();
	cout << "i love china" << endl;
	cin.ignore();
	return 0;
}