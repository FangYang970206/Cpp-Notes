#include <iostream>

using namespace std;

namespace fy1{
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

    void fy1_test(){
        Derived d;
    }
}

namespace fy2{
    class A{
    public:
        virtual void vfunc1() {cout << "A::vfunc1()" << endl;}
    private:
        int data1;
    };

    class B : public A{
    public:
        virtual void vfunc1() {cout << "B::vfunc1()" << endl;}
    private:
        int data2;
    };

    void fy2_test(){
        B b;
        A a = (A) b;
        a.vfunc1();

        A* pa = new B;
        pa->vfunc1();

        pa = &b;
        pa->vfunc1();
    }
}


int main() {
    cout << "Ctor and Dtor test:" << endl;
    fy1::fy1_test();
    cout << endl;
    cout << "object model test:" << endl;
    fy2::fy2_test();
    return 0;
}