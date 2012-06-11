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
// Modified 6/10/2012
// By John R. Bandela
// All changes under Apache License 2.0 as above
// Released under Apache License 2.0
#ifndef FOLLY_FORMATARG_H_
#define FOLLY_FORMATARG_H_

#include <stdexcept>
#include "folly/Range.h"
#include "folly/Likely.h"
#include "folly/Conv.h"
#include <boost/preprocessor.hpp>
#define JRB_FORWARD(z,n,data) std::forward<BOOST_PP_CAT(Arg,n)>(BOOST_PP_CAT(arg,n))



namespace folly {

/**
 * Parsed format argument.
 */
struct FormatArg {
  /**
   * Parse a format argument from a string.  Keeps a reference to the
   * passed-in string -- does not copy the given characters.
   */
  explicit FormatArg(StringPiece sp)
    : fullArgString(sp),
      fill(kDefaultFill),
      align(Align::DEFAULT),
      sign(Sign::DEFAULT),
      basePrefix(false),
      thousandsSeparator(false),
      width(kDefaultWidth),
      precision(kDefaultPrecision),
      presentation(kDefaultPresentation),
      nextKeyMode_(NextKeyMode::NONE) {
    if (!sp.empty()) {
      initSlow();
    }
  }

  enum class Type {
    INTEGER,
    FLOAT,
    OTHER
  };
  /**
   * Validate the argument for the given type; throws on error.
   */
  void validate(Type type) const;

  /**
   * Throw an exception if the first argument is false.  The exception
   * message will contain the argument string as well as any passed-in
   * arguments to enforce, formatted using folly::to<std::string>.
   */
  //template <typename... Args>
  //void enforce(bool v, Args&&... args) const {
  //  if (UNLIKELY(!v)) {
  //    error(std::forward<Args>(args)...);
  //  }
  //}
  #define BOOST_PP_LOCAL_MACRO(N) \
     template <BOOST_PP_ENUM_PARAMS(N,class Arg)> \
  void enforce(bool v BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N, Arg, &&arg ) ) const { \
    if (UNLIKELY(!v)) { \
      error(BOOST_PP_ENUM(N,JRB_FORWARD,unused) ); \
    } \
  }
   /**/

#define BOOST_PP_LOCAL_LIMITS (1, 5)

#include BOOST_PP_LOCAL_ITERATE()

#undef BOOST_PP_LOCAL_MACRO
#undef BOOST_PP_LOCAL_LIMITS


  //template <typename... Args>
  //void  __declspec(noreturn) error(Args&&... args) const;
    #define BOOST_PP_LOCAL_MACRO(N) \
	template <BOOST_PP_ENUM_PARAMS(N, class Arg)> \
  void  __declspec(noreturn) error(BOOST_PP_ENUM_BINARY_PARAMS(N, Arg, &&arg)) const; 
   /**/

#define BOOST_PP_LOCAL_LIMITS (1, 5)

#include BOOST_PP_LOCAL_ITERATE()

#undef BOOST_PP_LOCAL_MACRO
#undef BOOST_PP_LOCAL_LIMITS


  /**
   * Full argument string, as passed in to the constructor.
   */
  StringPiece fullArgString;

  /**
   * Fill
   */
  static const char kDefaultFill = '\0';
  char fill;

  /**
   * Alignment
   */
  enum class Align : uint8_t {
    DEFAULT,
    LEFT,
    RIGHT,
    PAD_AFTER_SIGN,
    CENTER,
    INVALID
  };
  Align align;

  /**
   * Sign
   */
  enum class Sign : uint8_t {
    DEFAULT,
    PLUS_OR_MINUS,
    MINUS,
    SPACE_OR_MINUS,
    INVALID
  };
  Sign sign;

  /**
   * Output base prefix (0 for octal, 0x for hex)
   */
  bool basePrefix;

  /**
   * Output thousands separator (comma)
   */
  bool thousandsSeparator;

  /**
   * Field width
   */
  static const int kDefaultWidth = -1;
  int width;

  /**
   * Precision
   */
  static const int kDefaultPrecision = -1;
  int precision;

  /**
   * Presentation
   */
  static const char kDefaultPresentation = '\0';
  char presentation;

  /**
   * Split a key component from "key", which must be non-empty (an exception
   * is thrown otherwise).
   */
  template <bool emptyOk/*=false*/>
  StringPiece splitKey();

  StringPiece splitKey(){return splitKey<false>();}


  

  /**
   * Is the entire key empty?
   */
  bool keyEmpty() const {
    return nextKeyMode_ == NextKeyMode::NONE && key_.empty();
  }

  /**
   * Split an key component from "key", which must be non-empty and a valid
   * integer (an exception is thrown otherwise).
   */
  int splitIntKey();

  void setNextIntKey(int val) {
    assert(nextKeyMode_ == NextKeyMode::NONE);
    nextKeyMode_ = NextKeyMode::INT;
    nextIntKey_ = val;
  }

  void setNextKey(StringPiece val) {
    assert(nextKeyMode_ == NextKeyMode::NONE);
    nextKeyMode_ = NextKeyMode::STRING;
    nextKey_ = val;
  }

 private:
  void initSlow();
  template <bool emptyOk>
  StringPiece doSplitKey();

  StringPiece key_;
  int nextIntKey_;
  StringPiece nextKey_;
  enum class NextKeyMode {
    NONE,
    INT,
    STRING,
  };
  NextKeyMode nextKeyMode_;
};

//template <typename... Args>
//inline void FormatArg::error(Args&&... args) const {
//  throw std::invalid_argument(to<std::string>(
//      "folly::format: invalid format argument {", fullArgString, "}: ",
//      std::forward<Args>(args)...));
//}

    #define BOOST_PP_LOCAL_MACRO(N) \
template <BOOST_PP_ENUM_PARAMS(N,typename Arg)> \
inline void FormatArg::error(BOOST_PP_ENUM_BINARY_PARAMS(N,Arg,&&arg)) const { \
  throw std::invalid_argument(to<std::string>( \
      "folly::format: invalid format argument {", fullArgString, "}: ", \
      BOOST_PP_ENUM(N,JRB_FORWARD,unused))); \
}
   /**/

#define BOOST_PP_LOCAL_LIMITS (1, 5)

#include BOOST_PP_LOCAL_ITERATE()

#undef BOOST_PP_LOCAL_MACRO
#undef BOOST_PP_LOCAL_LIMITS

template <bool emptyOk>
inline StringPiece FormatArg::splitKey() {
  enforce(nextKeyMode_ != NextKeyMode::INT, "integer key expected");
  return doSplitKey<emptyOk>();
}

template <bool emptyOk>
inline StringPiece FormatArg::doSplitKey() {
  if (nextKeyMode_ == NextKeyMode::STRING) {
    nextKeyMode_ = NextKeyMode::NONE;
    if (!emptyOk) {  // static
      enforce(!nextKey_.empty(), "non-empty key required");
    }
    return nextKey_;
  }

  if (key_.empty()) {
    if (!emptyOk) {  // static
      error("non-empty key required");
    }
    return StringPiece();
  }

  const char* b = key_.begin();
  const char* e = key_.end();
  const char* p;
  if (e[-1] == ']') {
    --e;
    p = static_cast<const char*>(memchr(b, '[', e - b));
    enforce(p, "unmatched ']'");
  } else {
    p = static_cast<const char*>(memchr(b, '.', e - b));
  }
  if (p) {
    key_.assign(p + 1, e);
  } else {
    p = e;
    key_.clear();
  }
  if (!emptyOk) {  // static
    enforce(b != p, "non-empty key required");
  }
  return StringPiece(b, p);
}

inline int FormatArg::splitIntKey() {
  if (nextKeyMode_ == NextKeyMode::INT) {
    nextKeyMode_ = NextKeyMode::NONE;
    return nextIntKey_;
  }
  try {
    return to<int>(doSplitKey<true>());
  } catch (const std::out_of_range& e) {
    error("integer key required");
    return 0;  // unreached
  }
}

}  // namespace folly

#endif /* FOLLY_FORMATARG_H_ */

#undef JRB_FORWARD
