// Modified 6/10/2012
// By John R. Bandela
// All changes under Apache License 2.0 as above
// Copyright 2012 John R. Bandela
// Released under Apache License 2.0

// Functions at in Format.h and included files and format-inl.hpp
// Changed to use boost preprocessor
#include <boost/preprocessor.hpp>

#ifndef NEW_MAX_ARGUMENTS_NUMBER
# define NEW_MAX_ARGUMENTS_NUMBER 10
#endif
#define N BOOST_PP_ITERATION()

#define JRB_FORMATVALUES(z,n,data)  FormatValue< typename std::decay<BOOST_PP_CAT(Arg,n)>::type >

#define JRB_FORWARD(z,n,data) std::forward<BOOST_PP_CAT(Arg,n)>(BOOST_PP_CAT(arg,n))

#define JRB_FORMATVALUES_CONST(z,n,data) FormatValue<typename std::decay<BOOST_PP_CAT(Arg,n)>::type>(std::forward<BOOST_PP_CAT(Arg,n)>(BOOST_PP_CAT(arg,n)))

	template <bool containerMode BOOST_PP_ENUM_TRAILING_PARAMS(N, class Arg)>  
class BOOST_PP_CAT(Formatter,N) {  
  template <BOOST_PP_ENUM_PARAMS(N, class A)>  
  friend BOOST_PP_CAT(Formatter,N)<false, BOOST_PP_ENUM_PARAMS(N, A)> format(StringPiece fmt, BOOST_PP_ENUM_BINARY_PARAMS(N, A, &&a));  
  template <class C>  
  friend Formatter1<true, C> vformat(StringPiece fmt, C&& container);  
 public:  
  /**  
   * Append to output.  out(StringPiece sp) may be called (more than once)
   */  
  template <class Output>  
  void operator()(Output& out) const;  
   
  /**
   * Append to a string.
   */  
  template <class Str>  
  typename std::enable_if<detail::IsSomeString<Str>::value>::type  
  appendTo(Str& str) const {  
    auto appender = [&str] (StringPiece s) { str.append(s.data(), s.size()); };  
    (*this)(appender);  
  }  
   
  /**
   * Conversion to string
   */ 
  std::string str() const { 
    std::string s; 
    appendTo(s); 
    return s; 
  } 
   
  /**
   * Conversion to fbstring
   */ 
  fbstring fbstr() const { 
    fbstring s; 
    appendTo(s); 
    return s; 
  } 
   
 private: 
		 explicit BOOST_PP_CAT(Formatter,N)(StringPiece str, BOOST_PP_ENUM_BINARY_PARAMS(N,Arg,&&arg)); 
		  
  /* Not copyable */  
  BOOST_PP_CAT(Formatter,N)(const BOOST_PP_CAT(Formatter,N)&); 
  BOOST_PP_CAT(Formatter,N)& operator=(const BOOST_PP_CAT(Formatter,N)&); 
  /*
  // Movable, but the move constructor and assignment operator are private,
  // for the exclusive use of format() (below).  This way, you can't create
  // a Formatter object, but can handle references to it (for streaming,
  // conversion to string, etc) -- which is good, as Formatter objects are
  // dangerous (they hold references, possibly to temporaries) */  
  BOOST_PP_CAT(Formatter,N)(BOOST_PP_CAT(Formatter,N)&& other){containerMode_ = std::move(other.containerMode_);str = std::move(other.str);values_ = std::move(other.values_);}    
  BOOST_PP_CAT(Formatter,N)& operator=(BOOST_PP_CAT(Formatter,N)&& other){containerMode_ = std::move(other.containerMode_);str = std::move(other.str);values_ = std::move(other.values_);}  
   
  /*typedef std::tuple<FormatValue<  
      typename std::decay<Args>::type>...> ValueTuple;*/  
  typedef std::tuple<BOOST_PP_ENUM(N,JRB_FORMATVALUES,unused)> ValueTuple;

  static const size_t valueCount = N;  
   
  template <size_t K, class Callback> 
  typename std::enable_if<K == valueCount>::type 
  doFormatFrom(size_t i, FormatArg& arg, Callback& cb) const { 
    arg.error("argument index out of range, max=", i); 
  } 
   
  template <size_t K, class Callback> 
  typename std::enable_if<(K < valueCount)>::type 
  doFormatFrom(size_t i, FormatArg& arg, Callback& cb) const { 
    if (i == K) { 
      std::get<K>(values_).format(arg, cb); 
    } else { 
      doFormatFrom<K+1>(i, arg, cb); 
    } 
  } 
   
  template <class Callback> 
  void doFormat(size_t i, FormatArg& arg, Callback& cb) const { 
    return doFormatFrom<0>(i, arg, cb); 
  } 
   
  bool containerMode_; 
  StringPiece str_; 
  ValueTuple values_; 
};



 /* Formatter objects can be written to streams.
 */
//template<bool containerMode, class... Args>
//std::ostream& operator<<(std::ostream& out,
//                         const Formatter<containerMode, Args...>& formatter) {
//  auto writer = [&out] (StringPiece sp) { out.write(sp.data(), sp.size()); };
//  formatter(writer);
//  return out;
//}

template<bool containerMode BOOST_PP_ENUM_TRAILING_PARAMS(N,class Arg)>
std::ostream& operator<<(std::ostream& out,
                         const BOOST_PP_CAT(Formatter,N)<containerMode BOOST_PP_ENUM_TRAILING_PARAMS(N, Arg)>& formatter) {
  auto writer = [&out] (StringPiece sp) { out.write(sp.data(), sp.size()); };
  formatter(writer);
  return out;
}

/**
 * Create a formatter object.
 *
 * std::string formatted = format("{} {}", 23, 42);
 * LOG(INFO) << format("{} {}", 23, 42);
 */
template <BOOST_PP_ENUM_PARAMS(N,class Arg)>
BOOST_PP_CAT(Formatter,N)<false BOOST_PP_ENUM_TRAILING_PARAMS(N,Arg)> format(StringPiece fmt BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N,Arg, &&arg)) {
  return BOOST_PP_CAT(Formatter,N)<false BOOST_PP_ENUM_TRAILING_PARAMS(N, Arg)>(
      fmt BOOST_PP_ENUM_TRAILING(N,JRB_FORWARD,unused));
}


/**
 * Append formatted output to a string.
 *
 * std::string foo;
 * format(&foo, "{} {}", 42, 23);
 *
 * Shortcut for toAppend(format(...), &foo);
 */
template <class Str BOOST_PP_ENUM_TRAILING_PARAMS(N,class Arg)>
typename std::enable_if<detail::IsSomeString<Str>::value>::type
format(Str* out, StringPiece fmt BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N,Arg, &&arg)) {
  format(fmt BOOST_PP_ENUM_TRAILING(N,JRB_FORWARD,unused)).appendTo(*out);
}


namespace format_value{
/**
 * Format a Formatter object recursively.  Behaves just like
 * formatString(fmt.str(), arg, cb); but avoids creating a temporary
 * string if possible.
 */
//template <class FormatCallback, bool containerMode, class... Args>
//void formatFormatter(const Formatter<containerMode, Args...>& formatter,
//                     FormatArg& arg,
//                     FormatCallback& cb);


template <class FormatCallback, bool containerMode BOOST_PP_ENUM_TRAILING_PARAMS(N,class Arg)>
void formatFormatter(const BOOST_PP_CAT(Formatter,N)<containerMode BOOST_PP_ENUM_TRAILING_PARAMS(N,Arg)>& formatter,
                     FormatArg& arg,
                     FormatCallback& cb);
template <class FormatCallback, bool containerMode BOOST_PP_ENUM_TRAILING_PARAMS(N, class Arg)>
void formatFormatter(const BOOST_PP_CAT(Formatter,N)<containerMode BOOST_PP_ENUM_TRAILING_PARAMS(N , Arg)>& formatter,
                     FormatArg& arg,
                     FormatCallback& cb) {
  if (arg.width == FormatArg::kDefaultWidth &&
      arg.precision == FormatArg::kDefaultPrecision) {
    // nothing to do
    formatter(cb);
  } else if (arg.align != FormatArg::Align::LEFT &&
             arg.align != FormatArg::Align::DEFAULT) {
    // We can only avoid creating a temporary string if we align left,
    // as we'd need to know the size beforehand otherwise
    format_value::formatString(formatter.fbstr(), arg, cb);
  } else {
    auto fn = [&arg, &cb] (StringPiece sp) mutable {
      int sz = static_cast<int>(sp.size());
      if (arg.precision != FormatArg::kDefaultPrecision) {
        sz = std::min(arg.precision, sz);
        sp.reset(sp.data(), sz);
        arg.precision -= sz;
      }
      if (!sp.empty()) {
        cb(sp);
        if (arg.width != FormatArg::kDefaultWidth) {
          arg.width = std::max(arg.width - sz, 0);
        }
      }
    };
    formatter(fn);
    if (arg.width != FormatArg::kDefaultWidth && arg.width != 0) {
      // Rely on formatString to do appropriate padding
      format_value::formatString(StringPiece(), arg, cb);
    }
  }
}



}  // namespace format_value


template <bool containerMode BOOST_PP_ENUM_TRAILING_PARAMS(N,class Arg)>
BOOST_PP_CAT(Formatter,N)<containerMode BOOST_PP_ENUM_TRAILING_PARAMS(N, Arg)>::BOOST_PP_CAT(Formatter,N)(StringPiece str BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N,Arg,&&arg))
  : str_(str),
    /*values_(FormatValue<typename std::decay<Args>::type>(
        std::forward<Args>(args))...)*/values_(BOOST_PP_ENUM(N,JRB_FORMATVALUES_CONST,unused)) {
			//static_assert(containerMode==false,"container mode is true");
  static_assert(!containerMode || N == 1,
                "Exactly one argument required in container mode");
 // assert(containerMode==false);
}

template <bool containerMode BOOST_PP_ENUM_TRAILING_PARAMS(N,class Arg)>
template <class Output>
void BOOST_PP_CAT(Formatter,N)<containerMode BOOST_PP_ENUM_TRAILING_PARAMS(N,Arg)>::operator()(Output& out) const {
  auto p = str_.begin();
  auto end = str_.end();

  // Copy raw string (without format specifiers) to output;
  // not as simple as we'd like, as we still need to translate "}}" to "}"
  // and throw if we see any lone "}"
  auto outputString = [&out] (StringPiece s) {
    auto p = s.begin();
    auto end = s.end();
    while (p != end) {
      auto q = static_cast<const char*>(memchr(p, '}', end - p));
      if (!q) {
        out(StringPiece(p, end));
        break;
      }
      ++q;
      out(StringPiece(p, q));
      p = q;

      if (p == end || *p != '}') {
        throw std::invalid_argument(
            "folly::format: single '}' in format string");
      }
      ++p;
    }
  };

  int nextArg = 0;
  bool hasDefaultArgIndex = false;
  bool hasExplicitArgIndex = false;
  while (p != end) {
    auto q = static_cast<const char*>(memchr(p, '{', end - p));
    if (!q) {
      outputString(StringPiece(p, end));
      break;
    }
    outputString(StringPiece(p, q));
    p = q + 1;

    if (p == end) {
      throw std::invalid_argument(
          "folly::format: '}' at end of format string");
    }

    // "{{" -> "{"
    if (*p == '{') {
      out(StringPiece(p, 1));
      ++p;
      continue;
    }

    // Format string
    q = static_cast<const char*>(memchr(p, '}', end - p));
    if (q == end) {
      throw std::invalid_argument("folly::format: missing ending '}'");
    }
    FormatArg arg(StringPiece(p, q));
    p = q + 1;

    int argIndex = 0;
    auto piece = arg.splitKey<true>();  // empty key component is okay
    if (containerMode) {  // static
      if (piece.empty()) {
        arg.setNextIntKey(nextArg++);
        hasDefaultArgIndex = true;
      } else {
        arg.setNextKey(piece);
        hasExplicitArgIndex = true;
      }
    } else {
      if (piece.empty()) {
        argIndex = nextArg++;
        hasDefaultArgIndex = true;
      } else {
        try {
          argIndex = to<int>(piece);
        } catch (const std::out_of_range& e) {
          arg.error("argument index must be integer");
        }
        arg.enforce(argIndex >= 0, "argument index must be non-negative");
        hasExplicitArgIndex = true;
      }
    }

    if (hasDefaultArgIndex && hasExplicitArgIndex) {
      throw std::invalid_argument(
          "folly::format: may not have both default and explicit arg indexes");
    }

    doFormat(argIndex, arg, out);
  }
}



#undef N
#undef JRB_FORMATVALUES
#undef JRB_FORWARD
#undef JRB_FORMATVALUES_CONST




