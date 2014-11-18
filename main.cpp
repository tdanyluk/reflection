#include "reflect.h"

struct S{
    MEMBER(unsigned, timePosix);
    MEMBER(int, year);
    MEMBER(int, month);
    MEMBER(int, day);
    MEMBER(int, hour);
    MEMBER(int, minute);
    MEMBER(int, second);
    MEMBER(int, fixStatus);
    MEMBER(double, lon);
    MEMBER(double, lat);
    MEMBER(int, alt);
    MEMBER(double, pdop);
    MEMBER(double, hdop);
    void fun()
    {
        int i = reflect::last_member_id<S>::value;
        std::cout <<i << std::endl;
    }
};

struct PrintSquared
{
    template<typename T>
    bool operator()(T& member, reflect::MemberInfo info)
    {
        return std::cout << info.name << "^2=" << member*member << std::endl;
    }
};

template<typename T>
struct Print2
{
    T& a;
    T& b;

    Print2(T& a_, T& b_)
    :a(a_)
    ,b(b_)
    {}

    template<typename Member>
    bool processMember()
    {
        return std::cout << Member::get(a) << " " << Member::get(b) << std::endl;
    }
};

int main()
{

    reflect::Reader read;
    reflect::Printer print;
    S s;
    s.fun();
    reflect::for_each_member(s, read);
    std::cout << std::endl;
    reflect::for_each_member(s, print);
    std::cout << std::endl;
    reflect::for_members<S::reflect_member_id_year,S::reflect_member_id_second>(s, print);
    Print2<S> print2(s,s);
    reflect::for_member_descriptors<S, S::reflect_member_id_year,S::reflect_member_id_second>(print2);
    std::cout << std::endl;
    reflect::for_member_descriptors<S>(print2);
    std::cout << std::endl;
    PrintSquared printSquared;
    reflect::for_each_member(s, printSquared);
    return 0;
}
