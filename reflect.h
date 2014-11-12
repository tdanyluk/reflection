#ifndef REFLECT_H
#define REFLECT_H
#include <iostream>

#define _reflect_VALID(T,i) (sizeof(reflect::impl::valid<T,i>(0))==sizeof(char)?1:0)

#define MEMBER(type_, var)  \
    template<int i, int _>\
    struct _reflect_Member;\
    template<template<int,int> class T>\
    struct _reflect_index_of_##var\
    {\
        enum\
        {\
            i128=_reflect_VALID(T,127)*128,\
            i64=i128+_reflect_VALID(T,i128+63)*64,\
            i32=i64+_reflect_VALID(T,i64+31)*32,\
            i16=i32+_reflect_VALID(T,i32+15)*16,\
            i8=i16+_reflect_VALID(T,i16+7)*8,\
            i4=i8+_reflect_VALID(T,i8+3)*4,\
            i2=i4+_reflect_VALID(T,i4+1)*2,\
            value=i2+_reflect_VALID(T,i2)\
        };\
    };\
    type_ var;\
    template<int _>\
    struct _reflect_Member<_reflect_index_of_##var<_reflect_Member>::value,_>\
    {\
        typedef int _reflect_int;\
        typedef type_ type;\
        static reflect::MemberInfo info()\
        {\
           return reflect::MemberInfo(#var);\
        }\
        template<typename T>\
        static type_& get(T& t)\
        {\
            return t.var;\
        }\
    }

namespace reflect
{

    struct MemberInfo
    {
        const char* name;
        MemberInfo(const char* name_)
        :name(name_)
        {}
    };

    template<int i, typename T>
    typename T::template _reflect_Member<i,0>::type& get_member(T& t)
    {
        return T::template _reflect_Member<i,0>::get(t);
    }

    namespace impl
    {
        template<template<int,int> class T, int i>
        static char valid(typename T<i,0>::_reflect_int);

        template<template<int,int> class T, int i>
        static long valid(...);

        template<typename T, int i=0, int valid = _reflect_VALID(T::template _reflect_Member,i)>
        struct ForEachMember
        {
            ForEachMember<T, i+1> inner;

            template<typename F>
            bool operator()(T& t, F& f)
            {
                bool good = f(get_member<i>(t), T::template _reflect_Member<i,0>::info());
                return good ? inner(t,f) : false;
            }
        };

        template<typename T, int i>
        struct ForEachMember<T, i, false>
        {
            template<typename F>
            bool operator()(T& t, F& f)
            {
                return true;
            }
        };
    } // namespace impl

    template<typename T, typename F>
    bool for_each_member(T& t, F& f)
    {
        return impl::ForEachMember<T>()(t,f);
    }

    struct Printer {
        template<class T>
        bool operator()(T& t, reflect::MemberInfo i)
        {
            return std::cout << i.name << ": " << t << std::endl;
        }
    };

    struct Reader {
        template<class T>
        bool operator()(T& t, reflect::MemberInfo i)
        {
            std::cout << i.name << ": ";
            return std::cin >> t;
        }
    };

    namespace{
        Printer print;
        Reader read;
    }


} // namespace reflect

#endif // REFLECT_H
