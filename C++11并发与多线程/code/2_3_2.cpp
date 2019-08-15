#include <thread>
#include <iostream>

using namespace std;

class TA
{
private:
	int m_data;
public:
	TA(int m) : m_data(m) { cout << "TA构造函数" << endl; }
	TA(TA& t) : m_data(t.m_data) { cout << "TA拷贝构造" << endl; }
	~TA() { cout << "TA析构函数" << endl; }
	void operator()()
	{
		cout << "线程1开始启动" << endl;
		cout << "m_data: " << m_data << endl;
		cout << "线程1结束运行" << endl;
	}

	void my_print()
	{
		cout << "线程2开始启动" << endl;
		cout << "m_data: " << m_data << endl;
		cout << "线程2结束运行" << endl;
	}
};

int main()
{
	TA ta(6);
	cout << "ta 构造完成" << endl;
	thread myobj1(ta);
	myobj1.join();
	thread myobj2(&TA::my_print, ta);
	myobj2.join();
	cout << "i love china" << endl;
	return 0;
}