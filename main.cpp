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

#define ESC(...) __VA_ARGS__

#define Args_1 \
    ESC(template<class MemberInfo, class T1>\
    bool process(T1& a))
#define Args_2 \
    ESC(template<class MemberInfo, class T1, class T2>\
    bool process(T1& a, T2& b))
#define Args_3 \
    ESC(template<class MemberInfo, class T1, class T2, class T3>\
    bool process(T1& a, T2& b, T3& c))
#define SavedArgs_0 (){}
#define SavedArgs_1(Arg1Type, Arg1Name)\
    ESC((Arg1Type& Arg1Name##_)\
    : Arg1Name(Arg1Name##_)\
    {}\
    Arg1Type Arg1Name;)
#define SavedArgs_2(Arg1Type, Arg1Name, Arg2Type, Arg2Name) \
    ESC((Arg1Type& Arg1Name##_, Arg2Type& Arg2Name##_)\
    : Arg1Name(Arg1Name##_)\
    , Arg2Name(Arg2Name##_)\
    {}\
    Arg1Type Arg1Name;\
    Arg2Type Arg2Name;)
#define SavedArgs_3(Arg1Type, Arg1Name, Arg2Type, Arg2Name, Arg3Type, Arg3Name) \
    ESC((Arg1Type& Arg1Name##_, Arg2Type& Arg2Name##_, Arg3Type& Arg3Name##_)\
    : Arg1Name(Arg1Name##_)\
    , Arg2Name(Arg2Name##_)\
    , Arg3Name(Arg3Name##_)\
    {}\
    Arg1Type Arg1Name;\
    Arg2Type Arg2Name;\
    Arg3Type Arg3Name;)

#define DefFun(FunName, SavedArgs, Args, code) \
    namespace\
    {\
        struct FunName\
        {\
            FunName SavedArgs\
            Args\
            {\
                code;\
                return true;\
            }\
        };\
    }

DefFun(EqMul2, SavedArgs_0, Args_2, a = b*2);

DefFun(Interpolate, SavedArgs_1(const double, ratio), Args_3,
       a = b*(1-ratio) + c*(ratio));

struct A{
    MEMBER(double, a);
    MEMBER(double, b);
    MEMBER(double, c);
};

int main()
{
    A a1 = {1,2,3};
    A a2 = {10,10,10};
    A a3;
    Interpolate inter(0.2);
    reflect::for_each_member(a3, a1, a2, inter);
    reflect::Printer print;
    reflect::for_each_member(a3, print);

    S a, b;

    reflect::Reader read;
    EqMul2 eqMul2;
    reflect::Printer2 print2;

    reflect::for_each_member(a, read);
    reflect::for_each_member(b, (const S&)a, eqMul2);

    std::cout << std::endl;
    reflect::for_each_member<S::reflect_member_id_year, S::reflect_member_id_second>(a, b, print2);
}
