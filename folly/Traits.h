/*
 * Copyright 2012 Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// @author: Andrei Alexandrescu
// Modified 6/10/2012
// By John R. Bandela
// All changes under Apache License 2.0 as above
// Copyright 2012 John R. Bandela
// Released under Apache License 2.0
#ifndef FOLLY_BASE_TRAITS_H_
#define FOLLY_BASE_TRAITS_H_

#include <boost/type_traits.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/not.hpp>
#include <memory>

namespace folly {

/**
 * IsRelocatable<T>::value describes the ability of moving around
 * memory a value of type T by using memcpy (as opposed to the
 * conservative approach of calling the copy constructor and then
 * destroying the old temporary. Essentially for a relocatable type,
 * the following two sequences of code should be semantically
 * equivalent:
 *
 * void move1(T * from, T * to) {
 *   new(to) T(from);
 *   (*from).~T();
 * }
 *
 * void move2(T * from, T * to) {
 *   memcpy(from, to, sizeof(T));
 * }
 *
 * Most C++ types are relocatable; the ones that aren't would include
 * internal pointers or (very rarely) would need to update remote
 * pointers to pointers tracking them. All C++ primitive types and
 * type constructors are relocatable.
 *
 * This property can be used in a variety of optimizations. Currently
 * fbvector uses this property intensively.
 *
 * The default conservatively assumes the type is not
 * relocatable. Several specializations are defined for known
 * types. You may want to add your own specializations. Do so in
 * namespace folly and make sure you keep the specialization of
 * IsRelocatable<SomeStruct> in the same header as SomeStruct.
 */
template <class T> struct IsRelocatable : boost::mpl::not_<boost::is_class<T> >
{};

} // namespace folly

/**
 * Use this macro ONLY inside namespace folly. When using it with a
 * regular type, use it like this:
 *
 * // Make sure you're at namespace ::folly scope
 * template<> FOLLY_ASSUME_RELOCATABLE(MyType)
 *
 * When using it with a template type, use it like this:
 *
 * // Make sure you're at namespace ::folly scope
 * template<class T, class T2>
 * FOLLY_ASSUME_RELOCATABLE(MyType<T1, T2>)
 */
#define FOLLY_ASSUME_RELOCATABLE(...) \
  struct IsRelocatable<  __VA_ARGS__ > : ::boost::true_type {};

/**
 * Use this macro ONLY inside namespace boost. When using it with a
 * regular type, use it like this:
 *
 * // Make sure you're at namespace ::boost scope
 * template<> FOLLY_ASSUME_HAS_NOTHROW_CONSTRUCTOR(MyType)
 *
 * When using it with a template type, use it like this:
 *
 * // Make sure you're at namespace ::boost scope
 * template<class T, class T2>
 * FOLLY_ASSUME_HAS_NOTHROW_CONSTRUCTOR(MyType<T1, T2>)
 */
#define FOLLY_ASSUME_HAS_NOTHROW_CONSTRUCTOR(...) \
  struct has_nothrow_constructor<  __VA_ARGS__ > : ::boost::true_type {};

/**
 * The FOLLY_ASSUME_FBVECTOR_COMPATIBLE* macros below encode two
 * assumptions: first, that the type is relocatable per IsRelocatable
 * above, and that it has a nothrow constructor. Most types can be
 * assumed to satisfy both conditions, but it is the responsibility of
 * the user to state that assumption. User-defined classes will not
 * work with fbvector (see FBVector.h) unless they state this
 * combination of properties.
 *
 * Use FOLLY_ASSUME_FBVECTOR_COMPATIBLE with regular types like this:
 *
 * FOLLY_ASSUME_FBVECTOR_COMPATIBLE(MyType)
 *
 * The versions FOLLY_ASSUME_FBVECTOR_COMPATIBLE_1, _2, _3, and _4
 * allow using the macro for describing templatized classes with 1, 2,
 * 3, and 4 template parameters respectively. For template classes
 * just use the macro with the appropriate number and pass the name of
 * the template to it. Example:
 *
 * template <class T1, class T2> class MyType { ... };
 * ...
 * // Make sure you're at global scope
 * FOLLY_ASSUME_FBVECTOR_COMPATIBLE_2(MyType)
 */

// Use this macro ONLY at global level (no namespace)
#define FOLLY_ASSUME_FBVECTOR_COMPATIBLE(...)                           \
  namespace folly { template<> FOLLY_ASSUME_RELOCATABLE(__VA_ARGS__) }   \
  namespace boost { \
  template<> FOLLY_ASSUME_HAS_NOTHROW_CONSTRUCTOR(__VA_ARGS__) }
// Use this macro ONLY at global level (no namespace)
#define FOLLY_ASSUME_FBVECTOR_COMPATIBLE_1(...)                         \
  namespace folly {                                                     \
  template <class T1> FOLLY_ASSUME_RELOCATABLE(__VA_ARGS__<T1>) }       \
    namespace boost {                                                   \
    template <class T1> FOLLY_ASSUME_HAS_NOTHROW_CONSTRUCTOR(__VA_ARGS__<T1>) }
// Use this macro ONLY at global level (no namespace)
#define FOLLY_ASSUME_FBVECTOR_COMPATIBLE_2(...)                 \
  namespace folly {                                             \
  template <class T1, class T2>                                 \
  FOLLY_ASSUME_RELOCATABLE(__VA_ARGS__<T1, T2>) }               \
    namespace boost {                                           \
    template <class T1, class T2>                               \
    FOLLY_ASSUME_HAS_NOTHROW_CONSTRUCTOR(__VA_ARGS__<T1, T2>) }
// Use this macro ONLY at global level (no namespace)
#define FOLLY_ASSUME_FBVECTOR_COMPATIBLE_3(...)                         \
  namespace folly {                                                     \
  template <class T1, class T2, class T3>                               \
  FOLLY_ASSUME_RELOCATABLE(__VA_ARGS__<T1, T2, T3>) }                   \
    namespace boost {                                                   \
    template <class T1, class T2, class T3>                             \
    FOLLY_ASSUME_HAS_NOTHROW_CONSTRUCTOR(__VA_ARGS__<T1, T2, T3>) }
// Use this macro ONLY at global level (no namespace)
#define FOLLY_ASSUME_FBVECTOR_COMPATIBLE_4(...)                         \
  namespace folly {                                                     \
  template <class T1, class T2, class T3, class T4>                     \
  FOLLY_ASSUME_RELOCATABLE(__VA_ARGS__<T1, T2, T3, T4>) }               \
    namespace boost {                                                   \
    template <class T1, class T2, class T3, class T4>                   \
    FOLLY_ASSUME_HAS_NOTHROW_CONSTRUCTOR(__VA_ARGS__<T1, T2, T3, T4>) }

/**
 * Instantiate FOLLY_ASSUME_FBVECTOR_COMPATIBLE for a few types. It is
 * safe to assume that pair is compatible if both of its components
 * are. Furthermore, all STL containers can be assumed to comply,
 * although that is not guaranteed by the standard.
 */
#include <functional>

//namespace std {
//
//template <class T, class U>
//  struct pair;
//#ifndef _GLIBCXX_USE_FB
//template <class T, class R, class A>
//  class basic_string;
//#else
//template <class T, class R, class A, class S>
//  class basic_string;
//#endif
//template <class T, class A>
//  class vector;
//template <class T, class A>
//  class deque;
//template <class T, class A>
//  class list;
//template <class T, class C, class A>
//  class set;
//template <class K, class V, class C, class A>
//  class map;
//template <class T>
//  class shared_ptr;
//
//}
// Instead of forward declaring classes in namespace std include the files
#include<utility>
#include <string>
#include<vector>
#include<deque>
#include<list>
#include<set>
#include<map>
#include<memory>

namespace boost {

template <class T> class shared_ptr;

template <class T, class U>
struct has_nothrow_constructor< std::pair<T, U> >
    : ::boost::mpl::and_< has_nothrow_constructor<T>,
                          has_nothrow_constructor<U> > {};

} // namespace boost

namespace folly {

// STL commonly-used types
template <class T, class U>
struct IsRelocatable<  std::pair<T, U> >
    : ::boost::mpl::and_< IsRelocatable<T>, IsRelocatable<U> > {};

// Is T one of T1, T2, ..., Tn?
// Modified without variable templates
struct jrb_null_type{};
template <class T, class T1, class T2 = jrb_null_type, class T3 = jrb_null_type, class T4 = jrb_null_type, class T5 = jrb_null_type, class T6 = jrb_null_type, class T7 = jrb_null_type>
struct IsOneOf {
  enum { value = std::is_same<T, T1>::value || std::is_same<T, T2>::value|| std::is_same<T, T3>::value|| std::is_same<T, T4>::value|| std::is_same<T, T5>::value|| std::is_same<T, T6>::value|| std::is_same<T, T7>::value };;
};

//template <class T, class T1, class... Ts>
//struct IsOneOf<T, T1, Ts...> {
//  enum { value = std::is_same<T, T1>::value || IsOneOf<T, Ts...>::value };
//};

} // namespace folly

FOLLY_ASSUME_FBVECTOR_COMPATIBLE_3(std::basic_string);
FOLLY_ASSUME_FBVECTOR_COMPATIBLE_2(std::vector);
FOLLY_ASSUME_FBVECTOR_COMPATIBLE_2(std::list);
FOLLY_ASSUME_FBVECTOR_COMPATIBLE_2(std::deque);
FOLLY_ASSUME_FBVECTOR_COMPATIBLE_4(std::map);
FOLLY_ASSUME_FBVECTOR_COMPATIBLE_3(std::set);
FOLLY_ASSUME_FBVECTOR_COMPATIBLE_2(std::unique_ptr);
FOLLY_ASSUME_FBVECTOR_COMPATIBLE_1(std::shared_ptr);
FOLLY_ASSUME_FBVECTOR_COMPATIBLE_1(std::function);

// Boost
FOLLY_ASSUME_FBVECTOR_COMPATIBLE_1(boost::shared_ptr);

#endif //FOLLY_BASE_TRAITS_H_
