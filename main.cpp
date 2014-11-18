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

struct EqMul2
{
    template<class MemberInfo, class MemberType>
    bool process(MemberType& a, const MemberType& b)
    {
        a = b * 2;
        return true;
    }
};

int main()
{
    S a, b;

    reflect::Reader read;
    EqMul2 eqMul2;
    reflect::Printer2 print2;

    reflect::for_each_member(a, read);
    reflect::for_each_member(b, (const S&)a, eqMul2);

    std::cout << std::endl;
    reflect::for_each_member(a, b, print2);
}
