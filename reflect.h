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
        static const char* name()\
        {\
           return #var;\
        }\
        template<typename T>\
        static const type_& get(const T& t)\
        {\
            return t.var;\
        }\
        template<typename T>\
        static type_& get(T& t)\
        {\
            return t.var;\
        }\
    };\
    enum {reflect_member_id_##var = _reflect_index_of_##var<_reflect_Member>::value}

namespace reflect
{
    template<int i, typename T>
    const typename  T::template _reflect_Member<i,0>::type& get_member(const T& t)
    {
        return T::template _reflect_Member<i,0>::get(t);
    }

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

        template<typename T, int from, int to, bool valid = from <= to>
        struct ForMemberDescriptors
        {
            ForMemberDescriptors<T, from+1, to> inner;

            template<typename F>
            bool operator()(F& f)
            {
                bool good = f.template processMember<typename T::template _reflect_Member<from,0> >();
                return good ? inner(f) : false;
            }
        };

        template<typename T, int from, int to>
        struct ForMemberDescriptors<T, from, to, false>
        {
            template<typename F>
            bool operator()(F& f)
            {
                return true;
            }
        };

    } // namespace impl

    template<class T, template<int,int> class Member =  T::template _reflect_Member>
    struct last_member_id
    {
        enum { AssertComplete = sizeof(T) };
        enum
        {
            i128=_reflect_VALID(Member,127)*128,
            i64=i128+_reflect_VALID(Member,i128+63)*64,
            i32=i64+_reflect_VALID(Member,i64+31)*32,
            i16=i32+_reflect_VALID(Member,i32+15)*16,
            i8=i16+_reflect_VALID(Member,i16+7)*8,
            i4=i8+_reflect_VALID(Member,i8+3)*4,
            i2=i4+_reflect_VALID(Member,i4+1)*2,
            value=i2+_reflect_VALID(Member,i2)-1
        };
    };

    template<typename T, int from, int to, typename F>
    bool for_each_member_descriptor(F& f)
    {
        return impl::ForMemberDescriptors<T,from,to>()(f);
    }

    template<typename T, int from, typename F>
    bool for_each_member_descriptor(F& f)
    {
        return for_each_member_descriptor<T,from,last_member_id<T>::value,F>(f);
    }

    template<typename T, typename F>
    bool for_each_member_descriptor(F& f)
    {
        return for_each_member_descriptor<T,0,last_member_id<T>::value,F>(f);
    }

    template<typename T, typename Processor>
    struct ForwardMembersOf
    {
        T& a;
        Processor& p;

        ForwardMembersOf(T& a_, Processor& p_)
        :a(a_)
        ,p(p_)
        {}

        template<typename Member>
        bool processMember()
        {
            return p.template process<Member>(Member::get(a));
        }
    };

    template<typename T1, typename T2, typename Processor>
    struct ForwardMembersOf2
    {
        T1& a;
        T2& b;
        Processor& p;

        ForwardMembersOf2(T1& a_, T2& b_, Processor& p_)
        :a(a_)
        ,b(b_)
        ,p(p_)
        {}

        template<typename Member>
        bool processMember()
        {
            return p.template process<Member>(Member::get(a),Member::get(b));
        }
    };

    template<typename T1, typename T2, typename T3, typename Processor>
    struct ForwardMembersOf3
    {
        T1& a;
        T2& b;
        T3& c;
        Processor& p;

        ForwardMembersOf3(T1& a_, T2& b_, T3& c_, Processor& p_)
        :a(a_)
        ,b(b_)
        ,c(c_)
        ,p(p_)
        {}

        template<typename Member>
        bool processMember()
        {
            return p.template process<Member>(Member::get(a),Member::get(b),Member::get(c));
        }
    };

    //1 object

    template<int from, int to, typename T, typename F>
    bool for_each_member(T& obj, F& fun)
    {
        ForwardMembersOf<T, F> fwd(obj, fun);
        return for_each_member_descriptor<T,from,to>(fwd);
    }

    template<int from, typename T, typename F>
    bool for_each_member(T& obj, F& fun)
    {
        return for_each_member<from, last_member_id<T>::value, T, F>(obj,fun);
    }

    template<typename T, typename F>
    bool for_each_member(T& obj, F& fun)
    {
        return for_each_member<0, last_member_id<T>::value, T, F>(obj,fun);
    }

    // 2 objects

    template<int from, int to, typename T1, typename T2, typename F>
    bool for_each_member(T1& obj1, T2& obj2, F& fun)
    {
        ForwardMembersOf2<T1, T2, F> fwd(obj1, obj2, fun);
        return for_each_member_descriptor<T1,from,to>(fwd);
    }

    template<int from, typename T1, typename T2, typename F>
    bool for_each_member(T1& obj1, T2& obj2, F& fun)
    {
        return for_each_member<from, last_member_id<T1>::value, T1, T2, F>(obj1, obj2, fun);
    }

    template<typename T1, typename T2, typename F>
    bool for_each_member(T1& obj1, T2& obj2, F& fun)
    {
        return for_each_member<0, last_member_id<T1>::value, T1, T2, F>(obj1, obj2, fun);
    }

    // 3 objects

    template<int from, int to, typename T1, typename T2, typename T3, typename F>
    bool for_each_member(T1& obj1, T2& obj2, T3& obj3, F& fun)
    {
        ForwardMembersOf3<T1, T2, T3, F> fwd(obj1, obj2, obj3, fun);
        return for_each_member_descriptor<T1,from,to>(fwd);
    }

    template<int from, typename T1, typename T2, typename T3, typename F>
    bool for_each_member(T1& obj1, T2& obj2, T3& obj3, F& fun)
    {
        return for_each_member<from, last_member_id<T1>::value, T1, T2, T3, F>(obj1, obj2, obj3, fun);
    }

    template<typename T1, typename T2, typename T3, typename F>
    bool for_each_member(T1& obj1, T2& obj2, T3& obj3, F& fun)
    {
        return for_each_member<0, last_member_id<T1>::value, T1, T2, T3, F>(obj1, obj2, obj3, fun);
    }

    struct Printer {
        template<class MemberInfo, class MemberType>
        bool process(const MemberType& value)
        {
            return std::cout << MemberInfo::name() << ": " << value << std::endl;
        }
    };

    struct Printer2
    {
        template<class MemberInfo, class MemberType>
        bool process(const MemberType& value1, const MemberType& value2)
        {
            return std::cout << MemberInfo::name()  << ": " <<  value1 << ", " << value2 << std::endl;
        }
    };

    struct Printer3
    {
        template<class MemberInfo, class MemberType>
        bool process(const MemberType& value1, const MemberType& value2, const MemberType& value3)
        {
            return std::cout << MemberInfo::name()  << ": " <<  value1 << ", " << value2 << ", " << value3 <<std::endl;
        }
    };

    struct Reader {
        template<class MemberInfo, class MemberType>
        bool process(MemberType& value)
        {
            std::cout << MemberInfo::name() << ": ";
            return std::cin >> value;
        }
    };

} // namespace reflect

#endif // REFLECT_H
