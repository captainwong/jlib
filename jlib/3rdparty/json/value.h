// Copyright 2007-2010 Baptiste Lepilleur
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#ifndef CPPTL_JSON_H_INCLUDED
#define CPPTL_JSON_H_INCLUDED

#if !defined(JSON_IS_AMALGAMATION)
#include "forwards.h"
#endif // if !defined(JSON_IS_AMALGAMATION)
#include <string>
#include <vector>
#include <exception>

#ifndef JSON_USE_CPPTL_SMALLMAP
#include <map>
#else
#include <cpptl/smallmap.h>
#endif
#ifdef JSON_USE_CPPTL
#include <cpptl/forwards.h>
#endif

//Conditional NORETURN attribute on the throw functions would:
// a) suppress false positives from static code analysis 
// b) possibly improve optimization opportunities.
#if !defined(JSONCPP_NORETURN)
#  if defined(_MSC_VER)
#    define JSONCPP_NORETURN __declspec(noreturn)
#  elif defined(__GNUC__)
#    define JSONCPP_NORETURN __attribute__ ((__noreturn__))
#  else
#    define JSONCPP_NORETURN
#  endif
#endif

// Disable warning C4251: <data member>: <type> needs to have dll-interface to
// be used by...
#if defined(JSONCPP_DISABLE_DLL_INTERFACE_WARNING)
#pragma warning(push)
#pragma warning(disable : 4251)
#endif // if defined(JSONCPP_DISABLE_DLL_INTERFACE_WARNING)

#pragma pack(push, 8)

/** \brief JSON (JavaScript Object Notation).
 */
namespace Json {

/** Base class for all exceptions we throw.
 *
 * We use nothing but these internally. Of course, STL can throw others.
 */
class JSON_API Exception : public std::exception {
public:
  Exception(JSONCPP_STRING const& msg)
	  : msg_(msg)
  {}
  ~Exception() JSONCPP_NOEXCEPT JSONCPP_OVERRIDE {}
  char const* what() const JSONCPP_NOEXCEPT JSONCPP_OVERRIDE {
	  return msg_.c_str();
  }
protected:
  JSONCPP_STRING msg_;
};

/** Exceptions which the user cannot easily avoid.
 *
 * E.g. out-of-memory (when we use malloc), stack-overflow, malicious input
 * 
 * \remark derived from Json::Exception
 */
class JSON_API RuntimeError : public Exception {
public:
  RuntimeError(JSONCPP_STRING const& msg) : Exception(msg)
  {}
};

/** Exceptions thrown by JSON_ASSERT/JSON_FAIL macros.
 *
 * These are precondition-violations (user bugs) and internal errors (our bugs).
 * 
 * \remark derived from Json::Exception
 */
class JSON_API LogicError : public Exception {
public:
  LogicError(JSONCPP_STRING const& msg) : Exception(msg)
  {}
};

/// used internally
JSONCPP_NORETURN void throwRuntimeError(JSONCPP_STRING const& msg) {
	throw RuntimeError(msg);
}
/// used internally
JSONCPP_NORETURN void throwLogicError(JSONCPP_STRING const& msg) {
	throw LogicError(msg);
}

/** \brief Type of the value held by a Value object.
 */
enum ValueType {
  nullValue = 0, ///< 'null' value
  intValue,      ///< signed integer value
  uintValue,     ///< unsigned integer value
  realValue,     ///< double value
  stringValue,   ///< UTF-8 string value
  booleanValue,  ///< bool value
  arrayValue,    ///< array value (ordered list)
  objectValue    ///< object value (collection of name/value pairs).
};

enum CommentPlacement {
  commentBefore = 0,      ///< a comment placed on the line before a value
  commentAfterOnSameLine, ///< a comment just after a value on the same line
  commentAfter, ///< a comment on the line after a value (only make sense for
  /// root value)
  numberOfCommentPlacement
};

/** Duplicates the specified string value.
 * @param value Pointer to the string to duplicate. Must be zero-terminated if
 *              length is "unknown".
 * @param length Length of the value. if equals to unknown, then it will be
 *               computed using strlen(value).
 * @return Pointer on the duplicate instance of string.
 */
static inline char* duplicateStringValue(const char* value,
										 size_t length)
{
	// Avoid an integer overflow in the call to malloc below by limiting length
	// to a sane value.
	if (length >= static_cast<size_t>(Value::maxInt))
		length = Value::maxInt - 1;

	char* newString = static_cast<char*>(malloc(length + 1));
	if (newString == NULL) {
		throwRuntimeError(
			"in Json::Value::duplicateStringValue(): "
			"Failed to allocate string value buffer");
	}
	memcpy(newString, value, length);
	newString[length] = 0;
	return newString;
}

/* Record the length as a prefix.
 */
static inline char* duplicateAndPrefixStringValue(
	const char* value,
	unsigned int length)
{
	// Avoid an integer overflow in the call to malloc below by limiting length
	// to a sane value.
	JSON_ASSERT_MESSAGE(length <= static_cast<unsigned>(Value::maxInt) - sizeof(unsigned) - 1U,
						"in Json::Value::duplicateAndPrefixStringValue(): "
						"length too big for prefixing");
	unsigned actualLength = length + static_cast<unsigned>(sizeof(unsigned)) + 1U;
	char* newString = static_cast<char*>(malloc(actualLength));
	if (newString == 0) {
		throwRuntimeError(
			"in Json::Value::duplicateAndPrefixStringValue(): "
			"Failed to allocate string value buffer");
	}
	*reinterpret_cast<unsigned*>(newString) = length;
	memcpy(newString + sizeof(unsigned), value, length);
	newString[actualLength - 1U] = 0; // to avoid buffer over-run accidents by users later
	return newString;
}
inline static void decodePrefixedString(
	bool isPrefixed, char const* prefixed,
	unsigned* length, char const** value)
{
	if (!isPrefixed) {
		*length = static_cast<unsigned>(strlen(prefixed));
		*value = prefixed;
	} else {
		*length = *reinterpret_cast<unsigned const*>(prefixed);
		*value = prefixed + sizeof(unsigned);
	}
}
/** Free the string duplicated by duplicateStringValue()/duplicateAndPrefixStringValue().
 */
#if JSONCPP_USING_SECURE_MEMORY
static inline void releasePrefixedStringValue(char* value) {
	unsigned length = 0;
	char const* valueDecoded;
	decodePrefixedString(true, value, &length, &valueDecoded);
	size_t const size = sizeof(unsigned) + length + 1U;
	memset(value, 0, size);
	free(value);
}
static inline void releaseStringValue(char* value, unsigned length) {
	// length==0 => we allocated the strings memory
	size_t size = (length == 0) ? strlen(value) : length;
	memset(value, 0, size);
	free(value);
}
#else // !JSONCPP_USING_SECURE_MEMORY
static inline void releasePrefixedStringValue(char* value) {
	free(value);
}
static inline void releaseStringValue(char* value, unsigned) {
	free(value);
}
#endif // JSONCPP_USING_SECURE_MEMORY


//# ifdef JSON_USE_CPPTL
//   typedef CppTL::AnyEnumerator<const char *> EnumMemberNames;
//   typedef CppTL::AnyEnumerator<const Value &> EnumValues;
//# endif

/** \brief Lightweight wrapper to tag static string.
 *
 * Value constructor and objectValue member assignement takes advantage of the
 * StaticString and avoid the cost of string duplication when storing the
 * string or the member name.
 *
 * Example of usage:
 * \code
 * Json::Value aValue( StaticString("some text") );
 * Json::Value object;
 * static const StaticString code("code");
 * object[code] = 1234;
 * \endcode
 */
class JSON_API StaticString {
public:
  explicit StaticString(const char* czstring) : c_str_(czstring) {}

  operator const char*() const { return c_str_; }

  const char* c_str() const { return c_str_; }

private:
  const char* c_str_;
};

/** \brief Represents a <a HREF="http://www.json.org">JSON</a> value.
 *
 * This class is a discriminated union wrapper that can represents a:
 * - signed integer [range: Value::minInt - Value::maxInt]
 * - unsigned integer (range: 0 - Value::maxUInt)
 * - double
 * - UTF-8 string
 * - boolean
 * - 'null'
 * - an ordered list of Value
 * - collection of name/value pairs (javascript object)
 *
 * The type of the held value is represented by a #ValueType and
 * can be obtained using type().
 *
 * Values of an #objectValue or #arrayValue can be accessed using operator[]()
 * methods.
 * Non-const methods will automatically create the a #nullValue element
 * if it does not exist.
 * The sequence of an #arrayValue will be automatically resized and initialized
 * with #nullValue. resize() can be used to enlarge or truncate an #arrayValue.
 *
 * The get() methods can be used to obtain default value in the case the
 * required element does not exist.
 *
 * It is possible to iterate over the list of a #objectValue values using
 * the getMemberNames() method.
 *
 * \note #Value string-length fit in size_t, but keys must be < 2^30.
 * (The reason is an implementation detail.) A #CharReader will raise an
 * exception if a bound is exceeded to avoid security holes in your app,
 * but the Value API does *not* check bounds. That is the responsibility
 * of the caller.
 */
class JSON_API Value {
  friend class ValueIteratorBase;
public:
  typedef std::vector<JSONCPP_STRING> Members;
  typedef ValueIterator iterator;
  typedef ValueConstIterator const_iterator;
  typedef Json::UInt UInt;
  typedef Json::Int Int;
#if defined(JSON_HAS_INT64)
  typedef Json::UInt64 UInt64;
  typedef Json::Int64 Int64;
#endif // defined(JSON_HAS_INT64)
  typedef Json::LargestInt LargestInt;
  typedef Json::LargestUInt LargestUInt;
  typedef Json::ArrayIndex ArrayIndex;

  static const Value& null;  ///< We regret this reference to a global instance; prefer the simpler Value().
  static const Value& nullRef;  ///< just a kludge for binary-compatibility; same as null
  static Value const& nullSingleton(); ///< Prefer this to null or nullRef.

  /// Minimum signed integer value that can be stored in a Json::Value.
  static constexpr LargestInt minLargestInt = LargestInt(~(LargestUInt(-1) / 2));
  /// Maximum signed integer value that can be stored in a Json::Value.
  static constexpr LargestInt maxLargestInt = LargestInt(LargestUInt(-1) / 2);;
  /// Maximum unsigned integer value that can be stored in a Json::Value.
  static constexpr LargestUInt maxLargestUInt = LargestUInt(-1);;

  /// Minimum signed int value that can be stored in a Json::Value.
  static constexpr Int minInt = Int(~(UInt(-1) / 2));
  /// Maximum signed int value that can be stored in a Json::Value.
  static constexpr Int maxInt = Int(UInt(-1) / 2);
  /// Maximum unsigned int value that can be stored in a Json::Value.
  static constexpr UInt maxUInt = UInt(-1);

#if defined(JSON_HAS_INT64)
  /// Minimum signed 64 bits int value that can be stored in a Json::Value.
  static constexpr Int64 minInt64 = Int64(~(UInt64(-1) / 2));
  /// Maximum signed 64 bits int value that can be stored in a Json::Value.
  static constexpr Int64 maxInt64 = Int64(UInt64(-1) / 2);
  /// Maximum unsigned 64 bits int value that can be stored in a Json::Value.
  static constexpr UInt64 maxUInt64 = UInt64(-1);
  // The constant is hard-coded because some compiler have trouble
  // converting Value::maxUInt64 to a double correctly (AIX/xlC).
  // Assumes that UInt64 is a 64 bits integer.
  static constexpr double maxUInt64AsDouble = 18446744073709551615.0;
#endif // defined(JSON_HAS_INT64)

private:
#ifndef JSONCPP_DOC_EXCLUDE_IMPLEMENTATION
  class CZString {
  public:
    enum DuplicationPolicy {
      noDuplication = 0,
      duplicate,
      duplicateOnCopy
    };
    CZString(ArrayIndex index) : cstr_(0), index_(index) {}
    CZString(char const* str, unsigned length, DuplicationPolicy allocate)
		: cstr_(str) {
		// allocate != duplicate
		storage_.policy_ = allocate & 0x3;
		storage_.length_ = length & 0x3FFFFFFF;
	}
    CZString(CZString const& other) {
		cstr_ = (other.storage_.policy_ != noDuplication && other.cstr_ != 0
				 ? duplicateStringValue(other.cstr_, other.storage_.length_)
				 : other.cstr_);
		storage_.policy_ = static_cast<unsigned>(other.cstr_
												 ? (static_cast<DuplicationPolicy>(other.storage_.policy_) == noDuplication
													? noDuplication : duplicate)
												 : static_cast<DuplicationPolicy>(other.storage_.policy_)) & 3U;
		storage_.length_ = other.storage_.length_;
	}
#if JSON_HAS_RVALUE_REFERENCES
    CZString(CZString&& other) : cstr_(other.cstr_), index_(other.index_) {
		other.cstr_ = nullptr;
	}
#endif
    ~CZString() {
		if (cstr_ && storage_.policy_ == duplicate) {
			releaseStringValue(const_cast<char*>(cstr_), storage_.length_ + 1u); //+1 for null terminating character for sake of completeness but not actually necessary
		}
	}
    CZString& operator=(CZString other) {
		swap(other);
		return *this;
	}
    bool operator<(CZString const& other) const {
		if (!cstr_) return index_ < other.index_;
		//return strcmp(cstr_, other.cstr_) < 0;
		// Assume both are strings.
		unsigned this_len = this->storage_.length_;
		unsigned other_len = other.storage_.length_;
		unsigned min_len = std::min(this_len, other_len);
		JSON_ASSERT(this->cstr_ && other.cstr_);
		int comp = memcmp(this->cstr_, other.cstr_, min_len);
		if (comp < 0) return true;
		if (comp > 0) return false;
		return (this_len < other_len);
	}
    bool operator==(CZString const& other) const {
		if (!cstr_) return index_ == other.index_;
		//return strcmp(cstr_, other.cstr_) == 0;
		// Assume both are strings.
		unsigned this_len = this->storage_.length_;
		unsigned other_len = other.storage_.length_;
		if (this_len != other_len) return false;
		JSON_ASSERT(this->cstr_ && other.cstr_);
		int comp = memcmp(this->cstr_, other.cstr_, this_len);
		return comp == 0;
	}
    ArrayIndex index() const { return index_; }
    //const char* c_str() const; ///< \deprecated
    char const* data() const { return cstr_; }
    unsigned length() const { return storage_.length_; }
    bool isStaticString() const { return storage_.policy_ == noDuplication; }

  private:
    void swap(CZString& other) {
		std::swap(cstr_, other.cstr_);
		std::swap(index_, other.index_);
	}


    struct StringStorage {
      unsigned policy_: 2;
      unsigned length_: 30; // 1GB max
    };

    char const* cstr_;  // actually, a prefixed string, unless policy is noDup
    union {
      ArrayIndex index_;
      StringStorage storage_;
    };
  };

public:
#ifndef JSON_USE_CPPTL_SMALLMAP
  typedef std::map<CZString, Value> ObjectValues;
#else
  typedef CppTL::SmallMap<CZString, Value> ObjectValues;
#endif // ifndef JSON_USE_CPPTL_SMALLMAP
#endif // ifndef JSONCPP_DOC_EXCLUDE_IMPLEMENTATION

public:
  /** \brief Create a default Value of the given type.

    This is a very useful constructor.
    To create an empty array, pass arrayValue.
    To create an empty object, pass objectValue.
    Another Value can then be set to this one by assignment.
This is useful since clear() and resize() will not alter types.

    Examples:
\code
Json::Value null_value; // null
Json::Value arr_value(Json::arrayValue); // []
Json::Value obj_value(Json::objectValue); // {}
\endcode
  */
  Value(ValueType type = nullValue) {
	  static char const emptyString[] = "";
	  initBasic(vtype);
	  switch (vtype) {
		  case nullValue:
			  break;
		  case intValue:
		  case uintValue:
			  value_.int_ = 0;
			  break;
		  case realValue:
			  value_.real_ = 0.0;
			  break;
		  case stringValue:
			  // allocated_ == false, so this is safe.
			  value_.string_ = const_cast<char*>(static_cast<char const*>(emptyString));
			  break;
		  case arrayValue:
		  case objectValue:
			  value_.map_ = new ObjectValues();
			  break;
		  case booleanValue:
			  value_.bool_ = false;
			  break;
		  default:
			  JSON_ASSERT_UNREACHABLE;
	  }
  }


  Value(Int value) {
	  initBasic(intValue);
	  value_.int_ = value;
  }

  Value(UInt value) {
	  initBasic(uintValue);
	  value_.uint_ = value;
  }
#if defined(JSON_HAS_INT64)
  Value(Int64 value) {
	  initBasic(intValue);
	  value_.int_ = value;
  }
  Value(UInt64 value) {
	  initBasic(uintValue);
	  value_.uint_ = value;
  }
#endif // defined(JSON_HAS_INT64)

  Value(double value) {
	  initBasic(realValue);
	  value_.real_ = value;
  }

  Value(const char* value) {
	  initBasic(stringValue, true);
	  value_.string_ = duplicateAndPrefixStringValue(value, static_cast<unsigned>(strlen(value)));
  }

  Value(const char* beginValue, const char* endValue) {
	  initBasic(stringValue, true);
	  value_.string_ =
		  duplicateAndPrefixStringValue(beginValue, static_cast<unsigned>(endValue - beginValue));
  }

  Value(const JSONCPP_STRING& value) {
	  initBasic(stringValue, true);
	  value_.string_ =
		  duplicateAndPrefixStringValue(value.data(), static_cast<unsigned>(value.length()));
  }

  Value(const StaticString& value) {
	  initBasic(stringValue);
	  value_.string_ = const_cast<char*>(value.c_str());
  }

#ifdef JSON_USE_CPPTL
  Value(const CppTL::ConstString& value) {
	  initBasic(stringValue, true);
	  value_.string_ = duplicateAndPrefixStringValue(value, static_cast<unsigned>(value.length()));
  }
#endif

  Value(bool value) {
	  initBasic(booleanValue);
	  value_.bool_ = value;
  }

  Value(Value const& other)
	  : type_(other.type_), allocated_(false)
	  ,
	  comments_(0), start_(other.start_), limit_(other.limit_)
  {
	  switch (type_) {
		  case nullValue:
		  case intValue:
		  case uintValue:
		  case realValue:
		  case booleanValue:
			  value_ = other.value_;
			  break;
		  case stringValue:
			  if (other.value_.string_ && other.allocated_) {
				  unsigned len;
				  char const* str;
				  decodePrefixedString(other.allocated_, other.value_.string_,
									   &len, &str);
				  value_.string_ = duplicateAndPrefixStringValue(str, len);
				  allocated_ = true;
			  } else {
				  value_.string_ = other.value_.string_;
				  allocated_ = false;
			  }
			  break;
		  case arrayValue:
		  case objectValue:
			  value_.map_ = new ObjectValues(*other.value_.map_);
			  break;
		  default:
			  JSON_ASSERT_UNREACHABLE;
	  }
	  if (other.comments_) {
		  comments_ = new CommentInfo[numberOfCommentPlacement];
		  for (int comment = 0; comment < numberOfCommentPlacement; ++comment) {
			  const CommentInfo& otherComment = other.comments_[comment];
			  if (otherComment.comment_)
				  comments_[comment].setComment(
					  otherComment.comment_, strlen(otherComment.comment_));
		  }
	  }
  }

#if JSON_HAS_RVALUE_REFERENCES
  // Move constructor
  Value(Value&& other) {
	  initBasic(nullValue);
	  swap(other);
  }
#endif

  ~Value() {
	  switch (type_) {
		  case nullValue:
		  case intValue:
		  case uintValue:
		  case realValue:
		  case booleanValue:
			  break;
		  case stringValue:
			  if (allocated_)
				  releasePrefixedStringValue(value_.string_);
			  break;
		  case arrayValue:
		  case objectValue:
			  delete value_.map_;
			  break;
		  default:
			  JSON_ASSERT_UNREACHABLE;
	  }

	  delete[] comments_;

	  value_.uint_ = 0;
  }

  Value& operator=(Value other) {
	  swap(other);
	  return *this;
  }

  void swapPayload(Value& other) {
	  ValueType temp = type_;
	  type_ = other.type_;
	  other.type_ = temp;
	  std::swap(value_, other.value_);
	  int temp2 = allocated_;
	  allocated_ = other.allocated_;
	  other.allocated_ = temp2 & 0x1;
  }

  void swap(Value& other) {
	  swapPayload(other);
	  std::swap(comments_, other.comments_);
	  std::swap(start_, other.start_);
	  std::swap(limit_, other.limit_);
  }

  ValueType type() const { return type_; }

  int compare(const Value& other) const {
	  if (*this < other)
		  return -1;
	  if (*this > other)
		  return 1;
	  return 0;
  }

  bool operator<(const Value& other) const {
	  int typeDelta = type_ - other.type_;
	  if (typeDelta)
		  return typeDelta < 0 ? true : false;
	  switch (type_) {
		  case nullValue:
			  return false;
		  case intValue:
			  return value_.int_ < other.value_.int_;
		  case uintValue:
			  return value_.uint_ < other.value_.uint_;
		  case realValue:
			  return value_.real_ < other.value_.real_;
		  case booleanValue:
			  return value_.bool_ < other.value_.bool_;
		  case stringValue:
		  {
			  if ((value_.string_ == 0) || (other.value_.string_ == 0)) {
				  if (other.value_.string_) return true;
				  else return false;
			  }
			  unsigned this_len;
			  unsigned other_len;
			  char const* this_str;
			  char const* other_str;
			  decodePrefixedString(this->allocated_, this->value_.string_, &this_len, &this_str);
			  decodePrefixedString(other.allocated_, other.value_.string_, &other_len, &other_str);
			  unsigned min_len = std::min(this_len, other_len);
			  JSON_ASSERT(this_str && other_str);
			  int comp = memcmp(this_str, other_str, min_len);
			  if (comp < 0) return true;
			  if (comp > 0) return false;
			  return (this_len < other_len);
		  }
		  case arrayValue:
		  case objectValue:
		  {
			  int delta = int(value_.map_->size() - other.value_.map_->size());
			  if (delta)
				  return delta < 0;
			  return (*value_.map_) < (*other.value_.map_);
		  }
		  default:
			  JSON_ASSERT_UNREACHABLE;
	  }
	  return false; // unreachable
  }

  bool operator<=(const Value& other) const { return !(other < *this); }

  bool operator>=(const Value& other) const { return !(*this < other); }

  bool operator>(const Value& other) const { return other < *this; }

  bool operator==(const Value& other) const {
	  // if ( type_ != other.type_ )
	  // GCC 2.95.3 says:
	  // attempt to take address of bit-field structure member `Json::type_'
	  // Beats me, but a temp solves the problem.
	  int temp = other.type_;
	  if (type_ != temp)
		  return false;
	  switch (type_) {
		  case nullValue:
			  return true;
		  case intValue:
			  return value_.int_ == other.value_.int_;
		  case uintValue:
			  return value_.uint_ == other.value_.uint_;
		  case realValue:
			  return value_.real_ == other.value_.real_;
		  case booleanValue:
			  return value_.bool_ == other.value_.bool_;
		  case stringValue:
		  {
			  if ((value_.string_ == 0) || (other.value_.string_ == 0)) {
				  return (value_.string_ == other.value_.string_);
			  }
			  unsigned this_len;
			  unsigned other_len;
			  char const* this_str;
			  char const* other_str;
			  decodePrefixedString(this->allocated_, this->value_.string_, &this_len, &this_str);
			  decodePrefixedString(other.allocated_, other.value_.string_, &other_len, &other_str);
			  if (this_len != other_len) return false;
			  JSON_ASSERT(this_str && other_str);
			  int comp = memcmp(this_str, other_str, this_len);
			  return comp == 0;
		  }
		  case arrayValue:
		  case objectValue:
			  return value_.map_->size() == other.value_.map_->size() &&
				  (*value_.map_) == (*other.value_.map_);
		  default:
			  JSON_ASSERT_UNREACHABLE;
	  }
	  return false; // unreachable
  }

  bool operator!=(const Value& other) const { return !(*this == other); }

  const char* asCString() const {
	  JSON_ASSERT_MESSAGE(type_ == stringValue,
						  "in Json::asCString(): requires stringValue");
	  if (value_.string_ == 0) return 0;
	  unsigned this_len;
	  char const* this_str;
	  decodePrefixedString(this->allocated_, this->value_.string_, &this_len, &this_str);
	  return this_str;
  }

#if JSONCPP_USING_SECURE_MEMORY
  unsigned getCStringLength() const {
	  JSON_ASSERT_MESSAGE(type_ == stringValue,
						  "in Json::asCString(): requires stringValue");
	  if (value_.string_ == 0) return 0;
	  unsigned this_len;
	  char const* this_str;
	  decodePrefixedString(this->allocated_, this->value_.string_, &this_len, &this_str);
	  return this_len;
  }
#endif

  bool getString(char const** str, char const** cend) const {
	  if (type_ != stringValue) return false;
	  if (value_.string_ == 0) return false;
	  unsigned length;
	  decodePrefixedString(this->allocated_, this->value_.string_, &length, str);
	  *cend = *str + length;
	  return true;
  }

  JSONCPP_STRING asString() const {
	  switch (type_) {
		  case nullValue:
			  return "";
		  case stringValue:
		  {
			  if (value_.string_ == 0) return "";
			  unsigned this_len;
			  char const* this_str;
			  decodePrefixedString(this->allocated_, this->value_.string_, &this_len, &this_str);
			  return JSONCPP_STRING(this_str, this_len);
		  }
		  case booleanValue:
			  return value_.bool_ ? "true" : "false";
		  case intValue:
			  return valueToString(value_.int_);
		  case uintValue:
			  return valueToString(value_.uint_);
		  case realValue:
			  return valueToString(value_.real_);
		  default:
			  JSON_FAIL_MESSAGE("Type is not convertible to string");
	  }
  }

#ifdef JSON_USE_CPPTL
  CppTL::ConstString asConstString() const {
	  unsigned len;
	  char const* str;
	  decodePrefixedString(allocated_, value_.string_,
						   &len, &str);
	  return CppTL::ConstString(str, len);
  }
#endif

  Int asInt() const {
	  switch (type_) {
		  case intValue:
			  JSON_ASSERT_MESSAGE(isInt(), "LargestInt out of Int range");
			  return Int(value_.int_);
		  case uintValue:
			  JSON_ASSERT_MESSAGE(isInt(), "LargestUInt out of Int range");
			  return Int(value_.uint_);
		  case realValue:
			  JSON_ASSERT_MESSAGE(InRange(value_.real_, minInt, maxInt),
								  "double out of Int range");
			  return Int(value_.real_);
		  case nullValue:
			  return 0;
		  case booleanValue:
			  return value_.bool_ ? 1 : 0;
		  default:
			  break;
	  }
	  JSON_FAIL_MESSAGE("Value is not convertible to Int.");
  }

  UInt asUInt() const {
	  switch (type_) {
		  case intValue:
			  JSON_ASSERT_MESSAGE(isUInt(), "LargestInt out of UInt range");
			  return UInt(value_.int_);
		  case uintValue:
			  JSON_ASSERT_MESSAGE(isUInt(), "LargestUInt out of UInt range");
			  return UInt(value_.uint_);
		  case realValue:
			  JSON_ASSERT_MESSAGE(InRange(value_.real_, 0, maxUInt),
								  "double out of UInt range");
			  return UInt(value_.real_);
		  case nullValue:
			  return 0;
		  case booleanValue:
			  return value_.bool_ ? 1 : 0;
		  default:
			  break;
	  }
	  JSON_FAIL_MESSAGE("Value is not convertible to UInt.");
  }

#if defined(JSON_HAS_INT64)

  Int64 asInt64() const {
	  switch (type_) {
		  case intValue:
			  return Int64(value_.int_);
		  case uintValue:
			  JSON_ASSERT_MESSAGE(isInt64(), "LargestUInt out of Int64 range");
			  return Int64(value_.uint_);
		  case realValue:
			  JSON_ASSERT_MESSAGE(InRange(value_.real_, minInt64, maxInt64),
								  "double out of Int64 range");
			  return Int64(value_.real_);
		  case nullValue:
			  return 0;
		  case booleanValue:
			  return value_.bool_ ? 1 : 0;
		  default:
			  break;
	  }
	  JSON_FAIL_MESSAGE("Value is not convertible to Int64.");
  }

  UInt64 asUInt64() const {
	  switch (type_) {
		  case intValue:
			  JSON_ASSERT_MESSAGE(isUInt64(), "LargestInt out of UInt64 range");
			  return UInt64(value_.int_);
		  case uintValue:
			  return UInt64(value_.uint_);
		  case realValue:
			  JSON_ASSERT_MESSAGE(InRange(value_.real_, 0, maxUInt64),
								  "double out of UInt64 range");
			  return UInt64(value_.real_);
		  case nullValue:
			  return 0;
		  case booleanValue:
			  return value_.bool_ ? 1 : 0;
		  default:
			  break;
	  }
	  JSON_FAIL_MESSAGE("Value is not convertible to UInt64.");
  }
#endif // if defined(JSON_HAS_INT64)

  LargestInt asLargestInt() const {
#if defined(JSON_NO_INT64)
	  return asInt();
#else
	  return asInt64();
#endif
  }

  LargestUInt asLargestUInt() const {
#if defined(JSON_NO_INT64)
	  return asUInt();
#else
	  return asUInt64();
#endif
  }

  double asDouble() const {
	  switch (type_) {
		  case intValue:
			  return static_cast<double>(value_.int_);
		  case uintValue:
#if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
			  return static_cast<double>(value_.uint_);
#else  // if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
			  return integerToDouble(value_.uint_);
#endif // if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
		  case realValue:
			  return value_.real_;
		  case nullValue:
			  return 0.0;
		  case booleanValue:
			  return value_.bool_ ? 1.0 : 0.0;
		  default:
			  break;
	  }
	  JSON_FAIL_MESSAGE("Value is not convertible to double.");
  }

  float asFloat() const {
	  switch (type_) {
		  case intValue:
			  return static_cast<float>(value_.int_);
		  case uintValue:
#if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
			  return static_cast<float>(value_.uint_);
#else  // if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
			  // This can fail (silently?) if the value is bigger than MAX_FLOAT.
			  return static_cast<float>(integerToDouble(value_.uint_));
#endif // if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
		  case realValue:
			  return static_cast<float>(value_.real_);
		  case nullValue:
			  return 0.0;
		  case booleanValue:
			  return value_.bool_ ? 1.0f : 0.0f;
		  default:
			  break;
	  }
	  JSON_FAIL_MESSAGE("Value is not convertible to float.");
  }

  bool asBool() const {
	  switch (type_) {
		  case booleanValue:
			  return value_.bool_;
		  case nullValue:
			  return false;
		  case intValue:
			  return value_.int_ ? true : false;
		  case uintValue:
			  return value_.uint_ ? true : false;
		  case realValue:
			  // This is kind of strange. Not recommended.
			  return (value_.real_ != 0.0) ? true : false;
		  default:
			  break;
	  }
	  JSON_FAIL_MESSAGE("Value is not convertible to bool.");
  }

  bool isConvertibleTo(ValueType other) const {
	  switch (other) {
		  case nullValue:
			  return (isNumeric() && asDouble() == 0.0) ||
				  (type_ == booleanValue && value_.bool_ == false) ||
				  (type_ == stringValue && asString() == "") ||
				  (type_ == arrayValue && value_.map_->size() == 0) ||
				  (type_ == objectValue && value_.map_->size() == 0) ||
				  type_ == nullValue;
		  case intValue:
			  return isInt() ||
				  (type_ == realValue && InRange(value_.real_, minInt, maxInt)) ||
				  type_ == booleanValue || type_ == nullValue;
		  case uintValue:
			  return isUInt() ||
				  (type_ == realValue && InRange(value_.real_, 0, maxUInt)) ||
				  type_ == booleanValue || type_ == nullValue;
		  case realValue:
			  return isNumeric() || type_ == booleanValue || type_ == nullValue;
		  case booleanValue:
			  return isNumeric() || type_ == booleanValue || type_ == nullValue;
		  case stringValue:
			  return isNumeric() || type_ == booleanValue || type_ == stringValue ||
				  type_ == nullValue;
		  case arrayValue:
			  return type_ == arrayValue || type_ == nullValue;
		  case objectValue:
			  return type_ == objectValue || type_ == nullValue;
	  }
	  JSON_ASSERT_UNREACHABLE;
	  return false;
  }

  /// Number of values in array or object
  ArrayIndex size() const {
	  switch (type_) {
		  case nullValue:
		  case intValue:
		  case uintValue:
		  case realValue:
		  case booleanValue:
		  case stringValue:
			  return 0;
		  case arrayValue: // size of the array is highest index + 1
			  if (!value_.map_->empty()) {
				  ObjectValues::const_iterator itLast = value_.map_->end();
				  --itLast;
				  return (*itLast).first.index() + 1;
			  }
			  return 0;
		  case objectValue:
			  return ArrayIndex(value_.map_->size());
	  }
	  JSON_ASSERT_UNREACHABLE;
	  return 0; // unreachable;
  }

  bool empty() const {
	  if (isNull() || isArray() || isObject())
		  return size() == 0u;
	  else
		  return false;
  }

  bool operator!() const { return isNull(); }

  void clear() {
	  JSON_ASSERT_MESSAGE(type_ == nullValue || type_ == arrayValue ||
						  type_ == objectValue,
						  "in Json::clear(): requires complex value");
	  start_ = 0;
	  limit_ = 0;
	  switch (type_) {
		  case arrayValue:
		  case objectValue:
			  value_.map_->clear();
			  break;
		  default:
			  break;
	  }
  }

  void resize(ArrayIndex newSize) {
	  JSON_ASSERT_MESSAGE(type_ == nullValue || type_ == arrayValue,
						  "in Json::resize(): requires arrayValue");
	  if (type_ == nullValue)
		  *this = Value(arrayValue);
	  ArrayIndex oldSize = size();
	  if (newSize == 0)
		  clear();
	  else if (newSize > oldSize)
		  (*this)[newSize - 1];
	  else {
		  for (ArrayIndex index = newSize; index < oldSize; ++index) {
			  value_.map_->erase(index);
		  }
		  JSON_ASSERT(size() == newSize);
	  }
  }

  Value& operator[](ArrayIndex index) {
	  JSON_ASSERT_MESSAGE(
		  type_ == nullValue || type_ == arrayValue,
		  "in Json::operator[](ArrayIndex): requires arrayValue");
	  if (type_ == nullValue)
		  *this = Value(arrayValue);
	  CZString key(index);
	  ObjectValues::iterator it = value_.map_->lower_bound(key);
	  if (it != value_.map_->end() && (*it).first == key)
		  return (*it).second;

	  ObjectValues::value_type defaultValue(key, nullSingleton());
	  it = value_.map_->insert(it, defaultValue);
	  return (*it).second;
  }

  Value& operator[](int index) {
	  JSON_ASSERT_MESSAGE(
		  index >= 0,
		  "in Json::operator[](int index): index cannot be negative");
	  return (*this)[ArrayIndex(index)];
  }

  const Value& operator[](ArrayIndex index) const {
	  JSON_ASSERT_MESSAGE(
		  type_ == nullValue || type_ == arrayValue,
		  "in Json::operator[](ArrayIndex)const: requires arrayValue");
	  if (type_ == nullValue)
		  return nullSingleton();
	  CZString key(index);
	  ObjectValues::const_iterator it = value_.map_->find(key);
	  if (it == value_.map_->end())
		  return nullSingleton();
	  return (*it).second;
  }

  const Value& operator[](int index) const {
	  JSON_ASSERT_MESSAGE(
		  index >= 0,
		  "in Json::operator[](int index) const: index cannot be negative");
	  return (*this)[ArrayIndex(index)];
  }

  Value get(ArrayIndex index, const Value& defaultValue) const {
	  const Value* value = &((*this)[index]);
	  return value == &nullSingleton() ? defaultValue : *value;
  }

  bool isValidIndex(ArrayIndex index) const { return index < size(); }

  Value const* find(char const* key, char const* cend) const
  {
	  JSON_ASSERT_MESSAGE(
		  type_ == nullValue || type_ == objectValue,
		  "in Json::find(key, end, found): requires objectValue or nullValue");
	  if (type_ == nullValue) return NULL;
	  CZString actualKey(key, static_cast<unsigned>(cend - key), CZString::noDuplication);
	  ObjectValues::const_iterator it = value_.map_->find(actualKey);
	  if (it == value_.map_->end()) return NULL;
	  return &(*it).second;
  }
  const Value& operator[](const char* key) const
  {
	  Value const* found = find(key, key + strlen(key));
	  if (!found) return nullSingleton();
	  return *found;
  }
  Value const& operator[](JSONCPP_STRING const& key) const
  {
	  Value const* found = find(key.data(), key.data() + key.length());
	  if (!found) return nullSingleton();
	  return *found;
  }

  Value& operator[](const char* key) {
	  return resolveReference(key, key + strlen(key));
  }

  Value& operator[](const JSONCPP_STRING& key) {
	  return resolveReference(key.data(), key.data() + key.length());
  }

  Value& operator[](const StaticString& key) {
	  return resolveReference(key.c_str());
  }

#ifdef JSON_USE_CPPTL
  Value& operator[](const CppTL::ConstString& key) {
	  return resolveReference(key.c_str(), key.end_c_str());
  }
  Value const& operator[](CppTL::ConstString const& key) const
  {
	  Value const* found = find(key.c_str(), key.end_c_str());
	  if (!found) return nullSingleton();
	  return *found;
  }
#endif

  Value& append(const Value& value) { return (*this)[size()] = value; }

  Value get(char const* key, char const* cend, Value const& defaultValue) const
  {
	  Value const* found = find(key, cend);
	  return !found ? defaultValue : *found;
  }
  Value get(char const* key, Value const& defaultValue) const
  {
	  return get(key, key + strlen(key), defaultValue);
  }
  Value get(JSONCPP_STRING const& key, Value const& defaultValue) const
  {
	  return get(key.data(), key.data() + key.length(), defaultValue);
  }


  bool removeMember(const char* key, const char* cend, Value* removed)
  {
	  if (type_ != objectValue) {
		  return false;
	  }
	  CZString actualKey(key, static_cast<unsigned>(cend - key), CZString::noDuplication);
	  ObjectValues::iterator it = value_.map_->find(actualKey);
	  if (it == value_.map_->end())
		  return false;
	  *removed = it->second;
	  value_.map_->erase(it);
	  return true;
  }
  bool removeMember(const char* key, Value* removed)
  {
	  return removeMember(key, key + strlen(key), removed);
  }
  bool removeMember(JSONCPP_STRING const& key, Value* removed)
  {
	  return removeMember(key.data(), key.data() + key.length(), removed);
  }
  Value removeMember(const char* key)
  {
	  JSON_ASSERT_MESSAGE(type_ == nullValue || type_ == objectValue,
						  "in Json::removeMember(): requires objectValue");
	  if (type_ == nullValue)
		  return nullSingleton();

	  Value removed;  // null
	  removeMember(key, key + strlen(key), &removed);
	  return removed; // still null if removeMember() did nothing
  }
  Value removeMember(const JSONCPP_STRING& key)
  {
	  return removeMember(key.c_str());
  }

  bool removeIndex(ArrayIndex index, Value* removed) {
	  if (type_ != arrayValue) {
		  return false;
	  }
	  CZString key(index);
	  ObjectValues::iterator it = value_.map_->find(key);
	  if (it == value_.map_->end()) {
		  return false;
	  }
	  *removed = it->second;
	  ArrayIndex oldSize = size();
	  // shift left all items left, into the place of the "removed"
	  for (ArrayIndex i = index; i < (oldSize - 1); ++i) {
		  CZString keey(i);
		  (*value_.map_)[keey] = (*this)[i + 1];
	  }
	  // erase the last one ("leftover")
	  CZString keyLast(oldSize - 1);
	  ObjectValues::iterator itLast = value_.map_->find(keyLast);
	  value_.map_->erase(itLast);
	  return true;
  }

#ifdef JSON_USE_CPPTL
  Value get(const CppTL::ConstString& key,
				   const Value& defaultValue) const {
	  return get(key.c_str(), key.end_c_str(), defaultValue);
  }
#endif

  bool isMember(char const* key, char const* cend) const
  {
	  Value const* value = find(key, cend);
	  return NULL != value;
  }
  bool isMember(char const* key) const
  {
	  return isMember(key, key + strlen(key));
  }
  bool isMember(JSONCPP_STRING const& key) const
  {
	  return isMember(key.data(), key.data() + key.length());
  }

#ifdef JSON_USE_CPPTL
  bool isMember(const CppTL::ConstString& key) const {
	  return isMember(key.c_str(), key.end_c_str());
  }
#endif

  Members getMemberNames() const {
	  JSON_ASSERT_MESSAGE(
		  type_ == nullValue || type_ == objectValue,
		  "in Json::getMemberNames(), value must be objectValue");
	  if (type_ == nullValue)
		  return Members();
	  Members members;
	  members.reserve(value_.map_->size());
	  ObjectValues::const_iterator it = value_.map_->begin();
	  ObjectValues::const_iterator itEnd = value_.map_->end();
	  for (; it != itEnd; ++it) {
		  members.push_back(JSONCPP_STRING((*it).first.data(),
			  (*it).first.length()));
	  }
	  return members;
  }
  //
  //# ifdef JSON_USE_CPPTL
  // EnumMemberNames
  // enumMemberNames() const
  //{
  //   if ( type_ == objectValue )
  //   {
  //      return CppTL::Enum::any(  CppTL::Enum::transform(
  //         CppTL::Enum::keys( *(value_.map_), CppTL::Type<const CZString &>() ),
  //         MemberNamesTransform() ) );
  //   }
  //   return EnumMemberNames();
  //}
  //
  //
  // EnumValues
  // enumValues() const
  //{
  //   if ( type_ == objectValue  ||  type_ == arrayValue )
  //      return CppTL::Enum::anyValues( *(value_.map_),
  //                                     CppTL::Type<const Value &>() );
  //   return EnumValues();
  //}
  //
  //# endif

  static bool IsIntegral(double d) {
	  double integral_part;
	  return modf(d, &integral_part) == 0.0;
  }

  bool isNull() const { return type_ == nullValue; }

  bool isBool() const { return type_ == booleanValue; }

  bool isInt() const {
	  switch (type_) {
		  case intValue:
#if defined(JSON_HAS_INT64)
			  return value_.int_ >= minInt && value_.int_ <= maxInt;
#else
			  return true;
#endif
		  case uintValue:
			  return value_.uint_ <= UInt(maxInt);
		  case realValue:
			  return value_.real_ >= minInt && value_.real_ <= maxInt &&
				  IsIntegral(value_.real_);
		  default:
			  break;
	  }
	  return false;
  }

  bool isUInt() const {
	  switch (type_) {
		  case intValue:
#if defined(JSON_HAS_INT64)
			  return value_.int_ >= 0 && LargestUInt(value_.int_) <= LargestUInt(maxUInt);
#else
			  return value_.int_ >= 0;
#endif
		  case uintValue:
#if defined(JSON_HAS_INT64)
			  return value_.uint_ <= maxUInt;
#else
			  return true;
#endif
		  case realValue:
			  return value_.real_ >= 0 && value_.real_ <= maxUInt &&
				  IsIntegral(value_.real_);
		  default:
			  break;
	  }
	  return false;
  }

  bool isInt64() const {
#if defined(JSON_HAS_INT64)
	  switch (type_) {
		  case intValue:
			  return true;
		  case uintValue:
			  return value_.uint_ <= UInt64(maxInt64);
		  case realValue:
			  // Note that maxInt64 (= 2^63 - 1) is not exactly representable as a
			  // double, so double(maxInt64) will be rounded up to 2^63. Therefore we
			  // require the value to be strictly less than the limit.
			  return value_.real_ >= double(minInt64) &&
				  value_.real_ < double(maxInt64) && IsIntegral(value_.real_);
		  default:
			  break;
	  }
#endif // JSON_HAS_INT64
	  return false;
  }

  bool isUInt64() const {
#if defined(JSON_HAS_INT64)
	  switch (type_) {
		  case intValue:
			  return value_.int_ >= 0;
		  case uintValue:
			  return true;
		  case realValue:
			  // Note that maxUInt64 (= 2^64 - 1) is not exactly representable as a
			  // double, so double(maxUInt64) will be rounded up to 2^64. Therefore we
			  // require the value to be strictly less than the limit.
			  return value_.real_ >= 0 && value_.real_ < maxUInt64AsDouble &&
				  IsIntegral(value_.real_);
		  default:
			  break;
	  }
#endif // JSON_HAS_INT64
	  return false;
  }

  bool isIntegral() const {
#if defined(JSON_HAS_INT64)
	  return isInt64() || isUInt64();
#else
	  return isInt() || isUInt();
#endif
  }

  bool isDouble() const { return type_ == intValue || type_ == uintValue || type_ == realValue; }

  bool isNumeric() const { return isDouble(); }

  bool isString() const { return type_ == stringValue; }

  bool isArray() const { return type_ == arrayValue; }

  bool isObject() const { return type_ == objectValue; }

  void setComment(const char* comment, size_t len, CommentPlacement placement) {
	  if (!comments_)
		  comments_ = new CommentInfo[numberOfCommentPlacement];
	  if ((len > 0) && (comment[len - 1] == '\n')) {
		  // Always discard trailing newline, to aid indentation.
		  len -= 1;
	  }
	  comments_[placement].setComment(comment, len);
  }

  void setComment(const char* comment, CommentPlacement placement) {
	  setComment(comment, strlen(comment), placement);
  }

  void setComment(const JSONCPP_STRING& comment, CommentPlacement placement) {
	  setComment(comment.c_str(), comment.length(), placement);
  }

  bool hasComment(CommentPlacement placement) const {
	  return comments_ != 0 && comments_[placement].comment_ != 0;
  }

  JSONCPP_STRING getComment(CommentPlacement placement) const {
	  if (hasComment(placement))
		  return comments_[placement].comment_;
	  return "";
  }

  void setOffsetStart(ptrdiff_t start) { start_ = start; }

  void setOffsetLimit(ptrdiff_t limit) { limit_ = limit; }

  ptrdiff_t getOffsetStart() const { return start_; }

  ptrdiff_t getOffsetLimit() const { return limit_; }

  JSONCPP_STRING toStyledString() const {
	  StyledWriter writer;
	  return writer.write(*this);
  }

  const_iterator begin() const {
	  switch (type_) {
		  case arrayValue:
		  case objectValue:
			  if (value_.map_)
				  return const_iterator(value_.map_->begin());
			  break;
		  default:
			  break;
	  }
	  return const_iterator();
  }

  const_iterator end() const {
	  switch (type_) {
		  case arrayValue:
		  case objectValue:
			  if (value_.map_)
				  return const_iterator(value_.map_->end());
			  break;
		  default:
			  break;
	  }
	  return const_iterator();
  }

  iterator begin() {
	  switch (type_) {
		  case arrayValue:
		  case objectValue:
			  if (value_.map_)
				  return iterator(value_.map_->begin());
			  break;
		  default:
			  break;
	  }
	  return iterator();
  }

  iterator end() {
	  switch (type_) {
		  case arrayValue:
		  case objectValue:
			  if (value_.map_)
				  return iterator(value_.map_->end());
			  break;
		  default:
			  break;
	  }
	  return iterator();
  }

private:
  void initBasic(ValueType type, bool allocated = false) {
	  type_ = vtype;
	  allocated_ = allocated;
	  comments_ = 0;
	  start_ = 0;
	  limit_ = 0;
  }

  // Access an object value by name, create a null member if it does not exist.
  // @pre Type of '*this' is object or null.
  // @param key is null-terminated.
  Value& resolveReference(const char* key) {
	  JSON_ASSERT_MESSAGE(
		  type_ == nullValue || type_ == objectValue,
		  "in Json::Value::resolveReference(): requires objectValue");
	  if (type_ == nullValue)
		  *this = Value(objectValue);
	  CZString actualKey(
		  key, static_cast<unsigned>(strlen(key)), CZString::noDuplication); // NOTE!
	  ObjectValues::iterator it = value_.map_->lower_bound(actualKey);
	  if (it != value_.map_->end() && (*it).first == actualKey)
		  return (*it).second;

	  ObjectValues::value_type defaultValue(actualKey, nullSingleton());
	  it = value_.map_->insert(it, defaultValue);
	  Value& value = (*it).second;
	  return value;
  }

  // @param key is not null-terminated.
  Value& resolveReference(char const* key, char const* cend)
  {
	  JSON_ASSERT_MESSAGE(
		  type_ == nullValue || type_ == objectValue,
		  "in Json::Value::resolveReference(key, end): requires objectValue");
	  if (type_ == nullValue)
		  *this = Value(objectValue);
	  CZString actualKey(
		  key, static_cast<unsigned>(cend - key), CZString::duplicateOnCopy);
	  ObjectValues::iterator it = value_.map_->lower_bound(actualKey);
	  if (it != value_.map_->end() && (*it).first == actualKey)
		  return (*it).second;

	  ObjectValues::value_type defaultValue(actualKey, nullSingleton());
	  it = value_.map_->insert(it, defaultValue);
	  Value& value = (*it).second;
	  return value;
  }

  struct CommentInfo {
    CommentInfo() : comment_(0)
	{}
    ~CommentInfo() {
		if (comment_)
			releaseStringValue(comment_, 0u);
	}

    void setComment(const char* text, size_t len) {
		if (comment_) {
			releaseStringValue(comment_, 0u);
			comment_ = 0;
		}
		JSON_ASSERT(text != 0);
		JSON_ASSERT_MESSAGE(
			text[0] == '\0' || text[0] == '/',
			"in Json::Value::setComment(): Comments must start with /");
		// It seems that /**/ style comments are acceptable as well.
		comment_ = duplicateStringValue(text, len);
	}

    char* comment_;
  };

  // struct MemberNamesTransform
  //{
  //   typedef const char *result_type;
  //   const char *operator()( const CZString &name ) const
  //   {
  //      return name.c_str();
  //   }
  //};

  union ValueHolder {
    LargestInt int_;
    LargestUInt uint_;
    double real_;
    bool bool_;
    char* string_;  // actually ptr to unsigned, followed by str, unless !allocated_
    ObjectValues* map_;
  } value_;
  ValueType type_ : 8;
  unsigned int allocated_ : 1; // Notes: if declared as bool, bitfield is useless.
                               // If not allocated_, string_ must be null-terminated.
  CommentInfo* comments_;

  // [start, limit) byte offsets in the source JSON text from which this Value
  // was extracted.
  ptrdiff_t start_;
  ptrdiff_t limit_;
};

/** \brief Experimental and untested: represents an element of the "path" to
 * access a node.
 */
class JSON_API PathArgument {
public:
  friend class Path;

  PathArgument()
	  : key_(), index_(), kind_(kindNone) {}
  PathArgument(ArrayIndex index)
	  : key_(), index_(index), kind_(kindIndex) {}
  PathArgument(const char* key)
	  : key_(key), index_(), kind_(kindKey) {}
  PathArgument(const JSONCPP_STRING& key)
	  : key_(key.c_str()), index_(), kind_(kindKey) {}


private:
  enum Kind {
    kindNone = 0,
    kindIndex,
    kindKey
  };
  JSONCPP_STRING key_;
  ArrayIndex index_;
  Kind kind_;
};

/** \brief Experimental and untested: represents a "path" to access a node.
 *
 * Syntax:
 * - "." => root node
 * - ".[n]" => elements at index 'n' of root node (an array value)
 * - ".name" => member named 'name' of root node (an object value)
 * - ".name1.name2.name3"
 * - ".[0][1][2].name1[3]"
 * - ".%" => member name is provided as parameter
 * - ".[%]" => index is provied as parameter
 */
class JSON_API Path {
public:
  Path(const JSONCPP_STRING& path,
       const PathArgument& a1 = PathArgument(),
       const PathArgument& a2 = PathArgument(),
       const PathArgument& a3 = PathArgument(),
       const PathArgument& a4 = PathArgument(),
       const PathArgument& a5 = PathArgument()) {
	  InArgs in;
	  in.push_back(&a1);
	  in.push_back(&a2);
	  in.push_back(&a3);
	  in.push_back(&a4);
	  in.push_back(&a5);
	  makePath(path, in);
  }

  const Value& resolve(const Value& root) const {
	  const Value* node = &root;
	  for (Args::const_iterator it = args_.begin(); it != args_.end(); ++it) {
		  const PathArgument& arg = *it;
		  if (arg.kind_ == PathArgument::kindIndex) {
			  if (!node->isArray() || !node->isValidIndex(arg.index_)) {
				  // Error: unable to resolve path (array value expected at position...
				  return Value::null;
			  }
			  node = &((*node)[arg.index_]);
		  } else if (arg.kind_ == PathArgument::kindKey) {
			  if (!node->isObject()) {
				  // Error: unable to resolve path (object value expected at position...)
				  return Value::null;
			  }
			  node = &((*node)[arg.key_]);
			  if (node == &Value::nullSingleton()) {
				  // Error: unable to resolve path (object has no member named '' at
				  // position...)
				  return Value::null;
			  }
		  }
	  }
	  return *node;
  }
  Value resolve(const Value& root, const Value& defaultValue) const {
	  const Value* node = &root;
	  for (Args::const_iterator it = args_.begin(); it != args_.end(); ++it) {
		  const PathArgument& arg = *it;
		  if (arg.kind_ == PathArgument::kindIndex) {
			  if (!node->isArray() || !node->isValidIndex(arg.index_))
				  return defaultValue;
			  node = &((*node)[arg.index_]);
		  } else if (arg.kind_ == PathArgument::kindKey) {
			  if (!node->isObject())
				  return defaultValue;
			  node = &((*node)[arg.key_]);
			  if (node == &Value::nullSingleton())
				  return defaultValue;
		  }
	  }
	  return *node;
  }

  /// Creates the "path" to access the specified node and returns a reference on
  /// the node.
  Value& make(Value& root) const {
	  Value* node = &root;
	  for (Args::const_iterator it = args_.begin(); it != args_.end(); ++it) {
		  const PathArgument& arg = *it;
		  if (arg.kind_ == PathArgument::kindIndex) {
			  if (!node->isArray()) {
				  // Error: node is not an array at position ...
			  }
			  node = &((*node)[arg.index_]);
		  } else if (arg.kind_ == PathArgument::kindKey) {
			  if (!node->isObject()) {
				  // Error: node is not an object at position...
			  }
			  node = &((*node)[arg.key_]);
		  }
	  }
	  return *node;
  }


private:
  typedef std::vector<const PathArgument*> InArgs;
  typedef std::vector<PathArgument> Args;

  void makePath(const JSONCPP_STRING& path, const InArgs& in) {
	  const char* current = path.c_str();
	  const char* end = current + path.length();
	  InArgs::const_iterator itInArg = in.begin();
	  while (current != end) {
		  if (*current == '[') {
			  ++current;
			  if (*current == '%')
				  addPathInArg(path, in, itInArg, PathArgument::kindIndex);
			  else {
				  ArrayIndex index = 0;
				  for (; current != end && *current >= '0' && *current <= '9'; ++current)
					  index = index * 10 + ArrayIndex(*current - '0');
				  args_.push_back(index);
			  }
			  if (current == end || *++current != ']')
				  invalidPath(path, int(current - path.c_str()));
		  } else if (*current == '%') {
			  addPathInArg(path, in, itInArg, PathArgument::kindKey);
			  ++current;
		  } else if (*current == '.' || *current == ']') {
			  ++current;
		  } else {
			  const char* beginName = current;
			  while (current != end && !strchr("[.", *current))
				  ++current;
			  args_.push_back(JSONCPP_STRING(beginName, current));
		  }
	  }
  }
  void addPathInArg(const JSONCPP_STRING& path,
                    const InArgs& in,
                    InArgs::const_iterator& itInArg,
                    PathArgument::Kind kind) {
	  if (itInArg == in.end()) {
		  // Error: missing argument %d
	  } else if ((*itInArg)->kind_ != kind) {
		  // Error: bad argument type
	  } else {
		  args_.push_back(**itInArg++);
	  }
  }
  void invalidPath(const JSONCPP_STRING& path, int location) {
	  // Error: invalid path.
  }

  Args args_;
};

/** \brief base class for Value iterators.
 *
 */
class JSON_API ValueIteratorBase {
public:
  typedef std::bidirectional_iterator_tag iterator_category;
  typedef unsigned int size_t;
  typedef int difference_type;
  typedef ValueIteratorBase SelfType;

  bool operator==(const SelfType& other) const { return isEqual(other); }

  bool operator!=(const SelfType& other) const { return !isEqual(other); }

  difference_type operator-(const SelfType& other) const {
    return other.computeDistance(*this);
  }

  /// Return either the index or the member name of the referenced value as a
  /// Value.
  Value key() const;

  /// Return the index of the referenced Value, or -1 if it is not an arrayValue.
  UInt index() const;

  /// Return the member name of the referenced Value, or "" if it is not an
  /// objectValue.
  /// \note Avoid `c_str()` on result, as embedded zeroes are possible.
  JSONCPP_STRING name() const;

  /// Return the member name of the referenced Value. "" if it is not an
  /// objectValue.
  /// \deprecated This cannot be used for UTF-8 strings, since there can be embedded nulls.
  JSONCPP_DEPRECATED("Use `key = name();` instead.")
  char const* memberName() const;
  /// Return the member name of the referenced Value, or NULL if it is not an
  /// objectValue.
  /// \note Better version than memberName(). Allows embedded nulls.
  char const* memberName(char const** end) const;

protected:
  Value& deref() const;

  void increment();

  void decrement();

  difference_type computeDistance(const SelfType& other) const;

  bool isEqual(const SelfType& other) const;

  void copy(const SelfType& other);

private:
  Value::ObjectValues::iterator current_;
  // Indicates that iterator is for a null value.
  bool isNull_;

public:
  // For some reason, BORLAND needs these at the end, rather
  // than earlier. No idea why.
  ValueIteratorBase();
  explicit ValueIteratorBase(const Value::ObjectValues::iterator& current);
};

/** \brief const iterator for object and array value.
 *
 */
class JSON_API ValueConstIterator : public ValueIteratorBase {
  friend class Value;

public:
  typedef const Value value_type;
  //typedef unsigned int size_t;
  //typedef int difference_type;
  typedef const Value& reference;
  typedef const Value* pointer;
  typedef ValueConstIterator SelfType;

  ValueConstIterator();
  ValueConstIterator(ValueIterator const& other);

private:
/*! \internal Use by Value to create an iterator.
 */
  explicit ValueConstIterator(const Value::ObjectValues::iterator& current);
public:
  SelfType& operator=(const ValueIteratorBase& other);

  SelfType operator++(int) {
    SelfType temp(*this);
    ++*this;
    return temp;
  }

  SelfType operator--(int) {
    SelfType temp(*this);
    --*this;
    return temp;
  }

  SelfType& operator--() {
    decrement();
    return *this;
  }

  SelfType& operator++() {
    increment();
    return *this;
  }

  reference operator*() const { return deref(); }

  pointer operator->() const { return &deref(); }
};

/** \brief Iterator for object and array value.
 */
class JSON_API ValueIterator : public ValueIteratorBase {
  friend class Value;

public:
  typedef Value value_type;
  typedef unsigned int size_t;
  typedef int difference_type;
  typedef Value& reference;
  typedef Value* pointer;
  typedef ValueIterator SelfType;

  ValueIterator();
  explicit ValueIterator(const ValueConstIterator& other);
  ValueIterator(const ValueIterator& other);

private:
/*! \internal Use by Value to create an iterator.
 */
  explicit ValueIterator(const Value::ObjectValues::iterator& current);
public:
  SelfType& operator=(const SelfType& other);

  SelfType operator++(int) {
    SelfType temp(*this);
    ++*this;
    return temp;
  }

  SelfType operator--(int) {
    SelfType temp(*this);
    --*this;
    return temp;
  }

  SelfType& operator--() {
    decrement();
    return *this;
  }

  SelfType& operator++() {
    increment();
    return *this;
  }

  reference operator*() const { return deref(); }

  pointer operator->() const { return &deref(); }
};

} // namespace Json


namespace std {
/// Specialize std::swap() for Json::Value.
template<>
inline void swap(Json::Value& a, Json::Value& b) { a.swap(b); }
}

#if !defined(JSON_IS_AMALGAMATION)
#include "assertions.h"
#include "writer.h"
#include "value.h"
#endif // if !defined(JSON_IS_AMALGAMATION)
#include <math.h>
#include <sstream>
#include <utility>
#include <cstring>
#include <cassert>
#ifdef JSON_USE_CPPTL
#include <cpptl/conststring.h>
#endif
#include <cstddef> // size_t
#include <algorithm> // min()

#define JSON_ASSERT_UNREACHABLE assert(false)

namespace Json
{

// This is a walkaround to avoid the static initialization of Value::null.
// kNull must be word-aligned to avoid crashing on ARM.  We use an alignment of
// 8 (instead of 4) as a bit of future-proofing.
#if defined(__ARMEL__)
#define ALIGNAS(byte_alignment) __attribute__((aligned(byte_alignment)))
#else
#define ALIGNAS(byte_alignment)
#endif
//static const unsigned char ALIGNAS(8) kNull[sizeof(Value)] = { 0 };
//const unsigned char& kNullRef = kNull[0];
//const Value& Value::null = reinterpret_cast<const Value&>(kNullRef);
//const Value& Value::nullRef = null;

// static
Value const& Value::nullSingleton()
{
	static Value const nullStatic;
	return nullStatic;
}

// for backwards compatibility, we'll leave these global references around, but DO NOT
// use them in JSONCPP library code any more!
Value const& Value::null = Value::nullSingleton();
Value const& Value::nullRef = Value::nullSingleton();

#if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
template <typename T, typename U>
static inline bool InRange(double d, T min, U max) {
	// The casts can lose precision, but we are looking only for
	// an approximate range. Might fail on edge cases though. ~cdunn
	//return d >= static_cast<double>(min) && d <= static_cast<double>(max);
	return d >= min && d <= max;
}
#else  // if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
static inline double integerToDouble(Json::UInt64 value) {
	return static_cast<double>(Int64(value / 2)) * 2.0 + static_cast<double>(Int64(value & 1));
}

template <typename T> static inline double integerToDouble(T value) {
	return static_cast<double>(value);
}

template <typename T, typename U>
static inline bool InRange(double d, T min, U max) {
	return d >= integerToDouble(min) && d <= integerToDouble(max);
}
#endif // if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)


} // namespace Json

// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// ValueInternals...
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
#if !defined(JSON_IS_AMALGAMATION)

#include "json_valueiterator.inl"
#endif // if !defined(JSON_IS_AMALGAMATION)

#pragma pack(pop)

#if defined(JSONCPP_DISABLE_DLL_INTERFACE_WARNING)
#pragma warning(pop)
#endif // if defined(JSONCPP_DISABLE_DLL_INTERFACE_WARNING)

#endif // CPPTL_JSON_H_INCLUDED
