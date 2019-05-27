# C++面向对象程序设计_Part2

part1讲述了基于对象，part2则是在基于对象的基础上，建立类与类之间的联系，即面向对象编程以及面向对象设计。

主要讲述以下三点：

- Inheritance （继承）
- Composition（复合）
- Delegation （委托）

另外，我把补充内容中的对象模型放入到Part2，我觉得放入这里更加合适。

[TOC]

## Composition（复合）

![1558453874580](assets/1558453874580.png)

Composition（复合）就是has a， 上面的事例就是队列（queue）类中有一个双端队列（deque）类，队列中的成员函数通过双端队列的操作函数完成，这是类与类之间的第一个关系。（黑色菱形代表内部拥有）

deque中可能拥有很多方法，但queue中只通过deque提供了非常少的方法，这是一个设计模式Adapter，将一个拥有许多功能的类改造一下，得到另一个类。

## 内存视角下的composition（复合）

![1558459151904](assets/1558459151904.png)

可以看到有两个复合关系，最后queue的内存是40.

## composition（复合）关系下的构造与析构

![1558457528106](assets/1558457528106.png)

由于Container类是拥有Component类，所以在构造方面，先调用Component类的默认构造函数，然后再调用Container的构造函数，由内而外的构造，里面做好了，再做外面。析构则相反，先对Container进行析构，然后再对Component进行析构，过程是由外而内，将外面的去掉，才能看到里面去掉里面，符合常识。

## Delegation (委託) —— Composition by reference

![1558457888072](assets/1558457888072.png)

如果一个类（string）中拥有一个指针（StringRep*），该指针指向的是另一个类（StringRep），这种关系是Delegation（委托），更好的说法就是Composition by reference（学术界不说by pointer），两种类的生命周期不一样，与复合两种类会同时初始化不同，委托当需要用的时候再进行初始化。上图中的实例是一种非常有名的设计，叫handle/body，指针指向的类负责具体实现，可以看到有一个//pimpl，意思是pointer to implement，而拥有那个指针的类只提供外界接口，就是基于委托这种方式，Handle（string）是提供给外界的接口，body（StringRep）就是实现部分，为什么这么有名，这是因为String类设计好了就不需修改了，只需要修改实现的那一个类，具有很好的弹性，另外，还有可以进行共享机制（减小内存），下图的a，b，c共享一个StringRep，这种方式叫做reference counting，当需要修改其中一个时，需要把内容copy出来一份进行修改，另外两个依然指向原StringRep。（白色菱形代表指针指向）

## Inheritance （继承）

![1558491481526](assets/1558491481526.png)

继承的语法就是在类名后加上：public（还可以是protected，private）你想要继承的类，如果想继承多个类，用逗号隔开就可以了。什么时候用继承，确定一个关键点，子类is a 父类（例如，狗is a动物）。上述的List_nodes是继承了List_node_base所有的数据，另外还有自己的数据。

## Inheritance （继承）关系下的构造与析构

![1558716277153](assets/1558716277153.png)

继承的类（derived object）的一部分是基类（base part），对于要被继承的基类，它的析构函数必须是virtual，不然会出现问题，这个问题将在后面说。继承的构造函数会首先调用基类的构造函数，然后调用自己的构造函数（由内而外）。析构则相反，先析构自己，然后再调用基类的析构函数。

## Inheritance （继承）with virtual functions（虚函数）

![1558716850059](assets/1558716850059.png)

子类继承了父类的两样东西，一种是父类的数据，一种是父类函数的调用权。对于一个类而言，它的子类都可以访问所以的public方法，而子类要不要重新定义父类的函数呢？这时候就需要虚函数了，当public里面的函数不是虚函数时，则希望子类不重新定义该函数。当函数是虚函数时（在返回类型前加入关键字virtual），则希望子类重新定义它，并且父类已经有了默认定义。当函数是纯虚函数时（在结束符；前面加上=0），则希望子类一定要重新定义它，父类没有默认定义（但可以有默认定义）。该事例是定义了一个基类shape，然后矩形Rectangle和椭圆Ellipse对shape进行继承，基类的objectID是无需继承的，可以提前定好，在父类调用即可，而error函数，父类有默认的错误信息，如果子类有更精细的错误信息，父类允许子类可以重新定义的，打印出子类调用时的错误，而draw函数则必须重新定义，父类没有定义（draw shape没有意义），子类不同，所画出的形状自然不同。

## Inheritance （继承）with virtual ——经典实例

![1558718409179](assets/1558718409179.png)

对于在powerpoint打开ppt文件而言，有以下几步，先点打开命令的菜单栏，然后出现文件界面，选择我们要打开的文件名，然后程序会检查文件名是否符合规范，符合规范则在磁盘上搜索文件，搜索到了打开文件即可。而遇到注意的是，所以打开文件的过程都是这样，只有最后打开文件可能会不同（可能会打开不同格式的文件），于是有团队就将除文件打开函数以外的函数进行打包，子类直接继承，只要子类重新定义父类打开文件的函数即可。如下图所示：

![1558718827030](assets/1558718827030.png)

团队开发了CDocument类，定义Serialize函数需要重新定义，在OnFileOpen函数中的省略号即为打包好的过程。用CDocument类的人只需重新定义Serialize函数即可，则在main函数中，先创建一个CMyDoc实例myDoc，调用myDoc.OnFileOpen函数，子类没有定义这个函数，实则调用的是父类的函数，即CDocument::OnFileOpen(&myDoc), 进入父类函数中，运行打包好的过程，当运行到Serialize函数时，发现子类重新定义了它，则调用子类重新定义的Serialize函数，最后再返回到CDocument::OnFileOpen，继续下面的过程。再也不用写一般的步骤了，完美！这是一种非常有名的设计模式Template method（不是说C++ template），提供了一种应用框架，它将重复一样的操作写好，不确定的步骤留给实际应用设计者重新实现。十年前最有名的产品MFC就是这样一种应用框架。

深层次的理解，谁调用函数，this就是谁，当调用Serialize函数是，编译器是通过this->Serialize()调用，于是就调用到了this重新定义的Serialize函数。

![1558752779674](assets/1558752779674.png)

上图就是CDocument和CMyDoc的实例，用cout来模拟步骤，呼应上面两张图片。

## Inheritance + Composition关系下的构造与析构

![1558753009197](assets/1558753009197.png)

当同时存在继承和复合，类是如何进行构造和析构呢？这一节要讨论的问题：

1. 子类有父类的成分还有包含着另一个类；
2. 子类继承父类，父类拥有另外一个类。

情况2就很明显了，构造依然是自内而外，析构是由外而内。

对于情况1，这是侯捷老师留的作业，自己写代码判断，我写了一个：

```c++
#include <iostream>

using namespace std;

class Base;
class Derived;
class Component;

class Base{
public:
    Base() {cout << "Base Ctor" << endl;}
    virtual ~Base() {cout << "Base Dtor" << endl;}
};

class Component{
public:
    Component() {cout << "Component Ctor" << endl;};
    ~Component() {cout << "Component Dtor" << endl;};
};

class Derived : Base{
public:
    Derived() {cout << "Derived Ctor" << endl;}
    ~Derived() {cout << "Derived Dtor" << endl;}
protected:
    Component c;
};

int main() {
    {Derived d;}
    cin.ignore();
    return 0;
}
```

运行结果为：

```bash
Base Ctor
Component Ctor
Derived Ctor
Derived Dtor
Component Dtor
Base Dtor
```

可以看到先初始化父类（Base），然后再初始化Component类，再初始化自己，析构与构造相反。

下图也给出了结论。

![1558760439215](assets/1558760439215.png)

至此，面向对象的概念说完了，下面进入实例环节。

## Delegation (委托) + Inheritance (继承) （一）

![1558873574510](assets/1558873574510.png)

上述代码解决的是下图所示的问题，对同一份文件使用四个不同窗口去查看，或者右下角所示的，同一个数据，三种不同的viewer查看。数据只有一份，表现多种形式，数据变化，表现形式也会发生变化，要表现这样的特性，这就对表现文件的class和存储数据的class之间关系要有要求，上图就是下图的一种解法，23种设计模式之一。Subject类是存储数据的类，不过类中有delegation，使用了一个vector类用来存放Observer类的指针，这样Observer类以及它的所有子类都可以导入这个vector中，Observer类相当于表现形式类的父类，可以有多种表现形式，这些都是子类。update则是子类需要重新定义的方法，不同表现形式可以有不同的更新方法。对于Subject类来说，当我们想创建新的窗口（新的observer类）去查看它的时候，需要对将新的Observer类进行注册，函数attach就是实现这样的功能，可以将新的observer子类的指针加到vector中，注销的函数没有写出来，另外，当数据发生变化时，使用set_val函数，需要使用一个函数去更新所有的observer子类，这就是notify函数干的事，遍历vector每一个observer指针，调用指针指向的update方法，对表现形式进行更新。Delegation + Inheritance真的感觉好强大呀。

![1558873750010](assets/1558873750010.png)

下图是一个更详细的Observer解法构建

![1558875339692](assets/1558875339692.png)

## Delegation (委托) + Inheritance (继承) （二）

![1558887391028](assets/1558887391028.png)

第二个实例，构建一个文件系统。可以把Primitive类当作文件类，而Composite类当作目录类，与日常使用的文件系统一样，目录里面可以包含目录，也可以包含文件，所以目录里面存放的不止是目录本身还可以是文件，但是需要放入到同一个容器中，想法是使用指针，但文件和目录是不太一样的，所以解决方案是将文件和目录共同继承Component类，然后Composite类中的容器存放的是Component的指针，这样就可以既存放文件又可以存放目录了，这是一种经典的解决方案，也是23种设计模式之一，Composite。另外，Component类中还有一个虚函数add，这是给目录进行继承的，因为目录可以新建目录和文件，这里不能设置为纯虚函数，因为文件不能继承这个函数，文件是不能在进行添加的。

## Prototype

![1558923111947](assets/1558923111947.png)

这又是23种设计模式之一的Prototype，Prototype要解决的是要设计一个类，这个类是为未来的子类所设计的，他可以访问和操作未来的子类，这就很有意思了，都不知道未来的子类是啥，要去访问这个子类，这是怎么去做呢？原来在子类内部会申明一个关于子类的静态变量，就是上图中的<u>LSAT</u> : LandSatImage，另外这个子类会定义一个私有的构造方法（前面有一个负号，可以通过定义静态变量调用私有的构造方法），构造方法里面会调用父类的addPrototype函数，将静态变量的指针传到父类，父类就把传入的指针（通过父类的指针形式）加入到自己的容器当中，这样父类就知道子类的类型，就可以操作子类了，上述操作是这样的，父类有一个findAndClone函数，根据输入参数i选择父类容器中的子类进行clone，返回子类的指针，而clone父类定义的是一个纯虚函数，子类必须重新定义，上图中子类重新定义的是返回新建一个子类，返回它的指针，不过这个新建是调用的另外一个构造函数（有一个＃号，代表protect），使用private里面的构造函数是不行的，它是为父类打通桥梁的，为了与private里面的构造函数区分开，形参有一个int类型，这个int类型不会进行使用。

下面的图片是相关代码，解释上面的文字已经说的很清楚了。

父类Image

![1558925338958](assets/1558925338958.png)

两个子类代码：

![1558925377433](assets/1558925377433.png)

主函数：

![1558925446700](assets/1558925446700.png)

Prototype例子来自于《Design Patterns Explained Simply》这本书，经典致敬！

![1558925624955](assets/1558925624955.png)

面向对象的例子讲完了，下面介绍更加深层次的内容，理解面向对象更底层的东西。

