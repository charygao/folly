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
#include "folly/Random.h"
#include <boost/test/minimal.hpp>
#include <boost/random.hpp>
#include <boost/next_prior.hpp>
#include <array>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <list>
#include <folly\small_vector.h>



#ifdef _DEBUG
#pragma comment(lib,"..\\Debug\\follyvc11lib.lib")
#else
#pragma comment(lib,"..\\Release\\follyvc11lib.lib")
#endif
#define JRB_FORWARD(z,n,data) std::forward<BOOST_PP_CAT(Arg,n)>(BOOST_PP_CAT(arg,n))

using namespace folly;
auto static const seed = randomNumberSeed();
typedef boost::mt19937 RandomT;
static RandomT rng(seed);
static const size_t maxString = 100;
static const bool avoidAliasing = true;

template <class Integral1, class Integral2>
Integral2 random(Integral1 low, Integral2 up) {
  boost::uniform_int<> range(low, up);
  return range(rng);
}

template <class String>
void randomString(String* toFill, unsigned int maxSize = 1000) {
  assert(toFill);
  toFill->resize(random(0, maxSize));
  FOR_EACH (i, *toFill) {
    *i = random('a', 'z');
  }
}

template <class String, class Integral>
void Num2String(String& str, Integral n) {
  str.resize(10, '\0');
//    ultoa((unsigned long)n, &str[0], 10);
  sprintf(&str[0], "%ul", 10);
  str.resize(strlen(str.c_str()));
}

std::list<char> RandomList(unsigned int maxSize) {
  std::list<char> lst(random(0u, maxSize));
  std::list<char>::iterator i = lst.begin();
  for (; i != lst.end(); ++i) {
    *i = random('a', 'z');
  }
  return lst;
}

template<class T> T randomObject();

template<> int randomObject<int>() {
  return random(0, 1024);
}

template<> folly::fbstring randomObject<folly::fbstring>() {
  folly::fbstring result;
  randomString(&result);
  return result;
}

//template <class... Args>
//std::string fstr(StringPiece fmt, Args&&... args) {
//  return format(fmt, std::forward<Args>(args)...).str();
//}
std::string fstr(StringPiece fmt) { 
  return format(fmt).str(); 
}
//
//std::string vstr(StringPiece fmt) { 
//  return fmt.toString(); 
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

class UserDefinedType { int whatevs_; };


FOLLY_ASSUME_FBVECTOR_COMPATIBLE(UserDefinedType);


void test_fbvector(){
		std::cout << "Testing fbvector ...\n";

	{
		fbvector<int> v(10, 20);
		BOOST_CHECK(v.size()== 10);
		FOR_EACH (i, v) {
			BOOST_CHECK(*i== 20);
		}
	}

	{
		fbvector<int> v;
		v.assign(10, 20);
		BOOST_CHECK(v.size()== 10);
		FOR_EACH (i, v) {
			BOOST_CHECK(*i== 20);

		}
	}
		{
			fbvector<int> v;
			auto const n = random(0U, 10000U); 
			v.reserve(n);
			auto const n1 = random(0U, 10000U);
			auto const obj = randomObject<int>();
			v.assign(n1, obj);
			v.shrink_to_fit();
			// Nothing to verify except that the call made it through
		}

		{
			fbvector<int> v;
			fbvector<int>::const_iterator i = v.end();
			v.insert(i, 10, 20);
			BOOST_CHECK(v.size()== 10);
			FOR_EACH (i, v) {
				BOOST_CHECK(*i== 20);
			}
		}


  fbvector< fbvector<double> > matrix(100, fbvector<double>(100));


 {
  fbvector<std::string> v(10, "hello");
  BOOST_CHECK(v.size()== 10);
  v.push_back("world");
}




 {
  fbvector<UserDefinedType> v(10);
  BOOST_CHECK(v.size()== 10);
  v.push_back(UserDefinedType());
}

 {
  fbvector<int> v1(100, 100);
  fbvector<int> v2;
  BOOST_CHECK(v1.size()== 100);
  BOOST_CHECK(v1.front()== 100);
  BOOST_CHECK(v2.size()== 0);
  v2 = std::move(v1);
  BOOST_CHECK(v1.size()== 0);
  BOOST_CHECK(v2.size()== 100);
  BOOST_CHECK(v2.front()== 100);

  v1.assign(100, 100);
  auto other = std::move(v1);
  BOOST_CHECK(v1.size()== 0);
  BOOST_CHECK(other.size()== 100);
  BOOST_CHECK(other.front()== 100);
}

 {
  fbvector<std::string> s(12,"asd");
  BOOST_CHECK(s.size()== 12);
  BOOST_CHECK(s.front()== "asd");
  s.emplace_back("funk");
  BOOST_CHECK(s.back()== "funk");
}

//TEST(fbvector, initializer_lists) {
//  fbvector<int> vec = { 1, 2, 3 };
//  BOOST_CHECK(vec.size(), 3);
//  BOOST_CHECK(vec[0], 1);
//  BOOST_CHECK(vec[1], 2);
//  BOOST_CHECK(vec[2], 3);
//
//  vec = { 0, 0, 12, 16 };
//  BOOST_CHECK(vec.size(), 4);
//  BOOST_CHECK(vec[0], 0);
//  BOOST_CHECK(vec[1], 0);
//  BOOST_CHECK(vec[2], 12);
//  BOOST_CHECK(vec[3], 16);
//
//  vec.insert(vec.begin() + 1, { 23, 23 });
//  BOOST_CHECK(vec.size(), 6);
//  BOOST_CHECK(vec[0], 0);
//  BOOST_CHECK(vec[1], 23);
//  BOOST_CHECK(vec[2], 23);
//  BOOST_CHECK(vec[3], 0);
//  BOOST_CHECK(vec[4], 12);
//  BOOST_CHECK(vec[5], 16);
//}

 {
  fbvector<std::unique_ptr<int> > v(12);
  std::unique_ptr<int> p(new int(12));
  v.push_back(std::move(p));
  BOOST_CHECK(*v.back()== 12);

  v[0] = std::move(p);
  BOOST_CHECK(v[0].get()==false);
  v[0].reset(new int(32));
  std::unique_ptr<int> somePtr;
  v.insert(v.begin(), std::move(somePtr));
  BOOST_CHECK(*v[1]== 32);
}

 {
  fbvector<fbstring> cycle;
  cycle.push_back("foo");
  cycle.push_back("bar");
  cycle.push_back("baz");
  fbstring message("Cycle detected: ");
  FOR_EACH_R (node_name, cycle) {
    message += "[";
    message += *node_name;
    message += "] ";
  }
  BOOST_CHECK("Cycle detected: [baz] [bar] [foo] "== message);
}
 	std::cout << "... Finished testing fbvector \n";

}

#define EXPECT_ANY_THROW(e)try{e; BOOST_CHECK(false);}catch(...){}
#define EXPECT_THROW(e,ex)try{e; BOOST_CHECK(false);}catch(ex&){}catch(...){BOOST_CHECK(false);}


void test_dynamic(){
		std::cout << "Testing dynamic ...\n";

	using folly::dynamic;

 {
  dynamic obj = dynamic::object("a", false);
  BOOST_CHECK(obj.at("a")== false);
  BOOST_CHECK(obj.size()== 1);
  obj.insert("a", true);
  BOOST_CHECK(obj.size()== 1);
  BOOST_CHECK(obj.at("a")== true);
  obj.at("a") = nullptr;
  BOOST_CHECK(obj.size()== 1);
  BOOST_CHECK(obj.at("a") == nullptr);

  dynamic newObject = dynamic::object;

  newObject["z"] = 12;
  BOOST_CHECK(newObject.size()== 1);
  newObject["a"] = true;
  BOOST_CHECK(newObject.size()== 2);

  BOOST_CHECK(*newObject.keys().begin()== newObject.items().begin()->first);
  BOOST_CHECK(*newObject.values().begin()== newObject.items().begin()->second);
  std::vector<std::pair<folly::fbstring, dynamic>> found;
  found.push_back(std::make_pair(
     newObject.keys().begin()->asString(),
     *newObject.values().begin()));

  BOOST_CHECK(*boost::next(newObject.keys().begin())==
            boost::next(newObject.items().begin())->first);
  BOOST_CHECK(*boost::next(newObject.values().begin())==
            boost::next(newObject.items().begin())->second);
  found.push_back(std::make_pair(
      boost::next(newObject.keys().begin())->asString(),
      *boost::next(newObject.values().begin())));

  std::sort(found.begin(), found.end());

  BOOST_CHECK("a"== found[0].first);
  BOOST_CHECK(found[0].second.asBool());

  BOOST_CHECK("z"== found[1].first);
  BOOST_CHECK(12== found[1].second.asInt());

  dynamic obj2 = dynamic::object;
  BOOST_CHECK(obj2.isObject());

  dynamic d3 = nullptr;
  BOOST_CHECK(d3 == nullptr);
  d3 = dynamic::object;
  BOOST_CHECK(d3.isObject());
  d3["foo"]  = dynamic::array;
    d3["foo"].push_back(1);
  d3["foo"].push_back(2);
  d3["foo"].push_back(3);
  BOOST_CHECK(d3.count("foo")== 1);

  d3[123] = 321;
  BOOST_CHECK(d3.at(123)== 321);

  d3["123"] = 42;
  BOOST_CHECK(d3.at("123")== 42);
  BOOST_CHECK(d3.at(123)== 321);

  // We don't allow objects as keys in objects.
  EXPECT_ANY_THROW(newObject[d3] = 12);
}

{
  dynamic obj = dynamic::object("key1", "val")
                               ("key2", "val2");
  BOOST_CHECK(obj.count("key1")== 1);
  BOOST_CHECK(obj.count("key2")== 1);
  BOOST_CHECK(obj.erase("key1")== 1);
  BOOST_CHECK(obj.count("key1")== 0);
  BOOST_CHECK(obj.count("key2")== 1);
  BOOST_CHECK(obj.erase("key1")== 0);
  obj["key1"] = 12;
  BOOST_CHECK(obj.count("key1")== 1);
  BOOST_CHECK(obj.count("key2")== 1);
  auto it = obj.find("key2");
  obj.erase(it);
  BOOST_CHECK(obj.count("key1")== 1);
  BOOST_CHECK(obj.count("key2")== 0);

  obj["asd"] = 42.0;
  obj["foo"] = 42.0;
  BOOST_CHECK(obj.size()== 3);
  auto ret = obj.erase(boost::next(obj.items().begin()), obj.items().end());
  BOOST_CHECK(ret == obj.items().end());
  BOOST_CHECK(obj.size()== 1);
  obj.erase(obj.items().begin());
  BOOST_CHECK(obj.empty());
}

 {
  dynamic arr = dynamic::array;
  arr.push_back(1);
  arr.push_back(2);
  arr.push_back(3);
  arr.push_back(4);
  arr.push_back(5);
  arr.push_back(6);


  EXPECT_THROW(arr.erase(1), std::exception);
  BOOST_CHECK(arr.size()== 6);
  BOOST_CHECK(arr[0]== 1);
  arr.erase(arr.begin());
  BOOST_CHECK(arr.size()== 5);

  arr.erase(boost::next(arr.begin()), boost::prior(arr.end()));
  BOOST_CHECK(arr.size()== 2);
  BOOST_CHECK(arr[0]== 2);
  BOOST_CHECK(arr[1]== 6);
}

{
  dynamic str = "hello world";
  BOOST_CHECK(11== str.size());
  BOOST_CHECK(str.empty()==false);
  str = "";
  BOOST_CHECK(str.empty());
}

 {
  dynamic array = dynamic::array;
    array.push_back(1);
  array.push_back(2);
  array.push_back(3);
  //{ 1, 2, 3 };
  BOOST_CHECK(array.size()== 3);
  BOOST_CHECK(array.at(0)== 1);
  BOOST_CHECK(array.at(1)== 2);
  BOOST_CHECK(array.at(2)== 3);

  EXPECT_ANY_THROW(array.at(3));

  array.push_back("foo");
  BOOST_CHECK(array.size()== 4);

  array.resize(12, "something");
  BOOST_CHECK(array.size()== 12);
  BOOST_CHECK(array[11]== "something");
}

 {
  dynamic val = dynamic::array;
  val.push_back("foo");
  val.push_back("bar");
  std::array<dynamic,2>init = {"foo1", "bar1"};
  val.push_back(init);
  BOOST_CHECK(val.at(2).at(0)== "foo1");
  BOOST_CHECK(val.at(2).at(1)== "bar1");
  dynamic val2 = val;
  BOOST_CHECK(val2.at(2).at(0)== "foo1");
  BOOST_CHECK(val2.at(2).at(1)== "bar1");
  BOOST_CHECK(val.at(2).at(0)== "foo1");
  BOOST_CHECK(val.at(2).at(1)== "bar1");
  val2.at(2).at(0) = "foo3";
  val2.at(2).at(1) = "bar3";
  BOOST_CHECK(val.at(2).at(0)== "foo1");
  BOOST_CHECK(val.at(2).at(1)== "bar1");
  BOOST_CHECK(val2.at(2).at(0)== "foo3");
  BOOST_CHECK(val2.at(2).at(1)== "bar3");

  std::array<dynamic,3> init3 = {"d", "e", "f"};
  dynamic obj = dynamic::object("a", "b")
                               ("c", init3)
                               ;
  BOOST_CHECK(obj.at("a")== "b");
  dynamic obj2 = obj;
  std::array<dynamic,3> init4 =  {1, 2, 3};
  obj2.at("a") = init4;
  BOOST_CHECK(obj.at("a")== "b");
  dynamic expected = init4;
  BOOST_CHECK(obj2.at("a")== expected);
}

 {
  bool caught = false;
  try {
    dynamic d1 = dynamic::object;
    dynamic d2 = dynamic::object;
    auto foo = d1 < d2;
  } catch (std::exception const& e) {
    caught = true;
  }
  BOOST_CHECK(caught);

  dynamic foo = "asd";
  dynamic bar = "bar";
  dynamic sum = foo + bar;
  BOOST_CHECK(sum== "asdbar");

  dynamic some = 12;
  dynamic nums = 4;
  dynamic math = some / nums;
  BOOST_CHECK(math== 3);
}

 {
  dynamic str = "12.0";
  BOOST_CHECK(str.asDouble()== 12.0);
  EXPECT_ANY_THROW(str.asInt());
  EXPECT_ANY_THROW(str.asBool());

  str = "12";
  BOOST_CHECK(str.asInt()== 12);
  BOOST_CHECK(str.asDouble()== 12.0);
  str = "0";
  BOOST_CHECK(str.asBool()== false);
  BOOST_CHECK(str.asInt()== 0);
  BOOST_CHECK(str.asDouble()== 0);
  BOOST_CHECK(str.asString()== "0");
}

{
  std::ostringstream out;
  dynamic doubl = 123.33;
  dynamic dint = 12;
  out << "0x" << std::hex << ++dint << ' ' << std::setprecision(1)
      << doubl << '\n';
  BOOST_CHECK(out.str()== "0xd 1e+002\n"); // changed from original test of 0xd 1e+02\n

  out.str("");
  dynamic arrr = dynamic::array; 
  arrr.push_back(1);
  arrr.push_back(2);
  arrr.push_back(3);
  out << arrr;
  BOOST_CHECK(out.str()== "[1,2,3]");

  out.str("");
  dynamic objy = dynamic::object("a", 12);
  out << objy;
  BOOST_CHECK(out.str()== "{\"a\":12}");

  out.str("");
  std::array<dynamic,3> init = { objy, dynamic::object(12, "str"),
                          dynamic::object(true, false) };
  dynamic objy2 = init ;
  out << objy2;
  BOOST_CHECK(out.str()== "[{\"a\":12},{12:\"str\"},{true:false}]");
}

 {
  dynamic d1 = dynamic::object("foo", "bar");
  BOOST_CHECK(d1.getDefault("foo", "baz")== "bar");
  BOOST_CHECK(d1.getDefault("quux", "baz")== "baz");

  dynamic d2 = dynamic::object("foo", "bar");
  BOOST_CHECK(d2.setDefault("foo", "quux")== "bar");
  d2.setDefault("bar", dynamic(dynamic::array)).push_back(42);
  BOOST_CHECK(d2["bar"][0]== 42);

  dynamic d3 = dynamic::object, empty = dynamic::object;
  BOOST_CHECK(d3.getDefault("foo")== empty);
  d3.setDefault("foo")["bar"] = "baz";
  BOOST_CHECK(d3["foo"]["bar"]== "baz");

  // we do not allow getDefault/setDefault on arrays
  dynamic d4 = dynamic::array;
  EXPECT_ANY_THROW(d4.getDefault("foo", "bar"));
  EXPECT_ANY_THROW(d4.setDefault("foo", "bar"));
}

 	std::cout << "... Finished testing dynamic \n";

}



void test_json(){
		std::cout << "Testing json ...\n";

	using folly::dynamic;
using folly::parseJson;
using folly::toJson;

 {

 // VC++ has problems with unicode embedded characters
  //auto val = parseJson("\"I \u2665 UTF-8\"");
  //BOOST_CHECK("I \u2665 UTF-8"== val.asString());
  //val = parseJson("\"I \\u2665 UTF-8\"");
  //BOOST_CHECK("I \u2665 UTF-8"== val.asString());
  //val = parseJson("\"I \U0001D11E playing in G-clef\"");
  //BOOST_CHECK("I \U0001D11E playing in G-clef"== val.asString());

  //val = parseJson("\"I \\uD834\\uDD1E playing in G-clef\"");
  //BOOST_CHECK("I \U0001D11E playing in G-clef"== val.asString());
}

 {
  auto num = parseJson("12");
  BOOST_CHECK(num.isInt());
  BOOST_CHECK(num== 12);
  num = parseJson("12e5");
  BOOST_CHECK(num.isDouble());
  BOOST_CHECK(num== 12e5);
  auto numAs1 = num.asDouble();
  BOOST_CHECK(numAs1== 12e5);
  BOOST_CHECK(num== 12e5);
  BOOST_CHECK(num== 1200000);

  auto largeNumber = parseJson("4611686018427387904");
  BOOST_CHECK(largeNumber.isInt());
  BOOST_CHECK(largeNumber== 4611686018427387904L);

  auto negative = parseJson("-123");
  BOOST_CHECK(negative== -123);

  auto bfalse = parseJson("false");
  auto btrue = parseJson("true");
  BOOST_CHECK(bfalse== false);
  BOOST_CHECK(btrue== true);

  auto null = parseJson("null");
  BOOST_CHECK(null == nullptr);

  auto doub1 = parseJson("12.0");
  auto doub2 = parseJson("12e2");
  BOOST_CHECK(doub1== 12.0);
  BOOST_CHECK(doub2== 12e2);
  BOOST_CHECK(std::numeric_limits<double>::infinity()==
            parseJson("Infinity").asDouble());
  BOOST_CHECK(-std::numeric_limits<double>::infinity()==
            parseJson("-Infinity").asDouble());
  BOOST_CHECK(boost::math::isnan(parseJson("NaN").asDouble()));
  EXPECT_THROW(parseJson("infinity"), std::runtime_error);
  EXPECT_THROW(parseJson("inf"), std::runtime_error);
  EXPECT_THROW(parseJson("nan"), std::runtime_error);

  auto array = parseJson(
    "[12,false, false  , null , [12e4,32, [], 12]]");
  BOOST_CHECK(array.size()== 5);
  if (array.size() == 5) {
    BOOST_CHECK(boost::prior(array.end())->size()== 4);
  }

  bool caught = false;
  try {
    parseJson("\n[12,\n\nnotvalidjson");
  } catch (const std::exception& e) {
    caught = true;
  }
  BOOST_CHECK(caught);

  caught = false;
  try {
    parseJson("12e2e2");
  } catch (const std::exception& e) {
    caught = true;
  }
  BOOST_CHECK(caught);

  caught = false;
  try {
    parseJson("{\"foo\":12,\"bar\":42} \"something\"");
  } catch (const std::exception& e) {
    // incomplete parse
    caught = true;
  }
  BOOST_CHECK(caught);
  std::array<dynamic,1> init1 = { "heh" };
  std::array<dynamic,5> init2 =       {
        dynamic::object("a", "b")
                       ("c", "d"),
        12.5,
        "Yo Dawg",
        init1,
        nullptr
      };
  dynamic anotherVal = dynamic::object
    ("foo", "bar")
    ("junk", 12)
    ("another", 32.2)
    ("a",init2 )
    ;
  // Print then parse and get the same thing, hopefully.
  auto value = parseJson(toJson(anotherVal));
  BOOST_CHECK(value== anotherVal);

  // Test an object with non-string values.
  dynamic something = folly::parseJson(
    "{\"old_value\":40,\"changed\":true,\"opened\":false}");
  dynamic expected = dynamic::object
    ("old_value", 40)
    ("changed", true)
    ("opened", false);
  BOOST_CHECK(something== expected);
}

 {
  auto badDouble = (1ll << 63ll) + 1;
  dynamic badDyn = badDouble;
  BOOST_CHECK(folly::toJson(badDouble)== folly::to<folly::fbstring>(badDouble));
  folly::json::serialization_opts opts;
  opts.javascript_safe = true;
  //EXPECT_ANY_THROW(folly::json::serialize(badDouble, opts));

  auto okDouble = 1ll << 63ll;
  dynamic okDyn = okDouble;
  BOOST_CHECK(folly::toJson(okDouble)== folly::to<folly::fbstring>(okDouble));
}

{
  //auto value = parseJson(R"( "f\"oo" )");
  //BOOST_CHECK(toJson(value), R"("f\"oo")");
  //value = parseJson("\"Control code: \001 \002 \x1f\"");
  //BOOST_CHECK(toJson(value), R"("Control code: \u0001 \u0002 \u001f")");

  bool caught = false;
  try {
    dynamic d = dynamic::object;
    d["abc"] = "xyz";
    d[42.33] = "asd";
    auto str = toJson(d);
  } catch (std::exception const& e) {
    // We're not allowed to have non-string keys in json.
    caught = true;
  }
  BOOST_CHECK(caught);
}

 {
  folly::json::serialization_opts opts;
  opts.encode_non_ascii = true;

  //// simple tests
  //BOOST_CHECK(folly::json::serialize("\x1f", opts), R"("\u001f")");
  //BOOST_CHECK(folly::json::serialize("\xc2\xa2", opts), R"("\u00a2")");
  //BOOST_CHECK(folly::json::serialize("\xe2\x82\xac", opts), R"("\u20ac")");

  //// multiple unicode encodings
  //BOOST_CHECK(
  //  folly::json::serialize("\x1f\xe2\x82\xac", opts),
  //  R"("\u001f\u20ac")");
  //BOOST_CHECK(
  //  folly::json::serialize("\x1f\xc2\xa2\xe2\x82\xac", opts),
  //  R"("\u001f\u00a2\u20ac")");
  //BOOST_CHECK(
  //  folly::json::serialize("\xc2\x80\xef\xbf\xbf", opts),
  //  R"("\u0080\uffff")");
  //BOOST_CHECK(
  //  folly::json::serialize("\xe0\xa0\x80\xdf\xbf", opts),
  //  R"("\u0800\u07ff")");

  //// first possible sequence of a certain length
  //BOOST_CHECK(folly::json::serialize("\xc2\x80", opts), R"("\u0080")");
  //BOOST_CHECK(folly::json::serialize("\xe0\xa0\x80", opts), R"("\u0800")");

  //// last possible sequence of a certain length
  //BOOST_CHECK(folly::json::serialize("\xdf\xbf", opts), R"("\u07ff")");
  //BOOST_CHECK(folly::json::serialize("\xef\xbf\xbf", opts), R"("\uffff")");

  //// other boundary conditions
  //BOOST_CHECK(folly::json::serialize("\xed\x9f\xbf", opts), R"("\ud7ff")");
  //BOOST_CHECK(folly::json::serialize("\xee\x80\x80", opts), R"("\ue000")");
  //BOOST_CHECK(folly::json::serialize("\xef\xbf\xbd", opts), R"("\ufffd")");

  // incomplete sequences
  EXPECT_ANY_THROW(folly::json::serialize("a\xed\x9f", opts));
  EXPECT_ANY_THROW(folly::json::serialize("b\xee\x80", opts));
  EXPECT_ANY_THROW(folly::json::serialize("c\xef\xbf", opts));

  // impossible bytes
  EXPECT_ANY_THROW(folly::json::serialize("\xfe", opts));
  EXPECT_ANY_THROW(folly::json::serialize("\xff", opts));

  // Sample overlong sequences
  EXPECT_ANY_THROW(folly::json::serialize("\xc0\xaf", opts));
  EXPECT_ANY_THROW(folly::json::serialize("\xe0\x80\xaf", opts));

  // Maximum overlong sequences
  EXPECT_ANY_THROW(folly::json::serialize("\xc1\xbf", opts));
  EXPECT_ANY_THROW(folly::json::serialize("\x30\x9f\xbf", opts));

  // illegal code positions
  EXPECT_ANY_THROW(folly::json::serialize("\xed\xa0\x80", opts));
  EXPECT_ANY_THROW(folly::json::serialize("\xed\xbf\xbf", opts));

  // Overlong representation of NUL character
  EXPECT_ANY_THROW(folly::json::serialize("\xc0\x80", opts));
  EXPECT_ANY_THROW(folly::json::serialize("\xe0\x80\x80", opts));

  // Longer than 3 byte encodings
  EXPECT_ANY_THROW(folly::json::serialize("\xf4\x8f\xbf\xbf", opts));
  EXPECT_ANY_THROW(folly::json::serialize("\xed\xaf\xbf\xed\xbf\xbf", opts));
}

 {
  folly::json::serialization_opts opts;
  opts.validate_utf8 = true;

  //// valid utf8 strings
  //BOOST_CHECK(folly::json::serialize("a\xc2\x80z", opts), R"("a\u00c2\u0080z")");
  //BOOST_CHECK(
  //  folly::json::serialize("a\xe0\xa0\x80z", opts),
  //  R"("a\u00e0\u00a0\u0080z")");
  //BOOST_CHECK(
  //  folly::json::serialize("a\xe0\xa0\x80m\xc2\x80z", opts),
  //  R"("a\u00e0\u00a0\u0080m\u00c2\u0080z")");

  // test with invalid utf8
  EXPECT_ANY_THROW(folly::json::serialize("a\xe0\xa0\x80z\xc0\x80", opts));
  EXPECT_ANY_THROW(folly::json::serialize("a\xe0\xa0\x80z\xe0\x80\x80", opts));
}

 		std::cout << "... Finished testing json ...\n";

}

void test_format(){
		std::cout << "Testing format ...\n";

	  BOOST_CHECK("hello"== format("hello").str());
  BOOST_CHECK("42"== fstr("{}", 42));
  BOOST_CHECK("42 42"== fstr("{0} {0}", 42));
  BOOST_CHECK("00042  23   42"== fstr("{0:05} {1:3} {0:4}", 42, 23));
  BOOST_CHECK("hello world hello 42"==
            fstr("{0} {1} {0} {2}", "hello", "world", 42));
  BOOST_CHECK("XXhelloXX"== fstr("{:X^9}", "hello"));
  BOOST_CHECK("XXX42XXXX"== fstr("{:X^9}", 42));
  BOOST_CHECK("-0xYYYY2a"== fstr("{:Y=#9x}", -42));
  BOOST_CHECK("*"== fstr("{}", '*'));
  BOOST_CHECK("42"== fstr("{}", 42));
  BOOST_CHECK("0042"== fstr("{:04}", 42));

  BOOST_CHECK("hello  "== fstr("{:7}", "hello"));
  BOOST_CHECK("hello  "== fstr("{:<7}", "hello"));
  BOOST_CHECK("  hello"== fstr("{:>7}", "hello"));

  fbvector<int> v1 ;//
  v1.push_back(10);
  v1.push_back(20);
  v1.push_back(30);
  BOOST_CHECK("0020"== fstr("{0[1]:04}", v1));
  BOOST_CHECK("0020"== fstr(fstr("{{0[{0}]:04}}",1), v1));
  BOOST_CHECK("0020"== vstr("{1:04}", v1));
  BOOST_CHECK("10 20"== vstr("{} {}", v1));

  const std::vector<int> v2(v1.begin(),v1.end());
  BOOST_CHECK("0020"== fstr("{0[1]:04}", v2));
  BOOST_CHECK("0020"== vstr("{1:04}", v2));

  const int p[] = {10, 20, 30};
  const int* q = p;
  BOOST_CHECK("0020"== fstr("{0[1]:04}", p));
  BOOST_CHECK("0020"== vstr("{1:04}", p));
  BOOST_CHECK("0020"== fstr("{0[1]:04}", q));
  BOOST_CHECK("0020"== vstr("{1:04}", q));

  BOOST_CHECK("0x"== fstr("{}", p).substr(0, 2));
  BOOST_CHECK("10"== vstr("{}", p));
  BOOST_CHECK("0x"== fstr("{}", q).substr(0, 2));
  BOOST_CHECK("10"== vstr("{}", q));
  q = nullptr;
  BOOST_CHECK("(null)"== fstr("{}", q));

  std::map<int, std::string> m;
  m[10]= "hello";
  m[20] =  "world";
  BOOST_CHECK("worldXX"== fstr("{[20]:X<7}", m));
  BOOST_CHECK("worldXX"== vstr("{20:X<7}", m));

  std::map<std::string, std::string> m2;
    m2["hello"]= "world";
  BOOST_CHECK("worldXX"== fstr("{[hello]:X<7}", m2));
  BOOST_CHECK("worldXX"== vstr("{hello:X<7}", m2));

  // Test indexing in strings
  BOOST_CHECK("61 62"== fstr("{0[0]:x} {0[1]:x}", "abcde"));
  BOOST_CHECK("61 62"== vstr("{0:x} {1:x}", "abcde"));
  BOOST_CHECK("61 62"== fstr("{0[0]:x} {0[1]:x}", std::string("abcde")));
  BOOST_CHECK("61 62"== vstr("{0:x} {1:x}", std::string("abcde")));

  // Test booleans
  BOOST_CHECK("true"== fstr("{}", true));
  BOOST_CHECK("1"== fstr("{:d}", true));
  BOOST_CHECK("false"== fstr("{}", false));
  BOOST_CHECK("0"== fstr("{:d}", false));

  // Test pairs
  {
    std::pair<int, std::string> p(42, "hello");
    BOOST_CHECK("    42 hello "== fstr("{0[0]:6} {0[1]:6}", p));
    BOOST_CHECK("    42 hello "== vstr("{:6} {:6}", p));
  }

  // Test tuples
  {
    std::tuple<int, std::string, int> t  = std::make_tuple( 42, "hello", 23 );
    BOOST_CHECK("    42 hello      23"== fstr("{0[0]:6} {0[1]:6} {0[2]:6}", t));
    BOOST_CHECK("    42 hello      23"== vstr("{:6} {:6} {:6}", t));
  }

  // Test writing to stream
  std::ostringstream os;
  os << format("{} {}", 42, 23);
  BOOST_CHECK("42 23"== os.str());

  // Test appending to string
  fbstring s;
  format(&s, "{} {}", 42, 23);
  format(&s, " hello {:X<7}", "world");
  BOOST_CHECK("42 23 hello worldXX"== s.toStdString());

  double d = 1;
  BOOST_CHECK("1"== fstr("{}", 1.0));
  BOOST_CHECK("0.1"== fstr("{}", 0.1));
  BOOST_CHECK("0.01"== fstr("{}", 0.01));
  BOOST_CHECK("0.001"== fstr("{}", 0.001));
  BOOST_CHECK("0.0001"== fstr("{}", 0.0001));
  BOOST_CHECK("1e-5"== fstr("{}", 0.00001));
  BOOST_CHECK("1e-6"== fstr("{}", 0.000001));

  BOOST_CHECK("10"== fstr("{}", 10.0));
  BOOST_CHECK("100"== fstr("{}", 100.0));
  BOOST_CHECK("1000"== fstr("{}", 1000.0));
  BOOST_CHECK("10000"== fstr("{}", 10000.0));
  BOOST_CHECK("100000"== fstr("{}", 100000.0));
  BOOST_CHECK("1e+6"== fstr("{}", 1000000.0));
  BOOST_CHECK("1e+7"== fstr("{}", 10000000.0));

  BOOST_CHECK("1.00"== fstr("{:.2f}", 1.0));
  BOOST_CHECK("0.10"== fstr("{:.2f}", 0.1));
  BOOST_CHECK("0.01"== fstr("{:.2f}", 0.01));
  BOOST_CHECK("0.00"== fstr("{:.2f}", 0.001));
	std::cout << "... finished testing format \n";

}

// String tests

template <class String> void clause_21_3_1_a(String & test) {
  test.String::~String();
  new(&test) String();
}
template <class String> void clause_21_3_1_b(String & test) {
  // Copy constructor
  const size_t pos = random(0, test.size());
  String s(test, pos, random(0, (size_t)(test.size() - pos)));
  test = s;
}
template <class String> void clause_21_3_1_c(String & test) {
  // Constructor from char*, size_t
  const size_t
    pos = random(0, test.size()),
    n = random(0, test.size() - pos);
  std::string before(test.data(), test.size());
  String s(test.c_str() + pos, n);
  std::string after(test.data(), test.size());
  BOOST_CHECK(before== after);

  // Constructor from char*, char*
  String s1(test.begin(), test.end());
  BOOST_CHECK(test== s1);
  String s2(test.data(), test.data() + test.size());
  BOOST_CHECK(test== s2);

  // Constructor from iterators
  std::list<char> lst;
  for (auto c : test) lst.push_back(c);
  String s3(lst.begin(), lst.end());
  BOOST_CHECK(test==s3);

  // Constructor from wchar_t iterators
  std::list<wchar_t> list1;
  for (auto c : test) list1.push_back(c);
  String s4(list1.begin(), list1.end());
  BOOST_CHECK(test== s4);

  // Constructor from wchar_t pointers
  wchar_t t[20];
  t[0] = 'a';
  t[1] = 'b';
  String s5(t, t + 2);;
  BOOST_CHECK("ab"== s5);

  test = s;
}
template <class String> void clause_21_3_1_d(String & test) {
  // Assignment
  auto size = random(0, 2000);
  String s(size, '\0');
  BOOST_CHECK(s.size()== size);
  FOR_EACH_RANGE (i, 0, s.size()) {
    s[i] = random('a', 'z');
  }
  test = s;
}
template <class String> void clause_21_3_1_e(String & test) {
  // Assignment from char*
  String s(random(0, 1000), '\0');
  size_t i = 0;
  for (; i != s.size(); ++i) {
    s[i] = random('a', 'z');
  }
  test = s.c_str();
}
template <class String> void clause_21_3_1_f(String & test) {
  // Aliased assign
  const size_t pos = random(0, test.size());
  if (avoidAliasing) {
    test = String(test.c_str() + pos);
  } else {
    test = test.c_str() + pos;
  }
}
template <class String> void clause_21_3_1_g(String & test) {
  // Assignment from char
  test = random('a', 'z');
}

template <class String> void clause_21_3_2(String & test) {
  // Iterators. The code below should leave test unchanged
  BOOST_CHECK(test.size()== test.end() - test.begin());
  BOOST_CHECK(test.size()== test.rend() - test.rbegin());

  auto s = test.size();
  test.resize(test.end() - test.begin());
  BOOST_CHECK(s== test.size());
  test.resize(test.rend() - test.rbegin());
  BOOST_CHECK(s== test.size());
}

template <class String> void clause_21_3_3(String & test) {
  // exercise capacity, size, max_size
  BOOST_CHECK(test.size()== test.length());
  BOOST_CHECK(test.size()<= test.max_size());
  BOOST_CHECK(test.capacity()<= test.max_size());
  BOOST_CHECK(test.size()<= test.capacity());
  // exercise empty
  if (test.empty()) test = "empty";
  else test = "not empty";
}

template <class String> void clause_21_3_4(String & test) {
  // exercise element access 21.3.4
  if (!test.empty()) {
    auto const i = random(0, test.size() - 1);
    BOOST_CHECK(test[i]== test.at(i));
    test = test[i];
  }
}

template <class String> void clause_21_3_5_a(String & test) {
  // 21.3.5 modifiers (+=)
  String test1;
  randomString(&test1);
  assert(test1.size() == strlen(test1.c_str()));
  auto len = test.size();
  test += test1;
  BOOST_CHECK(test.size()== test1.size() + len);
  FOR_EACH_RANGE (i, 0, test1.size()) {
    BOOST_CHECK(test[len + i]== test1[i]);
  }
  // aliasing modifiers
  String test2 = test;
  auto dt = test2.data();
  auto sz = test.c_str();
  len = test.size();
  BOOST_CHECK(memcmp(sz, dt, len)== 0);
  String copy(test.data(), test.size());
  BOOST_CHECK(strlen(test.c_str())== len);
  test += test;
  //test.append(test);
  BOOST_CHECK(test.size()== 2 * len);
  BOOST_CHECK(strlen(test.c_str())== 2 * len);
  FOR_EACH_RANGE (i, 0, len) {
    BOOST_CHECK(test[i]== copy[i]);
    BOOST_CHECK(test[i]== test[len + i]);
  }
  len = test.size();
  BOOST_CHECK(strlen(test.c_str())== len);
  // more aliasing
  auto const pos = random(0, test.size());
  BOOST_CHECK(strlen(test.c_str() + pos)== len - pos);
  if (avoidAliasing) {
    String addMe(test.c_str() + pos);
    BOOST_CHECK(addMe.size()== len - pos);
    test += addMe;
  } else {
    test += test.c_str() + pos;
  }
  BOOST_CHECK(test.size()== 2 * len - pos);
  // single char
  len = test.size();
  test += random('a', 'z');
  BOOST_CHECK(test.size()== len + 1);
}

template <class String> void clause_21_3_5_b(String & test) {
  // 21.3.5 modifiers (append, push_back)
  String s;

  // Test with a small string first
  char c = random('a', 'z');
  s.push_back(c);
  BOOST_CHECK(s[s.size() - 1]== c);
  BOOST_CHECK(s.size()== 1);
  s.resize(s.size() - 1);

  randomString(&s, maxString);
  test.append(s);
  randomString(&s, maxString);
  test.append(s, random(0, s.size()), random(0, maxString));
  randomString(&s, maxString);
  test.append(s.c_str(), random(0, s.size()));
  randomString(&s, maxString);
  test.append(s.c_str());
  test.append(random(0, maxString), random('a', 'z'));
  std::list<char> lst(RandomList(maxString));
  test.append(lst.begin(), lst.end());
  c = random('a', 'z');
  test.push_back(c);
  BOOST_CHECK(test[test.size() - 1]== c);
}

template <class String> void clause_21_3_5_c(String & test) {
  // assign
  String s;
  randomString(&s);
  test.assign(s);
}

template <class String> void clause_21_3_5_d(String & test) {
  // assign
  String s;
  randomString(&s, maxString);
  test.assign(s, random(0, s.size()), random(0, maxString));
}

template <class String> void clause_21_3_5_e(String & test) {
  // assign
  String s;
  randomString(&s, maxString);
  test.assign(s.c_str(), random(0, s.size()));
}

template <class String> void clause_21_3_5_f(String & test) {
  // assign
  String s;
  randomString(&s, maxString);
  test.assign(s.c_str());
}

template <class String> void clause_21_3_5_g(String & test) {
  // assign
  String s;
  randomString(&s, maxString);
  test.assign(random(0, maxString), random('a', 'z'));
}

template <class String> void clause_21_3_5_h(String & test) {
  // assign from bidirectional iterator
  std::list<char> lst(RandomList(maxString));
  test.assign(lst.begin(), lst.end());
}

template <class String> void clause_21_3_5_i(String & test) {
  // assign from aliased source
  test.assign(test);
}

template <class String> void clause_21_3_5_j(String & test) {
  // assign from aliased source
  test.assign(test, random(0, test.size()), random(0, maxString));
}

template <class String> void clause_21_3_5_k(String & test) {
  // assign from aliased source
  test.assign(test.c_str(), random(0, test.size()));
}

template <class String> void clause_21_3_5_l(String & test) {
  // assign from aliased source
  test.assign(test.c_str());
}

template <class String> void clause_21_3_5_m(String & test) {
  // insert
  String s;
  randomString(&s, maxString);
  test.insert(random(0, test.size()), s);
  randomString(&s, maxString);
  test.insert(random(0, test.size()),
              s, random(0, s.size()),
              random(0, maxString));
  randomString(&s, maxString);
  test.insert(random(0, test.size()),
              s.c_str(), random(0, s.size()));
  randomString(&s, maxString);
  test.insert(random(0, test.size()), s.c_str());
  test.insert(random(0, test.size()),
              random(0, maxString), random('a', 'z'));
  test.insert(test.begin() + random(0, test.size()),
              random('a', 'z'));
  std::list<char> lst(RandomList(maxString));
  test.insert(test.begin() + random(0, test.size()),
              lst.begin(), lst.end());
}

template <class String> void clause_21_3_5_n(String & test) {
  // erase
  if (!test.empty()) {
    test.erase(random(0, test.size()), random(0, maxString));
  }
  if (!test.empty()) {
    // TODO: is erase(end()) allowed?
    test.erase(test.begin() + random(0, test.size() - 1));
  }
  if (!test.empty()) {
    auto const i = test.begin() + random(0, test.size());
    if (i != test.end()) {
      test.erase(i, i + random(0, size_t(test.end() - i)));
    }
  }
}

template <class String> void clause_21_3_5_o(String & test) {
  auto pos = random(0, test.size());
  if (avoidAliasing) {
    test.replace(pos, random(0, test.size() - pos),
                 String(test));
  } else {
    test.replace(pos, random(0, test.size() - pos), test);
  }
  pos = random(0, test.size());
  String s;
  randomString(&s, maxString);
  test.replace(pos, pos + random(0, test.size() - pos), s);
  auto pos1 = random(0, test.size());
  auto pos2 = random(0, test.size());
  if (avoidAliasing) {
    test.replace(pos1, pos1 + random(0, test.size() - pos1),
                 String(test),
                 pos2, pos2 + random(0, test.size() - pos2));
  } else {
    test.replace(pos1, pos1 + random(0, test.size() - pos1),
                 test, pos2, pos2 + random(0, test.size() - pos2));
  }
  pos1 = random(0, test.size());
  String str;
  randomString(&str, maxString);
  pos2 = random(0, str.size());
  test.replace(pos1, pos1 + random(0, test.size() - pos1),
               str, pos2, pos2 + random(0, str.size() - pos2));
  pos = random(0, test.size());
  if (avoidAliasing) {
    test.replace(pos, random(0, test.size() - pos),
                 String(test).c_str(), test.size());
  } else {
    test.replace(pos, random(0, test.size() - pos),
                 test.c_str(), test.size());
  }
  pos = random(0, test.size());
  randomString(&str, maxString);
  test.replace(pos, pos + random(0, test.size() - pos),
               str.c_str(), str.size());
  pos = random(0, test.size());
  randomString(&str, maxString);
  test.replace(pos, pos + random(0, test.size() - pos),
               str.c_str());
  pos = random(0, test.size());
  test.replace(pos, random(0, test.size() - pos),
               random(0, maxString), random('a', 'z'));
  pos = random(0, test.size());
  if (avoidAliasing) {
    test.replace(
      test.begin() + pos,
      test.begin() + pos + random(0, test.size() - pos),
      String(test));
  } else {
    test.replace(
      test.begin() + pos,
      test.begin() + pos + random(0, test.size() - pos),
      test);
  }
  pos = random(0, test.size());
  if (avoidAliasing) {
    test.replace(
      test.begin() + pos,
      test.begin() + pos + random(0, test.size() - pos),
      String(test).c_str(),
      test.size() - random(0, test.size()));
  } else {
    test.replace(
      test.begin() + pos,
      test.begin() + pos + random(0, test.size() - pos),
      test.c_str(),
      test.size() - random(0, test.size()));
  }
  pos = random(0, test.size());
  auto const n = random(0, test.size() - pos);
  typename String::iterator b = test.begin();
  String str1;
  randomString(&str1, maxString);
  const String & str3 = str1;
  const typename String::value_type* ss = str3.c_str();
  test.replace(
    b + pos,
    b + pos + n,
    ss);
  pos = random(0, test.size());
  test.replace(
    test.begin() + pos,
    test.begin() + pos + random(0, test.size() - pos),
    random(0, maxString), random('a', 'z'));
}

template <class String> void clause_21_3_5_p(String & test) {
  std::vector<typename String::value_type>
    vec(random(1, maxString));
  test.copy(
    &vec[0],
    vec.size(),
    random(0, test.size()));
}

template <class String> void clause_21_3_5_q(String & test) {
  String s;
  randomString(&s, maxString);
  s.swap(test);
}

template <class String> void clause_21_3_6_a(String & test) {
  // 21.3.6 string operations
  // exercise c_str() and data()
  assert(test.c_str() == test.data());
  // exercise get_allocator()
  String s;
  randomString(&s, maxString);
  assert(test.get_allocator() == s.get_allocator());
}

template <class String> void clause_21_3_6_b(String & test) {
  String str = test.substr(
    random(0, test.size()),
    random(0, test.size()));
  Num2String(test, test.find(str, random(0, test.size())));
}

template <class String> void clause_21_3_6_c(String & test) {
  auto from = random(0, test.size());
  auto length = random(0, test.size() - from);
  String str = test.substr(from, length);
  Num2String(test, test.find(str.c_str(),
                             random(0, test.size()),
                             random(0, str.size())));
}

template <class String> void clause_21_3_6_d(String & test) {
  String str = test.substr(
    random(0, test.size()),
    random(0, test.size()));
  Num2String(test, test.find(str.c_str(),
                             random(0, test.size())));
}

template <class String> void clause_21_3_6_e(String & test) {
  Num2String(test, test.find(
               random('a', 'z'),
               random(0, test.size())));
}

template <class String> void clause_21_3_6_f(String & test) {
  String str = test.substr(
    random(0, test.size()),
    random(0, test.size()));
  Num2String(test, test.rfind(str, random(0, test.size())));
}

template <class String> void clause_21_3_6_g(String & test) {
  String str = test.substr(
    random(0, test.size()),
    random(0, test.size()));
  Num2String(test, test.rfind(str.c_str(),
                              random(0, test.size()),
                              random(0, str.size())));
}

template <class String> void clause_21_3_6_h(String & test) {
  String str = test.substr(
    random(0, test.size()),
    random(0, test.size()));
  Num2String(test, test.rfind(str.c_str(),
                              random(0, test.size())));
}

template <class String> void clause_21_3_6_i(String & test) {
  Num2String(test, test.rfind(
               random('a', 'z'),
               random(0, test.size())));
}

template <class String> void clause_21_3_6_j(String & test) {
  String str;
  randomString(&str, maxString);
  Num2String(test, test.find_first_of(str,
                                      random(0, test.size())));
}

template <class String> void clause_21_3_6_k(String & test) {
  String str;
  randomString(&str, maxString);
  Num2String(test, test.find_first_of(str.c_str(),
                                      random(0, test.size()),
                                      random(0, str.size())));
}

template <class String> void clause_21_3_6_l(String & test) {
  String str;
  randomString(&str, maxString);
  Num2String(test, test.find_first_of(str.c_str(),
                                      random(0, test.size())));
}

template <class String> void clause_21_3_6_m(String & test) {
  Num2String(test, test.find_first_of(
               random('a', 'z'),
               random(0, test.size())));
}

template <class String> void clause_21_3_6_n(String & test) {
  String str;
  randomString(&str, maxString);
  Num2String(test, test.find_last_of(str,
                                     random(0, test.size())));
}

template <class String> void clause_21_3_6_o(String & test) {
  String str;
  randomString(&str, maxString);
  Num2String(test, test.find_last_of(str.c_str(),
                                     random(0, test.size()),
                                     random(0, str.size())));
}

template <class String> void clause_21_3_6_p(String & test) {
  String str;
  randomString(&str, maxString);
  Num2String(test, test.find_last_of(str.c_str(),
                                     random(0, test.size())));
}

template <class String> void clause_21_3_6_q(String & test) {
  Num2String(test, test.find_last_of(
               random('a', 'z'),
               random(0, test.size())));
}

template <class String> void clause_21_3_6_r(String & test) {
  String str;
  randomString(&str, maxString);
  Num2String(test, test.find_first_not_of(str,
                                          random(0, test.size())));
}

template <class String> void clause_21_3_6_s(String & test) {
  String str;
  randomString(&str, maxString);
  Num2String(test, test.find_first_not_of(str.c_str(),
                                          random(0, test.size()),
                                          random(0, str.size())));
}

template <class String> void clause_21_3_6_t(String & test) {
  String str;
  randomString(&str, maxString);
  Num2String(test, test.find_first_not_of(str.c_str(),
                                          random(0, test.size())));
}

template <class String> void clause_21_3_6_u(String & test) {
  Num2String(test, test.find_first_not_of(
               random('a', 'z'),
               random(0, test.size())));
}

template <class String> void clause_21_3_6_v(String & test) {
  String str;
  randomString(&str, maxString);
  Num2String(test, test.find_last_not_of(str,
                                         random(0, test.size())));
}

template <class String> void clause_21_3_6_w(String & test) {
  String str;
  randomString(&str, maxString);
  Num2String(test, test.find_last_not_of(str.c_str(),
                                         random(0, test.size()),
                                         random(0, str.size())));
}

template <class String> void clause_21_3_6_x(String & test) {
  String str;
  randomString(&str, maxString);
  Num2String(test, test.find_last_not_of(str.c_str(),
                                         random(0, test.size())));
}

template <class String> void clause_21_3_6_y(String & test) {
  Num2String(test, test.find_last_not_of(
               random('a', 'z'),
               random(0, test.size())));
}

template <class String> void clause_21_3_6_z(String & test) {
  test = test.substr(random(0, test.size()), random(0, test.size()));
}

template <class String> void clause_21_3_7_a(String & test) {
  String s;
  randomString(&s, maxString);
  int tristate = test.compare(s);
  if (tristate > 0) tristate = 1;
  else if (tristate < 0) tristate = 2;
  Num2String(test, tristate);
}

template <class String> void clause_21_3_7_b(String & test) {
  String s;
  randomString(&s, maxString);
  int tristate = test.compare(
    random(0, test.size()),
    random(0, test.size()),
    s);
  if (tristate > 0) tristate = 1;
  else if (tristate < 0) tristate = 2;
  Num2String(test, tristate);
}

template <class String> void clause_21_3_7_c(String & test) {
  String str;
  randomString(&str, maxString);
  int tristate = test.compare(
    random(0, test.size()),
    random(0, test.size()),
    str,
    random(0, str.size()),
    random(0, str.size()));
  if (tristate > 0) tristate = 1;
  else if (tristate < 0) tristate = 2;
  Num2String(test, tristate);
}

template <class String> void clause_21_3_7_d(String & test) {
  String s;
  randomString(&s, maxString);
  int tristate = test.compare(s.c_str());
  if (tristate > 0) tristate = 1;
  else if (tristate < 0) tristate = 2;
                Num2String(test, tristate);
}

template <class String> void clause_21_3_7_e(String & test) {
  String str;
  randomString(&str, maxString);
  int tristate = test.compare(
    random(0, test.size()),
    random(0, test.size()),
    str.c_str(),
    random(0, str.size()));
  if (tristate > 0) tristate = 1;
  else if (tristate < 0) tristate = 2;
  Num2String(test, tristate);
}

template <class String> void clause_21_3_7_f(String & test) {
  String s1;
  randomString(&s1, maxString);
  String s2;
  randomString(&s2, maxString);
  test = s1 + s2;
}

template <class String> void clause_21_3_7_g(String & test) {
  String s;
  randomString(&s, maxString);
  String s1;
  randomString(&s1, maxString);
  test = s.c_str() + s1;
}

template <class String> void clause_21_3_7_h(String & test) {
  String s;
  randomString(&s, maxString);
  test = typename String::value_type(random('a', 'z')) + s;
}

template <class String> void clause_21_3_7_i(String & test) {
  String s;
  randomString(&s, maxString);
  String s1;
  randomString(&s1, maxString);
  test = s + s1.c_str();
}

template <class String> void clause_21_3_7_j(String & test) {
  String s;
  randomString(&s, maxString);
  String s1;
  randomString(&s1, maxString);
  test = s + s1.c_str();
}

template <class String> void clause_21_3_7_k(String & test) {
  String s;
  randomString(&s, maxString);
  test = s + typename String::value_type(random('a', 'z'));
}

// Numbering here is from C++11
template <class String> void clause_21_4_8_9_a(String & test) {
 std::stringstream s("asd asdfjhuhdf    asdfasdf\tasdsdf");
  String str;
  while (s) {
    s >> str;
    test += str + test;
  }
}

void test_fbstring(){
 {
	 	std::cout << "Testing fbstring ...\n";

  std::string r;
  folly::fbstring c;
#define TEST_CLAUSE(x)                                              \
  do {                                                              \
      if (1) {} else std::cerr << "Testing clause " << #x <<std::endl;   \
      randomString(&r);                                             \
      c = r;                                                        \
      BOOST_CHECK(c== r);                                              \
      auto localSeed = seed + count;                                \
      rng = RandomT(localSeed);                                     \
      clause_##x(r);                                                \
      rng = RandomT(localSeed);                                     \
      clause_##x(c);                                                \
      BOOST_CHECK(r== c); \
  /*std::cerr                                               
        << "Lengths: " << r.size() << " vs. " << c.size()   << std::endl        \
        << "\nReference: '" << r << "'"                             \
        << "\nActual:    '" << c.data()[0] << "'";  */;                \
    } while (++count % 100 != 0)

  int count = 0;
  TEST_CLAUSE(21_3_1_a);
  TEST_CLAUSE(21_3_1_b);
  TEST_CLAUSE(21_3_1_c);
  TEST_CLAUSE(21_3_1_d);
  TEST_CLAUSE(21_3_1_e);
  TEST_CLAUSE(21_3_1_f);
  TEST_CLAUSE(21_3_1_g);

  TEST_CLAUSE(21_3_2);
  TEST_CLAUSE(21_3_3);
  TEST_CLAUSE(21_3_4);
  TEST_CLAUSE(21_3_5_a);
  TEST_CLAUSE(21_3_5_b);
  TEST_CLAUSE(21_3_5_c);
  TEST_CLAUSE(21_3_5_d);
  TEST_CLAUSE(21_3_5_e);
  TEST_CLAUSE(21_3_5_f);
  TEST_CLAUSE(21_3_5_g);
  TEST_CLAUSE(21_3_5_h);
  TEST_CLAUSE(21_3_5_i);
  TEST_CLAUSE(21_3_5_j);
  TEST_CLAUSE(21_3_5_k);
  TEST_CLAUSE(21_3_5_l);
  TEST_CLAUSE(21_3_5_m);
  TEST_CLAUSE(21_3_5_n);
  TEST_CLAUSE(21_3_5_o);
  TEST_CLAUSE(21_3_5_p);

  TEST_CLAUSE(21_3_6_a);
  TEST_CLAUSE(21_3_6_b);
  TEST_CLAUSE(21_3_6_c);
  TEST_CLAUSE(21_3_6_d);
  TEST_CLAUSE(21_3_6_e);
  TEST_CLAUSE(21_3_6_f);
  TEST_CLAUSE(21_3_6_g);
  TEST_CLAUSE(21_3_6_h);
  TEST_CLAUSE(21_3_6_i);
  TEST_CLAUSE(21_3_6_j);
  TEST_CLAUSE(21_3_6_k);
  TEST_CLAUSE(21_3_6_l);
  TEST_CLAUSE(21_3_6_m);
  TEST_CLAUSE(21_3_6_n);
  TEST_CLAUSE(21_3_6_o);
  TEST_CLAUSE(21_3_6_p);
  TEST_CLAUSE(21_3_6_q);
  TEST_CLAUSE(21_3_6_r);
  TEST_CLAUSE(21_3_6_s);
  TEST_CLAUSE(21_3_6_t);
  TEST_CLAUSE(21_3_6_u);
  TEST_CLAUSE(21_3_6_v);
  TEST_CLAUSE(21_3_6_w);
  TEST_CLAUSE(21_3_6_x);
  TEST_CLAUSE(21_3_6_y);
  TEST_CLAUSE(21_3_6_z);

  TEST_CLAUSE(21_3_7_a);
  TEST_CLAUSE(21_3_7_b);
  TEST_CLAUSE(21_3_7_c);
  TEST_CLAUSE(21_3_7_d);
  TEST_CLAUSE(21_3_7_e);
  TEST_CLAUSE(21_3_7_f);
  TEST_CLAUSE(21_3_7_g);
  TEST_CLAUSE(21_3_7_h);
  TEST_CLAUSE(21_3_7_i);
  TEST_CLAUSE(21_3_7_j);
  TEST_CLAUSE(21_3_7_k);

  TEST_CLAUSE(21_4_8_9_a);
}
 using namespace std;
{
  fbstring s1 = "\
Lorem ipsum dolor sit amet, consectetur adipiscing elit. Cras accumsan \n\
elit ut urna consectetur in sagittis mi auctor. Nulla facilisi. In nec \n\
dolor leo, vitae imperdiet neque. Donec ut erat mauris, a faucibus \n\
elit. Integer consectetur gravida augue, sit amet mattis mauris auctor \n\
sed. Morbi congue libero eu nunc sodales adipiscing. In lectus nunc, \n\
vulputate a fringilla at, venenatis quis justo. Proin eu velit \n\
nibh. Maecenas vitae tellus eros. Pellentesque habitant morbi \n\
tristique senectus et netus et malesuada fames ac turpis \n\
egestas. Vivamus faucibus feugiat consequat. Donec fermentum neque sit \n\
amet ligula suscipit porta. Phasellus facilisis felis in purus luctus \n\
quis posuere leo tempor. Nam nunc purus, luctus a pharetra ut, \n\
placerat at dui. Donec imperdiet, diam quis convallis pulvinar, dui \n\
est commodo lorem, ut tincidunt diam nibh et nibh. Maecenas nec velit \n\
massa, ut accumsan magna. Donec imperdiet tempor nisi et \n\
laoreet. Phasellus lectus quam, ultricies ut tincidunt in, dignissim \n\
id eros. Mauris vulputate tortor nec neque pellentesque sagittis quis \n\
sed nisl. In diam lacus, lobortis ut posuere nec, ornare id quam.";
  const char* f = "c:\\temp\\fbstring_testing";
  {
    std::ofstream out(f);
    if (!(out << s1)) {
      std::cout << "Couldn't write to temp file.";
      return;
    }
  }
  std::vector<fbstring> v;
  boost::split(v, s1, boost::is_any_of("\n"));
  ifstream input(f);
  fbstring line;
  FOR_EACH (i, v) {
    BOOST_CHECK(getline(input, line));
    BOOST_CHECK(line== *i);
  }
}

{
  // Move constructor. Make sure we allocate a large string, so the
  // small string optimization doesn't kick in.
  auto size = random(100, 2000);
  fbstring s(size, 'a');
  fbstring test = std::move(s);
  BOOST_CHECK(s.empty());
  BOOST_CHECK(size== test.size());
}

 {
  // Move constructor. Make sure we allocate a large string, so the
  // small string optimization doesn't kick in.
  auto size = random(100, 2000);
  fbstring s(size, 'a');
  fbstring test;
  test = std::move(s);
  BOOST_CHECK(s.empty());
  BOOST_CHECK(size== test.size());
}

{
  // Make sure we allocate a large string, so the
  // small string optimization doesn't kick in.
  auto size1 = random(100, 2000);
  auto size2 = random(100, 2000);
  fbstring s1(size1, 'a');
  fbstring s2(size2, 'b');
  fbstring test;
  test = std::move(s1) + s2;
  BOOST_CHECK(s1.empty());
  BOOST_CHECK(size1 + size2== test.size());
}

 {
  // Make sure we allocate a large string, so the
  // small string optimization doesn't kick in.
  auto size1 = random(100, 2000);
  auto size2 = random(100, 2000);
  fbstring s1(size1, 'a');
  fbstring s2(size2, 'b');
  fbstring test;
  test = s1 + std::move(s2);
  BOOST_CHECK(size1 + size2== test.size());
}

 {
  // Below fails on std::string
  //try {
  //  std::string s(0);
  //  BOOST_CHECK(false);
  //} catch (const std::logic_error&) {
  //} catch (...) {
  //  BOOST_CHECK(false);
  //}

  try {
    fbstring s(0);
    BOOST_CHECK(false);
  } catch (const std::logic_error& e) {
  } catch (...) {
    BOOST_CHECK(false);
  }
}

{
  { // D479397
    fbstring str(1337, 'f');
    fbstring cp = str;
    cp.clear();
    cp.c_str();
    BOOST_CHECK(str.front()== 'f');
  }
  { // D481173, --extra-cxxflags=-DFBSTRING_CONSERVATIVE
    fbstring str(1337, 'f');
    for (int i = 0; i < 2; ++i) {
      fbstring cp = str;
      cp[1] = 'b';
      BOOST_CHECK(cp.c_str()[cp.size()]== '\0');
      cp.push_back('?');
    }
  }

  {
	  std::vector<char> vec;
	  fbstring fstr; std::string str;
	  fstr.assign(vec.begin(),vec.end());
	  str.assign(vec.begin(),vec.end());
	  BOOST_CHECK(fstr.size()==0);

  }
}
	std::cout << "... Finished testing fbstring \n";

}

//#define CONCAT(A, B) CONCAT_HELPER(A, B)
//#define CONCAT_HELPER(A, B) A##B
//#define BENCHFUN(F) CONCAT(CONCAT(BM_, F), CONCAT(_, STRING))
//
//#define STRING string
//#include "folly/test/FBStringTestBenchmarks.cpp.h"
//#undef STRING
//#define STRING fbstring
//#include "folly/test/FBStringTestBenchmarks.cpp.h"
//#undef STRING


// small_vector test
namespace {

struct NontrivialType {
  static int ctored;
  explicit NontrivialType() : a(0) {}

  /* implicit */ NontrivialType(int a) : a(a) {
    ++ctored;
  }

  NontrivialType(NontrivialType const& s) {
    ++ctored;
  }

  NontrivialType& operator=(NontrivialType const& o) {
    a = o.a;
    return *this;
  }

  int32_t a;
};
static_assert(!boost::has_trivial_copy<NontrivialType>::value,
              "NontrivialType isn't trivially copyable");

int NontrivialType::ctored = 0;

struct TestException {};

int throwCounter = 1;
void MaybeThrow() {
  if (!--throwCounter) {
    throw TestException();
  }
}

const int kMagic = 0xdeadbeef;
struct Thrower {
  static int alive;

  Thrower() : magic(kMagic) {
    BOOST_CHECK(magic== kMagic);
    MaybeThrow();
    ++alive;
  }
  Thrower(Thrower const& other) : magic(other.magic) {
    BOOST_CHECK(magic== kMagic);
    MaybeThrow();
    ++alive;
  }
  ~Thrower()  {
    BOOST_CHECK(magic== kMagic);
    magic = 0;
    --alive;
  }

  Thrower& operator=(Thrower const& other) {
    BOOST_CHECK(magic== kMagic);
    MaybeThrow();
    return *this;
  }

  // This is just to try to make sure we don't get our member
  // functions called on uninitialized memory.
  int magic;
};

int Thrower::alive = 0;

// Type that counts how many exist and doesn't support copy
// construction.
struct NoncopyableCounter {
  static int alive;
  NoncopyableCounter() {
    ++alive;
  }
  ~NoncopyableCounter() {
    --alive;
  }
  NoncopyableCounter(NoncopyableCounter&&) { ++alive; }
private:
  NoncopyableCounter(NoncopyableCounter const&);// = delete;
  NoncopyableCounter& operator=(NoncopyableCounter const&);// const = delete;
public:
  NoncopyableCounter& operator=(NoncopyableCounter&&) { return *this; }
};
int NoncopyableCounter::alive = 0;

// Check that throws don't break the basic guarantee for some cases.
// Uses the method for testing exception safety described at
// http://www.boost.org/community/exception_safety.html, to force all
// throwing code paths to occur.
struct TestBasicGuarantee {
  folly::small_vector<Thrower,3> vec;
  int const prepopulate;

  explicit TestBasicGuarantee(int prepopulate)
    : prepopulate(prepopulate)
  {
    throwCounter = 1000;
    for (int i = 0; i < prepopulate; ++i) {
      vec.push_back(Thrower());
    }
  }

  ~TestBasicGuarantee() {
    throwCounter = 1000;
  }

  template<class Operation>
  void operator()(int insertCount, Operation const& op) {
    bool done = false;

    std::unique_ptr<folly::small_vector<Thrower,3> > workingVec;
    for (int counter = 1; !done; ++counter) {
      throwCounter = 1000;
      workingVec.reset(new folly::small_vector<Thrower,3>(vec));
      throwCounter = counter;
      BOOST_CHECK(Thrower::alive== prepopulate * 2);
      try {
        op(*workingVec);
        done = true;
      } catch (...) {
        // Note that the size of the vector can change if we were
        // inserting somewhere other than the end (it's a basic only
        // guarantee).  All we're testing here is that we have the
        // right amount of uninitialized vs initialized memory.
        BOOST_CHECK(Thrower::alive== workingVec->size() + vec.size());
        continue;
      }

      // If things succeeded.
      BOOST_CHECK(workingVec->size()== prepopulate + insertCount);
      BOOST_CHECK(Thrower::alive== prepopulate * 2 + insertCount);
    }
  }
};

}


void test_small_vector(){
		std::cout << "Testing small_vector ...\n";

	using folly::small_vector;
using namespace folly::small_vector_policy;

#if defined(__x86_64__)

static_assert(sizeof(small_vector<int>) == 16,
              "Object size is not what we expect for small_vector<int>");
static_assert(sizeof(small_vector<int32_t,2>) == 16,
              "Object size is not what we expect for "
              "small_vector<int32_t,2>");
static_assert(sizeof(small_vector<int,10>) ==
                10 * sizeof(int) + sizeof(std::size_t),
              "Object size is not what we expect for small_vector<int,10>");

static_assert(sizeof(small_vector<int32_t,1,uint32_t>) ==
                8 + 4,
              "small_vector<int32_t,1,uint32_t> is wrong size");
static_assert(sizeof(small_vector<int32_t,1,uint16_t>) ==
                8 + 2,
              "small_vector<int32_t,1,uint32_t> is wrong size");
static_assert(sizeof(small_vector<int32_t,1,uint8_t>) ==
                8 + 1,
              "small_vector<int32_t,1,uint32_t> is wrong size");

static_assert(sizeof(small_vector<int32_t,1,OneBitMutex>) == 16,
              "OneBitMutex took more space than expected");

static_assert(sizeof(small_vector<int16_t,4,uint16_t>) == 10,
              "Sizeof unexpectedly large");
static_assert(sizeof(small_vector<int16_t,4,uint16_t,OneBitMutex>) == 10,
              "Sizeof unexpectedly large");
static_assert(sizeof(small_vector<int16_t,4,NoHeap,uint16_t,
                                  OneBitMutex>) == 10,
              "Sizeof unexpectedly large");

#endif


 {
  for (int prepop = 1; prepop < 30; ++prepop) {
    (TestBasicGuarantee(prepop))( // parens or a mildly vexing parse :(
      1,
      [&] (folly::small_vector<Thrower,3>& v) {
        v.push_back(Thrower());
      }
    );

    BOOST_CHECK(Thrower::alive== 0);

    (TestBasicGuarantee(prepop))(
      1,
      [&] (folly::small_vector<Thrower,3>& v) {
        v.insert(v.begin(), Thrower());
      }
    );

    BOOST_CHECK(Thrower::alive== 0);

    (TestBasicGuarantee(prepop))(
      1,
      [&] (folly::small_vector<Thrower,3>& v) {
        v.insert(v.begin() + 1, Thrower());
      }
    );

    BOOST_CHECK(Thrower::alive== 0);
  }

  TestBasicGuarantee(4)(
    3,
    [&] (folly::small_vector<Thrower,3>& v) {
      std::vector<Thrower> b;
      b.push_back(Thrower());
      b.push_back(Thrower());
      b.push_back(Thrower());

      /*
       * Apparently if you do the following initializer_list instead
       * of the above push_back's, and one of the Throwers throws,
       * g++4.6 doesn't destruct the previous ones.  Heh.
       */
      //b = { Thrower(), Thrower(), Thrower() };
      v.insert(v.begin() + 1, b.begin(), b.end());
    }
  );

  TestBasicGuarantee(2)(
    6,
    [&] (folly::small_vector<Thrower,3>& v) {
      std::vector<Thrower> b;
      for (int i = 0; i < 6; ++i) {
        b.push_back(Thrower());
      }

      v.insert(v.begin() + 1, b.begin(), b.end());
    }
  );

  BOOST_CHECK(Thrower::alive== 0);
  try {
    throwCounter = 4;
    folly::small_vector<Thrower,1> p(14, Thrower());
  } catch (...) {
  }
  BOOST_CHECK(Thrower::alive== 0);
}

// Run this with.
// MALLOC_CONF=prof_leak:true
// LD_PRELOAD=${JEMALLOC_PATH}/lib/libjemalloc.so.1
// LD_PRELOAD="$LD_PRELOAD:"${UNWIND_PATH}/lib/libunwind.so.7
 {
  for (int j = 0; j < 1000; ++j) {
    folly::small_vector<int, 10> someVec(300);
    for (int i = 0; i < 10000; ++i) {
      someVec.push_back(12);
    }
  }
}

{
  folly::small_vector<int> someVec(3, 3);
  someVec.insert(someVec.begin(), 12, 12);
  BOOST_CHECK(someVec.size()== 15);
  for (int i = 0; i < someVec.size(); ++i) {
    if (i < 12) {
      BOOST_CHECK(someVec[i]== 12);
    } else {
      BOOST_CHECK(someVec[i]== 3);
    }
  }

  auto oldSize = someVec.size();
  someVec.insert(someVec.begin() + 1, 12, 12);
  BOOST_CHECK(someVec.size()== oldSize + 12);

  folly::small_vector<std::string> v1(6, "asd"), v2(7, "wat");
  v1.insert(v1.begin() + 1, v2.begin(), v2.end());
  BOOST_CHECK(v1.size() == 6 + 7);
  BOOST_CHECK(v1.front()== "asd");
  BOOST_CHECK(v1[1]== "wat");
}

{
  folly::small_vector<int,10> somethingVec, emptyVec;
  somethingVec.push_back(1);
  somethingVec.push_back(2);
  somethingVec.push_back(3);
  somethingVec.push_back(4);

  // Swapping intern'd with intern'd.
  auto vec = somethingVec;
  BOOST_CHECK(vec == somethingVec);
  BOOST_CHECK(!(vec == emptyVec));
  BOOST_CHECK(!(somethingVec == emptyVec));

  // Swapping a heap vector with an intern vector.
  folly::small_vector<int,10> junkVec;
  junkVec.assign(12, 12);
  BOOST_CHECK(junkVec.size()== 12);
  for (auto i : junkVec) {
    BOOST_CHECK(i== 12);
  }
  swap(junkVec, vec);
  BOOST_CHECK(junkVec == somethingVec);
  BOOST_CHECK(vec.size()== 12);
  for (auto i : vec) {
    BOOST_CHECK(i== 12);
  }

  // Swapping two heap vectors.
  folly::small_vector<int,10> moreJunk(15, 15);
  BOOST_CHECK(moreJunk.size()== 15);
  for (auto i : moreJunk) {
    BOOST_CHECK(i== 15);
  }
  swap(vec, moreJunk);
  BOOST_CHECK(moreJunk.size()== 12);
  for (auto i : moreJunk) {
    BOOST_CHECK(i== 12);
  }
  BOOST_CHECK(vec.size()== 15);
  for (auto i : vec) {
    BOOST_CHECK(i== 15);
  }

  // Making a vector heap, then smaller than another non-heap vector,
  // then swapping.
  folly::small_vector<int,5> shrinker, other(4, 10);
  std::array<int,9> ar = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
  shrinker.assign(ar.begin(),ar.end());
  shrinker.erase(shrinker.begin() + 2, shrinker.end());
  BOOST_CHECK(shrinker.size() < other.size());
  swap(shrinker, other);
  BOOST_CHECK(shrinker.size()== 4);
  std::vector<int> jrbvec;
  jrbvec.push_back(10);
  BOOST_CHECK(boost::all(shrinker, boost::is_any_of(jrbvec)));
  std::array<int,2> ar2 = {0,1};
  BOOST_CHECK((other == small_vector<int,5>(ar2.begin(),ar2.end())));
}

 {
  NontrivialType::ctored = 0;

  folly::small_vector<NontrivialType> vec;
  vec.reserve(1024);
  vec.emplace_back(12);
  BOOST_CHECK(NontrivialType::ctored== 1);
  BOOST_CHECK(vec.front().a== 12);
  vec.emplace_back(13);
  BOOST_CHECK(vec.front().a== 12);
  BOOST_CHECK(vec.back().a== 13);
  BOOST_CHECK(NontrivialType::ctored== 2);

  NontrivialType::ctored = 0;
  for (int i = 0; i < 120; ++i) {
    vec.emplace_back(i);
  }
  BOOST_CHECK(NontrivialType::ctored== 120);
  BOOST_CHECK(vec[0].a== 12);
  BOOST_CHECK(vec[1].a== 13);
  BOOST_CHECK(vec.back().a== 119);

  // We implement emplace() with a temporary (see the implementation
  // for a comment about why), so this should make 2 ctor calls.
  NontrivialType::ctored = 0;
  vec.emplace(vec.begin(), 12);
  BOOST_CHECK(NontrivialType::ctored== 2);
}

 {
	 std::array<int,5> ar =  { 1, 2, 3, 4, 5 };
  folly::small_vector<int,4> notherVec; notherVec.assign(ar.begin(),ar.end());
  BOOST_CHECK(notherVec.front()== 1);
  BOOST_CHECK(notherVec.size()== 5);
  notherVec.erase(notherVec.begin());
  BOOST_CHECK(notherVec.front()== 2);
  BOOST_CHECK(notherVec.size()== 4);
  BOOST_CHECK(notherVec[2]== 4);
  BOOST_CHECK(notherVec[3]== 5);
  notherVec.erase(notherVec.begin() + 2);
  BOOST_CHECK(notherVec.size()== 3);
  BOOST_CHECK(notherVec[2]== 5);

  std::array<int,10> ar2 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
  folly::small_vector<int,2> vec2(ar2.begin(),ar2.end());
  vec2.erase(vec2.begin() + 1, vec2.end() - 1);
  std::array<int,2> ar3 = { 1, 10 };
  folly::small_vector<int,2> expected(ar3.begin(),ar3.end()); 
  BOOST_CHECK(vec2 == expected);

  folly::small_vector<std::string,3> v(102, "ASD");
  v.resize(1024, "D");
  BOOST_CHECK(v.size()== 1024);
  BOOST_CHECK(v.back()== "D");
  BOOST_CHECK(v.front()== "ASD");
  v.resize(1);
  BOOST_CHECK(v.front()== "ASD");
  BOOST_CHECK(v.size()== 1);
  v.resize(0);
  BOOST_CHECK(v.empty());
}

 {
	 std::array<int,5> ar = { 1, 2, 3, 4, 5 };
	 folly::small_vector<NontrivialType,7> vec(ar.begin(),ar.end());
  std::generate_n(std::back_inserter(vec), 102, std::rand);

  auto capacity = vec.capacity();

  auto oldSize = vec.size();
  for (int i = 0; i < oldSize; ++i) {
    vec.erase(vec.begin() + (std::rand() % vec.size()));
    BOOST_CHECK(vec.capacity()== capacity);
  }
  BOOST_CHECK(vec.empty());

  BOOST_CHECK(vec.capacity()== capacity);
  std::generate_n(std::back_inserter(vec), 102, std::rand);
  BOOST_CHECK(vec.capacity()== capacity);

  std::generate_n(std::back_inserter(vec), 4096, std::rand);
  BOOST_CHECK(vec.capacity() > capacity);

  vec.resize(10);
  vec.shrink_to_fit();
  BOOST_CHECK(vec.capacity() < capacity);
  vec.resize(4);
  vec.shrink_to_fit();
  BOOST_CHECK(vec.capacity()== 7); // in situ size
}

{
	std::array<std::string,2> ar = { "foo", "bar" };
  folly::small_vector<std::string,3> vec(ar.begin(),ar.end());
  vec.push_back("blah");
  vec.push_back("blah2");
  vec.push_back("blah3");
  vec.erase(vec.begin() + 2);

  std::vector<std::string> otherVec;
  for (auto& s : vec) {
    otherVec.push_back(s);
  }
  BOOST_CHECK(otherVec.size()== vec.size());
  if (otherVec.size() == vec.size()) {
    BOOST_CHECK(std::equal(otherVec.begin(), otherVec.end(), vec.begin()));
  }

  std::reverse(otherVec.begin(), otherVec.end());
  auto oit = otherVec.begin();
  auto rit = vec.crbegin();
  for (; rit != vec.crend(); ++oit, ++rit) {
    BOOST_CHECK(*oit== *rit);
  }
}

 {
  folly::small_vector<std::unique_ptr<std::string>,2> vec;
  for (int i = 0; i < 10; ++i) {
    vec.emplace(vec.begin(), new std::string("asd"));
  }
  BOOST_CHECK(vec.size()== 10);
  auto vec2 = std::move(vec);
  BOOST_CHECK(vec.size()== 0);
  BOOST_CHECK(vec2.size()== 10);
  vec2.clear();

  folly::small_vector<NoncopyableCounter,3> vec3;
  for (int i = 0; i < 10; ++i) {
    BOOST_CHECK(vec3.size()== i);
    BOOST_CHECK(NoncopyableCounter::alive== i);
    vec3.insert(vec3.begin(), NoncopyableCounter());
  }
  BOOST_CHECK(vec3.size()== 10);
  BOOST_CHECK(NoncopyableCounter::alive== 10);

  vec3.insert(vec3.begin() + 3, NoncopyableCounter());
  BOOST_CHECK(NoncopyableCounter::alive== 11);
  auto vec4 = std::move(vec3);
  BOOST_CHECK(NoncopyableCounter::alive== 11);
  vec4.resize(30);
  BOOST_CHECK(NoncopyableCounter::alive== 30);
  vec4.erase(vec4.begin(), vec4.end());
  BOOST_CHECK(vec4.size()== 0);
  BOOST_CHECK(NoncopyableCounter::alive== 0);
}

 {
  folly::small_vector<std::string,10> v1;
  v1.push_back("asd");
  v1.push_back("bsd");
  auto v2 = std::move(v1);
  BOOST_CHECK(v2.size()== 2);
  BOOST_CHECK(v2[0]== "asd");
  BOOST_CHECK(v2[1]== "bsd");

  v1 = std::move(v2);
  BOOST_CHECK(v1.size()== 2);
  BOOST_CHECK(v1[0]== "asd");
  BOOST_CHECK(v1[1]== "bsd");
}

 {
  typedef folly::small_vector<std::string,10,
    std::size_t,folly::small_vector_policy::NoHeap> Vector;

  Vector v;
  BOOST_CHECK(v.max_size()== 10);

  for (int i = 0; i < 10; ++i) {
    v.push_back(folly::to<std::string>(i));
    BOOST_CHECK(v.size()== i + 1);
  }

  bool caught = false;
  try {
    v.insert(v.begin(), "ha");
  } catch (const std::length_error&) {
    caught = true;
  }
  BOOST_CHECK(caught);

  // Check max_size works right with various policy combinations.
  //folly::small_vector<std::string,32,uint32_t,NoHeap,OneBitMutex> v2;
  //BOOST_CHECK(v2.max_size()== 32);
  //folly::small_vector<std::string,32,uint32_t,OneBitMutex> v3;
  //BOOST_CHECK(v3.max_size()== (1ul << 30) - 1);
  folly::small_vector<std::string,32,uint32_t> v4;
  BOOST_CHECK(v4.max_size()== (1ul << 31) - 1);

  /*
   * Test that even when we ask for a small number inlined it'll still
   * inline at least as much as it takes to store the value_type
   * pointer.
   */
  folly::small_vector<char,1,NoHeap> notsosmall;
  BOOST_CHECK(notsosmall.max_size()== sizeof(char*));
  caught = false;
  try {
    notsosmall.push_back(12);
    notsosmall.push_back(13);
    notsosmall.push_back(14);
  } catch (const std::length_error&) {
    caught = true;
  }
  BOOST_CHECK(!caught);
}
 {
  folly::small_vector<int,2,uint8_t> vec;
  BOOST_CHECK(vec.max_size()== 127);
  folly::small_vector<int,2,uint16_t> vec2;
  BOOST_CHECK(vec2.max_size()== (1 << 15) - 1);
  //folly::small_vector<int,2,uint16_t,OneBitMutex> vec3;
  //BOOST_CHECK(vec3.max_size()== (1 << 14) - 1);
}

 {
  // Use something bigger than the pointer so it can't get inlined.
  struct SomeObj {
    double a, b, c, d, e; int val;
    SomeObj(int val) : val(val) {}
    bool operator==(SomeObj const& o) const {
      return o.val == val;
    }
  };
  std::array<SomeObj,1> ar = { 1 };
  folly::small_vector<SomeObj,0> vec(ar.begin(),ar.end()); 
  BOOST_CHECK(vec.size()== 1);
  if (!vec.empty()) {
    BOOST_CHECK(vec[0] == 1);
  }
  std::array<int,4> ar2 = { 0, 1, 2, 3 };
  vec.insert(vec.begin(),ar2.begin(),ar2.end() );
  BOOST_CHECK(vec.size()== 5);
  std::array<int,5> ar3 = { 0, 1, 2, 3, 1 };
  BOOST_CHECK((vec == folly::small_vector<SomeObj,0>(ar3.begin(),ar3.end())));
}

{
  typedef folly::small_vector<int,3,uint32_t
#ifdef __x86_64__
    ,OneBitMutex
#endif
  > Vector;

  Vector a;

#ifdef __x86_64__
  a.lock();
  a.unlock();
#endif

  a.push_back(12);
  BOOST_CHECK(a.front()== 12);
  BOOST_CHECK(a.size()== 1);
  a.push_back(13);
  BOOST_CHECK(a.size()== 2);
  BOOST_CHECK(a.front()== 12);
  BOOST_CHECK(a.back()== 13);

  a.emplace(a.end(), 32);
  BOOST_CHECK(a.back()== 32);

  a.emplace(a.begin(), 12);
  BOOST_CHECK(a.front()== 12);
  BOOST_CHECK(a.back()== 32);
  a.erase(a.end() - 1);
  BOOST_CHECK(a.back()== 13);

  a.push_back(12);
  BOOST_CHECK(a.back()== 12);
  a.pop_back();
  BOOST_CHECK(a.back()== 13);

  const int s = 12;
  a.push_back(s); // lvalue reference

  Vector b, c;
  b = a;
  BOOST_CHECK(b == a);
  c = std::move(b);
  BOOST_CHECK(c == a);
  BOOST_CHECK(c != b && b != a);

  BOOST_CHECK(c.size()> 0);
  c.resize(1);
  BOOST_CHECK(c.size()== 1);

  Vector intCtor(12);
}

 {
  folly::small_vector<unsigned long, 1> vec;
  BOOST_CHECK(vec.size()== 0);
  BOOST_CHECK(vec.capacity()== 1);

  vec.push_back(0);
  BOOST_CHECK(vec.size()== 1);
  BOOST_CHECK(vec.capacity()== 1);

  vec.push_back(1);
  BOOST_CHECK(vec.size()== 2);
  BOOST_CHECK(vec.capacity()> 1);


  folly::small_vector<unsigned long, 2> vec2;
  BOOST_CHECK(vec2.size()== 0);
  BOOST_CHECK(vec2.capacity()== 2);

  vec2.push_back(0);
  vec2.push_back(1);
  BOOST_CHECK(vec2.size()== 2);
  BOOST_CHECK(vec2.capacity()== 2);

  vec2.push_back(2);
  BOOST_CHECK(vec2.size()== 3);
  BOOST_CHECK(vec2.capacity()> 2);

  int v1c = vec.capacity();
  int v2c = vec2.capacity();


  // Both have grown by the minimum amount
//  BOOST_CHECK(v1c== v2c);
// Above test fails in VC11, however, think test is probably non-portabl

  // Test capacity heapifying logic
  folly::small_vector<unsigned char, 1> vec3;
  const size_t hc_size = 1000000;
  for (size_t i = 0; i < hc_size; ++i) {
    auto v = (unsigned char)i;
    vec3.push_back(v);
    BOOST_CHECK(vec3[i]== v);
    BOOST_CHECK(vec3.size()== i + 1);
    BOOST_CHECK(vec3.capacity()> i);
  }
  for (auto i = hc_size; i > 0; --i) {
    auto v = (unsigned char)(i - 1);
    BOOST_CHECK(vec3.back()== v);
    vec3.pop_back();
    BOOST_CHECK(vec3.size()== i - 1);
  }
}

 {
  for (int i = 1; i < 33; ++i) {
    folly::small_vector<std::string> vec;
    for (int j = 0; j < i; ++j) {
      vec.push_back("abc");
    }
    BOOST_CHECK(vec.size()== i);
    vec.push_back(std::move(vec[0]));
    BOOST_CHECK(vec.size()== i + 1);

    BOOST_CHECK(vec[i]== "abc");
  }
}

 {
  for (int i = 1; i < 33; ++i) {
    folly::small_vector<std::string> vec;
    for (int j = 0; j < i; ++j) {
      vec.emplace_back("abc");
    }
    BOOST_CHECK(vec.size()== i);
    vec.emplace_back(std::move(vec[0]));
    BOOST_CHECK(vec.size()== i + 1);

    BOOST_CHECK(vec[i]== "abc");
  }
}

 {
  // end insert
  for (int i = 1; i < 33; ++i) {
    folly::small_vector<std::string> vec;
    for (int j = 0; j < i; ++j) {
      vec.push_back("abc");
    }
    BOOST_CHECK(vec.size()== i);
    vec.insert(vec.end(), std::move(vec[0]));
    BOOST_CHECK(vec.size()== i + 1);

    BOOST_CHECK(vec[i]== "abc");
    BOOST_CHECK(vec[vec.size() - 1]== "abc");
  }

  // middle insert
  for (int i = 2; i < 33; ++i) {
    folly::small_vector<std::string> vec;
    for (int j = 0; j < i; ++j) {
      vec.push_back("abc");
    }
    BOOST_CHECK(vec.size()== i);
    vec.insert(vec.end()-1, std::move(vec[0]));
    BOOST_CHECK(vec.size()== i + 1);

    BOOST_CHECK(vec[i-1]== "abc");
    BOOST_CHECK(vec[i]== "abc");
  }
}
 	std::cout << "... Finished testing small_vector \n";

}


int test_main( int argc, char* argv[] ){

	test_small_vector();

	test_fbstring();

	test_fbvector();

	test_dynamic();

	test_json();

	test_format();




	 return 0;

}



