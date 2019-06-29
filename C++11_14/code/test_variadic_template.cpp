#include <iostream>
#include <bitset>
#include <string>

using namespace std;


void print() {};

template<typename T, typename... Types>
void print(const T &firstArg, const Types &... args) {
    cout << firstArg << endl;
    print(args...);
}

template<typename... Types>
void print(const Types &... args)
{
    cout << "common print" << endl;
}

template<typename T, typename... Args>
void printf(const char *s, T val, Args... args) {
    while (*s) {
        if (*s == '%' && *(++s) != '%') {
            cout << val;
            printf(++s, args...);
            return;
        }
        cout << *(s++);
    }
    throw logic_error("extra arguments provided to printf");
}

void printf(const char *s) {
    while (*s) {
        if (*s == '%' && *(++s) != '%')
            throw runtime_error("invalid format string: missing arguments");
        cout << *(s++);
    }
}

template <typename ... Values> class tuple1;
template <> class tuple1<> {};

template <typename Head, typename... Tails>
class tuple1<Head, Tails...> :private tuple1<Tails...>
{
    typedef tuple1<Tails...> inherited;
public:
    tuple1() {}
    tuple1(Head v, Tails... tails)
    : m_head(v), inherited(tails...) {}

    Head head() {return m_head;}
    inherited& tail() {return *this;}
protected:
    Head m_head;
};

template <typename ... Values> class tuple2;
template <> class tuple2<> {};

template <typename Head, typename ... Tails>
class tuple2<Head, Tails...>
{
    typedef tuple2<Tails...> composited;
protected:
    Head m_head;
    composited m_tail;
public:
    tuple2() {}
    tuple2(Head v, Tails... vtails)
    : m_tail(vtails...), m_head(v) {}

    Head head() {return m_head;}
    composited& tail() {return m_tail;}
};



int main() {
    print(7.5, "hello", bitset<16>(377), 42);
    int* p = new int();
    printf("%d %s %p %f", 15, "This is ace", p, 3.1415);
    tuple1<int, float, string> t1(41, 6.3, "nico");
    cout << sizeof(t1) << endl;
    cout << t1.head() << endl;
    cout << t1.tail().head() << endl;
    cout << t1.tail().tail().head() << endl;
    tuple2<int, float, string> t2(41, 6.3, "nico");
    cout << sizeof(t2) << endl;
    cout << t2.head() << endl;
    cout << t2.tail().head() << endl;
    cout << t2.tail().tail().head() << endl;
    return 0;
}