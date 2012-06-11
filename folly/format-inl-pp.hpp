// Modified 6/10/2012
// By John R. Bandela
// All changes under Apache License 2.0 as above
// Copyright 2012 John R. Bandela
// Released under Apache License 2.0

// Functions at end of format-inl.hpp
// Changed to use boost preprocessor
#include <boost/preprocessor.hpp>

#ifndef NEW_MAX_ARGUMENTS_NUMBER
# define NEW_MAX_ARGUMENTS_NUMBER 10
#endif
#define N BOOST_PP_ITERATION()

// Partial specialization of FormatValue for tuples
template <BOOST_PP_ENUM_PARAMS(N,class Arg)>
class FormatValue<std::tuple<BOOST_PP_ENUM_PARAMS(N,Arg)> > {
  typedef std::tuple<BOOST_PP_ENUM_PARAMS(N,Arg)> Tuple;
 public:
  explicit FormatValue(const Tuple& val) : val_(val) { }

  template <class FormatCallback>
  void format(FormatArg& arg, FormatCallback& cb) const {
    int key = arg.splitIntKey();
    arg.enforce(key >= 0, "tuple index must be non-negative");
    doFormat(key, arg, cb);
  }

 private:
  static const size_t valueCount = std::tuple_size<Tuple>::value;

  template <size_t K, class Callback>
  typename std::enable_if<K == valueCount>::type
  doFormatFrom(size_t i, FormatArg& arg, Callback& cb) const {
    arg.enforce("tuple index out of range, max=", i);
  }

  template <size_t K, class Callback>
  typename std::enable_if<(K < valueCount)>::type
  doFormatFrom(size_t i, FormatArg& arg, Callback& cb) const {
    if (i == K) {
      FormatValue<typename std::decay<
        typename std::tuple_element<K, Tuple>::type>::type>(
          std::get<K>(val_)).format(arg, cb);
    } else {
      doFormatFrom<K+1>(i, arg, cb);
    }
  }

  template <class Callback>
  void doFormat(size_t i, FormatArg& arg, Callback& cb) const {
    return doFormatFrom<0>(i, arg, cb);
  }

  const Tuple& val_;
};

/**
 * Formatter objects can be appended to strings, and therefore they're
 * compatible with folly::toAppend and folly::to.
 */
template <class Tgt, bool containerMode BOOST_PP_ENUM_TRAILING_PARAMS(N,class Arg)>
typename std::enable_if<
   detail::IsSomeString<Tgt>::value>::type
toAppend(const BOOST_PP_CAT(Formatter,N)<containerMode BOOST_PP_ENUM_TRAILING_PARAMS(N,Arg)>& value, Tgt * result) {
  value.appendTo(*result);
}


#undef N