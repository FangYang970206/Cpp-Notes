#include <thread>
#include <iostream>

using namespace std;

int main()
{
	auto ta = []() { cout << "线程开始启动" << endl;
	cout << "线程结束运行" << endl; };
	thread myobj(ta);
	myobj.join();
	cout << "i love china" << endl;
	cin.ignore();
	return 0;
}