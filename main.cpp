#include "reflect.h"
#include <vector>
#include <map>
#include <set>
#include <sstream>

struct HexaAnnotation {int a, b;};

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
    MEMBER_EX(double, a, ANNOTATION(HexaAnnotation, ={10,20}));
    MEMBER(double, b);
    MEMBER(double, c);
};

struct NewPrinter {
    template<class MemberInfo, class MemberType>
    bool process(const MemberType& value)
    {
        if(MemberInfo::template Annotation<HexaAnnotation>::has)
        {
            std::cout << std::hex;
            HexaAnnotation a = MemberInfo::template Annotation<HexaAnnotation>::get();
            std::cout << "a.a: " << a.a <<std::endl;
        }
        else
        {
            std::cout << std::dec;
        }
        return std::cout << MemberInfo::name() << ": " << value << std::endl;
    }
};

struct B
{
    typedef std::map<int,int> mii;
    MEMBER(int,x);
    MEMBER(int,y);
    MEMBER(A,z);
    MEMBER(std::vector<A>, v);
    MEMBER(std::set<int>, s);
    MEMBER(mii, m);
    std::string ss;
};

struct C
{
    int a;
    struct TT;
    struct TT;
};

/* causes compiler error
template<typename P>
struct const_pair_reflector{
    MEMBER(typename P::first_type&,first);
    MEMBER(typename P::second_type&,second);
    const_pair_reflector(P& p)
    : first(p.first)
    , second(p.second)
    {}
};*/

std::string jsonQuote( const char *value )
{
   if (value == NULL)
      return "";
   // Not sure how to handle unicode...
   if (std::strpbrk(value, "\"\\\b\f\n\r\t") == NULL && !containsControlCharacter( value ))
      return std::string("\"") + value + "\"";
   // We have to walk value and escape any special characters.
   // Appending to std::string is not efficient, but this should be rare.
   // (Note: forward slashes are *not* rare, but I am not escaping them.)
   std::string::size_type maxsize = strlen(value)*2 + 3; // allescaped+quotes+NULL
   std::string result;
   result.reserve(maxsize); // to avoid lots of mallocs
   result += "\"";
   for (const char* c=value; *c != 0; ++c)
   {
      switch(*c)
      {
         case '\"':
            result += "\\\"";
            break;
         case '\\':
            result += "\\\\";
            break;
         case '\b':
            result += "\\b";
            break;
         case '\f':
            result += "\\f";
            break;
         case '\n':
            result += "\\n";
            break;
         case '\r':
            result += "\\r";
            break;
         case '\t':
            result += "\\t";
            break;
         //case '/':
            // Even though \/ is considered a legal escape in JSON, a bare
            // slash is also legal, so I see no reason to escape it.
            // (I hope I am not misunderstanding something.
            // blep notes: actually escaping \/ may be useful in javascript to avoid </
            // sequence.
            // Should add a flag to allow this compatibility mode and prevent this
            // sequence from occurring.
         default:
            if ( isControlCharacter( *c ) )
            {
               std::ostringstream oss;
               oss << "\\u" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << static_cast<int>(*c);
               result += oss.str();
            }
            else
            {
               result += *c;
            }
            break;
      }
   }
   result += "\"";
   return result;
}

struct RecPrinter {
    int indentLevel;
    std::string indent;
    RecPrinter(int indentLevel_ = 0)
    :indentLevel(indentLevel_)
    ,indent(indentLevel_*2,' ')
    {};

    template<class MemberInfo, class MemberType>
    bool process(const MemberType& value)
    {
        return (std::cout << indent << MemberInfo::name() << ": " )
                && processValue(value);
    }

    template<class MemberInfo, class Container>
    bool processContainer(const Container& cont)
    {
        RecPrinter p(indentLevel+1);
        if(!(std::cout << indent << MemberInfo::name() << ": [" << std::endl))
            return false;
        for(typename Container::const_iterator it = cont.begin(); it!=cont.end(); ++it)
        {
            if(!p.processValue(*it, false))
                return false;
        }
        if(!(std::cout <<  indent << "], "<< std::endl))
            return false;
        return true;
    }

    template<class MemberInfo, class T>
    bool process(const std::vector<T>& value)
    {
        return processContainer<MemberInfo>(value);
    }

    template<class MemberInfo, class T>
    bool process(const std::set<T>& value)
    {
        return processContainer<MemberInfo>(value);
    }

    template<class MemberInfo, class K, class V>
    bool process(const std::map<K,V>& value)
    {
        return processContainer<MemberInfo>(value);
    }

    template<class T1, class T2>
    bool processValue(const std::pair<T1,T2>& value, bool continueLastLine=true)
    {
        //const_pair_reflector<const std::pair<T1,T2> > pr(value);
        RecPrinter p(indentLevel+1);
        return (std::cout << (continueLastLine?"":indent) << "{"  << std::endl)
            && (std::cout << indent << "  first: ") && p.processValue(value.first)
            && (std::cout << indent << "  second: ") && p.processValue(value.second)
            && (std::cout <<  indent << "}, " << std::endl);
    }

    template<class MemberType>
    bool processValue(const std::string& value, bool continueLastLine = true)
    {
        return std::cout << (continueLastLine?"":indent) << '"' << jsonQuote(value.c_str()) << '"' << ","  << std::endl;
    }

    template<class MemberType>
    if_not_struct_bool processValue(const MemberType& value, bool continueLastLine = true)
    {
        return std::cout << (continueLastLine?"":indent) << value << ","  << std::endl;
    }

    template<class MemberType>
    if_struct_bool processValue(const MemberType& value, bool continueLastLine = true)
    {
        RecPrinter p(indentLevel+1);
        return (std::cout << (continueLastLine?"":indent) << "{"  << std::endl)
            && (reflect::for_each_member(value, p))
            && (std::cout <<  indent << "}, " << std::endl);
    }
};

int main()
{
    B bb;
    A a1 = {1,2,3};
    A a2 = {10,10,10};
    bb.v.push_back(a1);
    bb.v.push_back(a2);
    bb.s.insert(30);
    bb.m[5] = 6;
    bb.m[6] = 8;
    bb.ss = "Hi\n\r\\lol";
    RecPrinter rec_print;
    reflect::for_each_member(bb, rec_print);
    return 0;
    A a3;

    NewPrinter new_print;
    reflect::for_each_member(a2, new_print);

    Interpolate inter(0.2);
    reflect::for_each_member(a3, a1, a2, inter);
    reflect::Printer print;
    reflect::for_each_member(a3, print);
    C cc;
   // reflect::for_each_member(cc, print);
    S a, b;

    reflect::Reader read;
    EqMul2 eqMul2;
    reflect::Printer2 print2;

    reflect::for_each_member(a, read);
    reflect::for_each_member(b, (const S&)a, eqMul2);

    std::cout << std::endl;
    reflect::for_each_member<S::reflect_member_id_year, S::reflect_member_id_second>(a, b, print2);
}
