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
  return fmt.toString(); 
}

std::string vstr(StringPiece fmt) { 
  return fmt.toString(); 
}
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
}

#define EXPECT_ANY_THROW(e)try{e; BOOST_CHECK(false);}catch(...){}
#define EXPECT_THROW(e,ex)try{e; BOOST_CHECK(false);}catch(ex&){}catch(...){BOOST_CHECK(false);}


void test_dynamic(){
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


}



void test_json(){
	using folly::dynamic;
using folly::parseJson;
using folly::toJson;

 {
  auto val = parseJson("\"I \u2665 UTF-8\"");
  BOOST_CHECK("I \u2665 UTF-8"== val.asString());
  val = parseJson("\"I \\u2665 UTF-8\"");
  BOOST_CHECK("I \u2665 UTF-8"== val.asString());
  val = parseJson("\"I \U0001D11E playing in G-clef\"");
  BOOST_CHECK("I \U0001D11E playing in G-clef"== val.asString());

  val = parseJson("\"I \\uD834\\uDD1E playing in G-clef\"");
  BOOST_CHECK("I \U0001D11E playing in G-clef"== val.asString());
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
  EXPECT_ANY_THROW(folly::json::serialize(badDouble, opts));

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
}

void check_format(){
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

}
int test_main( int argc, char* argv[] ){


	test_fbvector();
	test_dynamic();
	test_json();

	std::cout << "Tests finished running... enter number\n";


	 int i;
	 std::cin >> i;

	 return 0;

}



