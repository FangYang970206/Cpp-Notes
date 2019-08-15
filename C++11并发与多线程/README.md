# C++并发与多线程笔记

[TOC]

## 前言

以往计算机是单核处理器的，某一时刻只能执行一个任务，由操作系统调度，每秒钟进行多次所谓的“任务切换”，这是一种并发的假象，不是真正的并发，这种切换叫上下文切换，是有时间开销的，比如操作系统要保存切换时的各种状态、执行的进度等信息，都需要时间，一会儿切换回来的时候需要复原这些信息。

而当代计算机绝大多数都是多核处理器，能够真正的并行执行多个任务（硬件并发）。并发的目的是为了能够同时干多个事，提高性能，它指的是两个或者更多的任务（独立的活动）同时进行，一个程序同时执行多个任务；如下图就是双核CPU并发执行任务的过程。

![1565868441548](assets/1565868441548.png)

## 1. 进程、线程与并发

**进程**：一个可执行程序运行起来，就创建了一个进程。Windows下双击一个可执行程序，Linux下，./文件名。可以通过在window任务管理器中查看进程的情况，而在Linux中则是通过`ps aux`命令。

**线程**：用来执行代码的，理解成一条代码的执行道路。每个进程就是执行起来的可执行程序，都有一个主线程，这个主线程是唯一的，一个进程中只能有一个主线程。当你执行可执行程序，产生了一个进程后，这个主线程就随着这个进程默默的启动起来了。程序运行起来的时候，实际上是进程中的主线程执行这个main函数的代码；主线程和进程不可分割。除了主线程，可以通过代码创建其他线程。每创建一个新线程，就可以在同一时刻，可以多做一件事。

**并发**：多进程并发和多线程并发。

- 多进程并发：进程之间通信。同一个电脑，管道，文件，消息队列，共享内存等；不同电脑，socket通信技术。

- 多线程并发：一个进程中的所有线程共享地址空间（共享内存），全局变量，指针，引用都可以在线程之间传递：使用多线程开销小于多进程。

和进程相比，线程有如下优点：线程启动速度更快，更轻量级；系统开销更小，执行速度跟块，比如共享内存这种通信方式比任何其他的通信方式更快。

## 2. 线程启动、结束，创建线程方法、join和detach

### 2.1 线程运行的开始和结束

主线程从main()开始执行，自己创建的线程，也需要从一个函数开始运行（初始函数），一旦这个函数运行完毕，就代表这这个线程结束。

整个进程时候执行完毕的标志是：主线程是否执行完毕，如果进程执行完毕了，就代表着整个进程执行完毕了；

一般情况下，如果其他子线程没有执行完毕，但是主线程执行完毕了，这些子线程也会被操作系统强行终止。如果要保持子线程的运行状态的话，那么就要让主线程一直运行。也有例外情况，使用detach()，主线程结束子线程继续运行。

### 2.2 join和detach

通过一个例子说明：

```c++
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
	myobj.join(); // 或者myobj.detach()
	cout << "i love china" << endl;
	cin.ignore();
	return 0;
}
```

首先创建线程需要包括头文件thread，直接通过thread类创建一个线程对象，如thread myobj(my_print)，线程的入口是my_print，创建同时开始运行。可以看到下面接着调用myobj的join方法，注释当中还显示着可以调用detach方法。

**join**：汇合，阻塞主线程，让主线程等待子线程执行完毕，然后子线程和主线程汇合，然后主线程继续执行。

**detach**：分离，主线程和子线程分离，各自执行各自的。引入原因：创建很多子线程，让主线程逐个等待子线程结束，不太好，所以引入detach。一旦detach之后，与这个主线程关联的thread对象就会失去与这个主线程的关联。此时子线程就会在后台运行，当子线程执行完毕后，有运行时库负责清理相关的资源（守护线程）。detach使线程失去控制。一旦使用detach后，不能再用join了。

运行join的输出如下：

```bash
线程开始启动
线程结束运行
i love china
```

运行detach可能的输出如下（运行时可能会有不同）：

```bash
线程开始启动
i love china
线程结束运行
```

可以看到join确实是等子线程运行完毕，主线程才开始运行。而detach则是主线程和子线程分开运行。

**joinable**：判断当前进程是否可以进行join，可以返回true，否则返回false。

### 2.3 用类和lambda表达式创建线程

用类创建线程有两种方式：一种是重载括号 operator()()； 第二种则是调用成员函数

重载括号：thread myobj(对象名，参数)；
调用成员函数： thread myobj(&类型：成员函数名，对象名，参数)；

例子与上述例子类似：

```c++
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
```

join运行结果如下：

```bash
线程1开始启动
线程1结束运行
线程2开始启动
线程2结束运行
i love china
```

上述中的ta是一个局部对象，有一点需要注意，在使用detach的时候，主线程先结束，ta就会被系统回收，那怎么能运行ta中的函数？实际上ta对象是被复制到线程中去的，所以主线程回收ta不会影响子线程。下面代码进行了测试。

```c++
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
```

运行结果如下，可以看到线程先调用了拷贝构造，然后在线程结束时析构拷贝的TA。

```bash
TA构造函数 //主函数TA构造
ta 构造完成
TA拷贝构造 //线程1开始
线程1开始启动
m_data: 6
线程1结束运行
TA析构函数 //线程1结束，释放拷贝的TA
TA拷贝构造 //线程2开始
线程2开始启动
m_data: 6
线程2结束运行
TA析构函数 //线程1结束，释放拷贝的TA
i love china
TA析构函数 //主函数TA析构
```

然后再看lambda表达式创建线程。

```c++
#include <thread>
#include <iostream>

using namespace std;

int main()
{
	auto ta = [](){ cout << "线程开始启动" << endl; 
				    cout << "线程结束运行" << endl; };
	thread myobj(ta);
	myobj.join();
	cout << "i love china" << endl;
	cin.ignore();
	return 0;
}
```

