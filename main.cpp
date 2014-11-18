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

};

struct PrintSquared
{
    template<typename T>
    bool operator()(T& member, reflect::MemberInfo info)
    {
        return std::cout << info.name << "^2=" << member*member << std::endl;
    }
};

int main()
{
    reflect::Reader read;
    reflect::Printer print;
    S s;
    reflect::for_each_member(s, read);
    std::cout << std::endl;
    reflect::for_each_member(s, print);
    std::cout << std::endl;
    reflect::for_members<S::reflect_member_id_year,S::reflect_member_id_second>(s, print);
    std::cout << std::endl;
    PrintSquared printSquared;
    reflect::for_each_member(s, printSquared);
    return 0;
}
