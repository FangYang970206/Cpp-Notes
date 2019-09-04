#include <iostream>
#include <bitset>
#include <string>
#include <vector>
#include <tuple>

using namespace std;

// version1
void print1() {};

template <typename T, typename... Types>
void print1(const T& firstArg, const Types&... args)
{
    cout << firstArg << endl;
    print1(args...);
}

// version2
template < typename T , typename ... Types>
void print2 (const T& firstArg , const Types&... args)
{
    cout << firstArg << endl;
    if constexpr ( sizeof ...(args) > 0) print2 (args...) ;
}

// version3
template < typename T , typename ... Types>
void print3 (const T& firstArg , const Types&... args)
{
    cout << firstArg << endl ;
    initializer_list <T> {
        ( [&args] {cout << args << endl;}(), firstArg)...};
}

// version4
template <typename ... Types>
void print4 (const Types&... args)
{
    initializer_list <int> { ([&args] {cout << args << endl;}(), 0)...};
}

template < typename T , typename ... Types>
void print3_test1 (const T& firstArg , const Types&... args)
{
    cout << firstArg << endl ;
    auto i_l = initializer_list <T> {
            ( [&args] {cout << args << endl;}(), firstArg)...};
    for (auto i : i_l)
        cout << i << endl;
}

template < typename T , typename ... Types>
void print3_test2 (const T& firstArg , const Types&... args)
{
    cout << firstArg << endl ;
    auto i_l = initializer_list <T> {
            ( [&args] {cout << args << endl;}(), [&args] {cout << args << endl;}(), firstArg)...};
    for (auto i : i_l)
        cout << i << endl;
}

template <typename T>
ostream& operator << (ostream& os, const vector<T>& vec){
    os << "{ ";
    for (auto& v : vec)
        os << v << ' ';
    os << "}";
    return os;
}

template <typename ... Values> class mytuple1;
template <> class mytuple1<> {};

template <typename Head, typename ... Tail>
class mytuple1<Head, Tail...> : private mytuple1<Tail ...>
{
    using inherited = mytuple1<Tail...>;
public:
    mytuple1() {}
    mytuple1(Head v, Tail... vtail) : m_head(v), inherited(vtail...) {}
    Head head() {return m_head;}
    inherited& tail() {return *this;}
protected:
    Head m_head;
};

template <typename ... Values> class mytuple2;
template <> class mytuple2<> {};

template <typename Head, typename ... Tail>
class mytuple2<Head, Tail...>
{
    using composited = mytuple2<Tail...>;
public:
    mytuple2() {}
    mytuple2(Head v, Tail... vtail) : m_head(v), m_tail(vtail...) {}
    Head head() {return m_head;}
    composited& tail() {return m_tail;}
protected:
    Head m_head;
    composited m_tail;
};

template <int IDX, int MAX, typename... Args>
struct PRINT_TUPLE {
    static void print (ostream& os, const tuple<Args...>& t){
        os << get<IDX>(t) << (IDX + 1 == MAX ? "": ",");
        PRINT_TUPLE<IDX+1, MAX, Args...>::print(os, t);
    }
};

template <int MAX, typename... Args>
struct PRINT_TUPLE<MAX, MAX, Args...> {
    static void print (ostream& os, const tuple<Args...>& t){

    }
};

template <typename ... Args>
ostream& operator << (ostream& os, const tuple<Args...>& t) {
    os << "[";
    PRINT_TUPLE<0, sizeof...(Args), Args...>::print(os, t);
    return os << "]";
}

int main()
{
    print1(7.5, "hello", bitset<16>(377), 42);
    print2(7.5, "hello", bitset<16>(377), 42);
    print3(7.5, "hello", bitset<16>(377), 42);
    print4(7.5, "hello", bitset<16>(377), 42);
    print1(vector<int> {1, 2, 3, 4});
    print2(vector<int> {1, 2, 3, 4});
    print3(vector<int> {1, 2, 3, 4});
    print4(vector<int> {1, 2, 3, 4});
    mytuple1<int, float, string> t1(41, 6.3, "nico");
    print1(t1.head(), t1.tail().head(), t1.tail().tail().head());
    mytuple2<int, float, string> t2(41, 6.3, "nico");
    print1(t2.head(), t2.tail().head(), t2.tail().tail().head());
    cout << make_tuple(41, 6.3, "nico");
    return 0;
}