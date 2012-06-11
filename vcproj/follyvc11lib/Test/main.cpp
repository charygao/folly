#include <iostream>
#include "folly\Format.h"
#include <utility>
#include <string>
#include <cassert>
#include <boost/preprocessor.hpp>
#include <folly\FBString.h>
#include <folly\FBVector.h>
#include <folly\dynamic.h>
#include <folly/json.h>

#pragma comment(lib,"..\\Debug\\follyvc11lib.lib");
#define JRB_FORWARD(z,n,data) std::forward<BOOST_PP_CAT(Arg,n)>(BOOST_PP_CAT(arg,n))

template<class A, class B>
void EXPECT_EQ(const A& a, const B& b){assert(a==b);}
using namespace folly;
//template <class... Args>
//std::string fstr(StringPiece fmt, Args&&... args) {
//  return format(fmt, std::forward<Args>(args)...).str();
//}
#define BOOST_PP_LOCAL_MACRO(N) \
template <BOOST_PP_ENUM_PARAMS(N, class Arg)> \
std::string fstr(StringPiece fmt BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N,Arg,&& arg)) { \
  return format(fmt BOOST_PP_ENUM_TRAILING(N,JRB_FORWARD,unused)).str(); \
}
   /**/

#define BOOST_PP_LOCAL_LIMITS (1, 10)

#include BOOST_PP_LOCAL_ITERATE()

#define BOOST_PP_LOCAL_MACRO(N) \
template <BOOST_PP_ENUM_PARAMS(N, class Arg)> \
std::string vstr(StringPiece fmt BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N,Arg,&& arg)) { \
  return vformat(fmt BOOST_PP_ENUM_TRAILING(N,JRB_FORWARD,unused)).str(); \
}
   /**/

#define BOOST_PP_LOCAL_LIMITS (1, 10)

#include BOOST_PP_LOCAL_ITERATE()

int main(){

	std:: string sj = folly::format("Hello {0}",5).str();;
	  //EXPECT_EQ("hello", fstr("hello"));
  EXPECT_EQ("42", fstr("{}", 42));
  EXPECT_EQ("42 42", fstr("{0} {0}", 42));
  EXPECT_EQ("00042  23   42", fstr("{0:05} {1:3} {0:4}", 42, 23));
  EXPECT_EQ("hello world hello 42",
            fstr("{0} {1} {0} {2}", "hello", "world", 42));
  EXPECT_EQ("XXhelloXX", fstr("{:X^9}", "hello"));
  EXPECT_EQ("XXX42XXXX", fstr("{:X^9}", 42));
  EXPECT_EQ("-0xYYYY2a", fstr("{:Y=#9x}", -42));
  EXPECT_EQ("*", fstr("{}", '*'));
  EXPECT_EQ("42", fstr("{}", 42));
  EXPECT_EQ("0042", fstr("{:04}", 42));

  EXPECT_EQ("hello  ", fstr("{:7}", "hello"));
  EXPECT_EQ("hello  ", fstr("{:<7}", "hello"));
  EXPECT_EQ("  hello", fstr("{:>7}", "hello"));

  fbvector<int> v1 ;//
  v1.push_back(10);
  v1.push_back(20);
  v1.push_back(30);
  EXPECT_EQ("0020", fstr("{0[1]:04}", v1));
  EXPECT_EQ("0020", fstr(fstr("{{0[{0}]:04}}",1), v1));
  EXPECT_EQ("0020", vstr("{1:04}", v1));
  EXPECT_EQ("10 20", vstr("{} {}", v1));

  const std::vector<int> v2(v1.begin(),v1.end());
  EXPECT_EQ("0020", fstr("{0[1]:04}", v2));
  EXPECT_EQ("0020", vstr("{1:04}", v2));

  const int p[] = {10, 20, 30};
  const int* q = p;
  EXPECT_EQ("0020", fstr("{0[1]:04}", p));
  EXPECT_EQ("0020", vstr("{1:04}", p));
  EXPECT_EQ("0020", fstr("{0[1]:04}", q));
  EXPECT_EQ("0020", vstr("{1:04}", q));

  EXPECT_EQ("0x", fstr("{}", p).substr(0, 2));
  EXPECT_EQ("10", vstr("{}", p));
  EXPECT_EQ("0x", fstr("{}", q).substr(0, 2));
  EXPECT_EQ("10", vstr("{}", q));
  q = nullptr;
  EXPECT_EQ("(null)", fstr("{}", q));

  std::map<int, std::string> m;
  m[10]= "hello";
  m[20] =  "world";
  EXPECT_EQ("worldXX", fstr("{[20]:X<7}", m));
  EXPECT_EQ("worldXX", vstr("{20:X<7}", m));

  std::map<std::string, std::string> m2;
    m2["hello"]= "world";
  EXPECT_EQ("worldXX", fstr("{[hello]:X<7}", m2));
  EXPECT_EQ("worldXX", vstr("{hello:X<7}", m2));

  // Test indexing in strings
  EXPECT_EQ("61 62", fstr("{0[0]:x} {0[1]:x}", "abcde"));
  EXPECT_EQ("61 62", vstr("{0:x} {1:x}", "abcde"));
  EXPECT_EQ("61 62", fstr("{0[0]:x} {0[1]:x}", std::string("abcde")));
  EXPECT_EQ("61 62", vstr("{0:x} {1:x}", std::string("abcde")));

  // Test booleans
  EXPECT_EQ("true", fstr("{}", true));
  EXPECT_EQ("1", fstr("{:d}", true));
  EXPECT_EQ("false", fstr("{}", false));
  EXPECT_EQ("0", fstr("{:d}", false));

  // Test pairs
  {
    std::pair<int, std::string> p(42, "hello");
    EXPECT_EQ("    42 hello ", fstr("{0[0]:6} {0[1]:6}", p));
    EXPECT_EQ("    42 hello ", vstr("{:6} {:6}", p));
  }

  // Test tuples
  {
    std::tuple<int, std::string, int> t  = std::make_tuple( 42, "hello", 23 );
    EXPECT_EQ("    42 hello      23", fstr("{0[0]:6} {0[1]:6} {0[2]:6}", t));
    EXPECT_EQ("    42 hello      23", vstr("{:6} {:6} {:6}", t));
  }

  // Test writing to stream
  std::ostringstream os;
  os << format("{} {}", 42, 23);
  EXPECT_EQ("42 23", os.str());

  // Test appending to string
  fbstring s;
  format(&s, "{} {}", 42, 23);
  format(&s, " hello {:X<7}", "world");
  EXPECT_EQ("42 23 hello worldXX", s.toStdString());

  double d = 1;
  EXPECT_EQ("1", fstr("{}", 1.0));
  EXPECT_EQ("0.1", fstr("{}", 0.1));
  EXPECT_EQ("0.01", fstr("{}", 0.01));
  EXPECT_EQ("0.001", fstr("{}", 0.001));
  EXPECT_EQ("0.0001", fstr("{}", 0.0001));
  EXPECT_EQ("1e-5", fstr("{}", 0.00001));
  EXPECT_EQ("1e-6", fstr("{}", 0.000001));

  EXPECT_EQ("10", fstr("{}", 10.0));
  EXPECT_EQ("100", fstr("{}", 100.0));
  EXPECT_EQ("1000", fstr("{}", 1000.0));
  EXPECT_EQ("10000", fstr("{}", 10000.0));
  EXPECT_EQ("100000", fstr("{}", 100000.0));
  EXPECT_EQ("1e+6", fstr("{}", 1000000.0));
  EXPECT_EQ("1e+7", fstr("{}", 10000000.0));

  EXPECT_EQ("1.00", fstr("{:.2f}", 1.0));
  EXPECT_EQ("0.10", fstr("{:.2f}", 0.1));
  EXPECT_EQ("0.01", fstr("{:.2f}", 0.01));
  EXPECT_EQ("0.00", fstr("{:.2f}", 0.001));

  fbstring sfb= fstr("{:.2f}", 1.0);
  sfb = "string";
   dynamic twelve = 12;
  dynamic str = "string";
  str = sfb;
 dynamic map = dynamic::object;
  map[str] = twelve;
   map[str + "another_str"] = twelve;
  map.insert("null_element", nullptr);
   ++map[str];
   assert(map[str] == 13);
	 

     dynamic something = folly::parseJson(
    "{\"old_value\":40,\"changed\":true,\"opened\":false}");

	 std::cout << something["old_value"] + 50;
	 int i;
	 std::cin >> i;

}


