// Copyright 2007-2010 Baptiste Lepilleur
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#ifndef JSON_WRITER_H_INCLUDED
#define JSON_WRITER_H_INCLUDED

#if !defined(JSON_IS_AMALGAMATION)
#include "value.h"
#endif // if !defined(JSON_IS_AMALGAMATION)
#include <vector>
#include <string>
#include <ostream>

// Disable warning C4251: <data member>: <type> needs to have dll-interface to
// be used by...
#if defined(JSONCPP_DISABLE_DLL_INTERFACE_WARNING)
#pragma warning(push)
#pragma warning(disable : 4251)
#endif // if defined(JSONCPP_DISABLE_DLL_INTERFACE_WARNING)

#pragma pack(push, 8)

namespace Json {

class Value;

/**

Usage:
\code
  using namespace Json;
  void writeToStdout(StreamWriter::Factory const& factory, Value const& value) {
    std::unique_ptr<StreamWriter> const writer(
      factory.newStreamWriter());
    writer->write(value, &std::cout);
    std::cout << std::endl;  // add lf and flush
  }
\endcode
*/
class JSON_API StreamWriter {
protected:
  JSONCPP_OSTREAM* sout_;  // not owned; will not delete
public:
  StreamWriter();
  virtual ~StreamWriter();
  /** Write Value into document as configured in sub-class.
      Do not take ownership of sout, but maintain a reference during function.
      \pre sout != NULL
      \return zero on success (For now, we always return zero, so check the stream instead.)
      \throw std::exception possibly, depending on configuration
   */
  virtual int write(Value const& root, JSONCPP_OSTREAM* sout) = 0;

  /** \brief A simple abstract factory.
   */
  class JSON_API Factory {
  public:
    virtual ~Factory();
    /** \brief Allocate a CharReader via operator new().
     * \throw std::exception if something goes wrong (e.g. invalid settings)
     */
    virtual StreamWriter* newStreamWriter() const = 0;
  };  // Factory
};  // StreamWriter

/** \brief Write into stringstream, then return string, for convenience.
 * A StreamWriter will be created from the factory, used, and then deleted.
 */
JSONCPP_STRING JSON_API writeString(StreamWriter::Factory const& factory, Value const& root);


/** \brief Build a StreamWriter implementation.

Usage:
\code
  using namespace Json;
  Value value = ...;
  StreamWriterBuilder builder;
  builder["commentStyle"] = "None";
  builder["indentation"] = "   ";  // or whatever you like
  std::unique_ptr<Json::StreamWriter> writer(
      builder.newStreamWriter());
  writer->write(value, &std::cout);
  std::cout << std::endl;  // add lf and flush
\endcode
*/
class JSON_API StreamWriterBuilder : public StreamWriter::Factory {
public:
  // Note: We use a Json::Value so that we can add data-members to this class
  // without a major version bump.
  /** Configuration of this builder.
    Available settings (case-sensitive):
    - "commentStyle": "None" or "All"
    - "indentation":  "<anything>"
    - "enableYAMLCompatibility": false or true
      - slightly change the whitespace around colons
    - "dropNullPlaceholders": false or true
      - Drop the "null" string from the writer's output for nullValues.
        Strictly speaking, this is not valid JSON. But when the output is being
        fed to a browser's Javascript, it makes for smaller output and the
        browser can handle the output just fine.
    - "useSpecialFloats": false or true
      - If true, outputs non-finite floating point values in the following way:
        NaN values as "NaN", positive infinity as "Infinity", and negative infinity
        as "-Infinity".

    You can examine 'settings_` yourself
    to see the defaults. You can also write and read them just like any
    JSON Value.
    \sa setDefaults()
    */
  Json::Value settings_;

  StreamWriterBuilder();
  ~StreamWriterBuilder() JSONCPP_OVERRIDE;

  /**
   * \throw std::exception if something goes wrong (e.g. invalid settings)
   */
  StreamWriter* newStreamWriter() const JSONCPP_OVERRIDE;

  /** \return true if 'settings' are legal and consistent;
   *   otherwise, indicate bad settings via 'invalid'.
   */
  bool validate(Json::Value* invalid) const;
  /** A simple way to update a specific setting.
   */
  Value& operator[](JSONCPP_STRING key);

  /** Called by ctor, but you can use this to reset settings_.
   * \pre 'settings' != NULL (but Json::null is fine)
   * \remark Defaults:
   * \snippet src/lib_json/json_writer.cpp StreamWriterBuilderDefaults
   */
  static void setDefaults(Json::Value* settings);
};

/** \brief Abstract class for writers.
 * \deprecated Use StreamWriter. (And really, this is an implementation detail.)
 */
class JSON_API Writer {
public:
  virtual ~Writer();

  virtual JSONCPP_STRING write(const Value& root) = 0;
};

/** \brief Outputs a Value in <a HREF="http://www.json.org">JSON</a> format
 *without formatting (not human friendly).
 *
 * The JSON document is written in a single line. It is not intended for 'human'
 *consumption,
 * but may be usefull to support feature such as RPC where bandwith is limited.
 * \sa Reader, Value
 * \deprecated Use StreamWriterBuilder.
 */
class JSON_API FastWriter : public Writer {

public:
  FastWriter();
  ~FastWriter() JSONCPP_OVERRIDE {}

  void enableYAMLCompatibility();

  /** \brief Drop the "null" string from the writer's output for nullValues.
   * Strictly speaking, this is not valid JSON. But when the output is being
   * fed to a browser's Javascript, it makes for smaller output and the
   * browser can handle the output just fine.
   */
  void dropNullPlaceholders();

  void omitEndingLineFeed();

public: // overridden from Writer
  JSONCPP_STRING write(const Value& root) JSONCPP_OVERRIDE;

private:
  void writeValue(const Value& value);

  JSONCPP_STRING document_;
  bool yamlCompatiblityEnabled_;
  bool dropNullPlaceholders_;
  bool omitEndingLineFeed_;
};

/** \brief Writes a Value in <a HREF="http://www.json.org">JSON</a> format in a
 *human friendly way.
 *
 * The rules for line break and indent are as follow:
 * - Object value:
 *     - if empty then print {} without indent and line break
 *     - if not empty the print '{', line break & indent, print one value per
 *line
 *       and then unindent and line break and print '}'.
 * - Array value:
 *     - if empty then print [] without indent and line break
 *     - if the array contains no object value, empty array or some other value
 *types,
 *       and all the values fit on one lines, then print the array on a single
 *line.
 *     - otherwise, it the values do not fit on one line, or the array contains
 *       object or non empty array, then print one value per line.
 *
 * If the Value have comments then they are outputed according to their
 *#CommentPlacement.
 *
 * \sa Reader, Value, Value::setComment()
 * \deprecated Use StreamWriterBuilder.
 */
class JSON_API StyledWriter : public Writer {
public:
  StyledWriter();
  ~StyledWriter() JSONCPP_OVERRIDE {}

public: // overridden from Writer
  /** \brief Serialize a Value in <a HREF="http://www.json.org">JSON</a> format.
   * \param root Value to serialize.
   * \return String containing the JSON document that represents the root value.
   */
  JSONCPP_STRING write(const Value& root) JSONCPP_OVERRIDE;

private:
  void writeValue(const Value& value);
  void writeArrayValue(const Value& value);
  bool isMultineArray(const Value& value);
  void pushValue(const JSONCPP_STRING& value);
  void writeIndent();
  void writeWithIndent(const JSONCPP_STRING& value);
  void indent();
  void unindent();
  void writeCommentBeforeValue(const Value& root);
  void writeCommentAfterValueOnSameLine(const Value& root);
  bool hasCommentForValue(const Value& value);
  static JSONCPP_STRING normalizeEOL(const JSONCPP_STRING& text);

  typedef std::vector<JSONCPP_STRING> ChildValues;

  ChildValues childValues_;
  JSONCPP_STRING document_;
  JSONCPP_STRING indentString_;
  unsigned int rightMargin_;
  unsigned int indentSize_;
  bool addChildValues_;
};

/** \brief Writes a Value in <a HREF="http://www.json.org">JSON</a> format in a
 human friendly way,
     to a stream rather than to a string.
 *
 * The rules for line break and indent are as follow:
 * - Object value:
 *     - if empty then print {} without indent and line break
 *     - if not empty the print '{', line break & indent, print one value per
 line
 *       and then unindent and line break and print '}'.
 * - Array value:
 *     - if empty then print [] without indent and line break
 *     - if the array contains no object value, empty array or some other value
 types,
 *       and all the values fit on one lines, then print the array on a single
 line.
 *     - otherwise, it the values do not fit on one line, or the array contains
 *       object or non empty array, then print one value per line.
 *
 * If the Value have comments then they are outputed according to their
 #CommentPlacement.
 *
 * \param indentation Each level will be indented by this amount extra.
 * \sa Reader, Value, Value::setComment()
 * \deprecated Use StreamWriterBuilder.
 */
class JSON_API StyledStreamWriter {
public:
  StyledStreamWriter(JSONCPP_STRING indentation = "\t");
  ~StyledStreamWriter() {}

public:
  /** \brief Serialize a Value in <a HREF="http://www.json.org">JSON</a> format.
   * \param out Stream to write to. (Can be ostringstream, e.g.)
   * \param root Value to serialize.
   * \note There is no point in deriving from Writer, since write() should not
   * return a value.
   */
  void write(JSONCPP_OSTREAM& out, const Value& root);

private:
  void writeValue(const Value& value);
  void writeArrayValue(const Value& value);
  bool isMultineArray(const Value& value);
  void pushValue(const JSONCPP_STRING& value);
  void writeIndent();
  void writeWithIndent(const JSONCPP_STRING& value);
  void indent();
  void unindent();
  void writeCommentBeforeValue(const Value& root);
  void writeCommentAfterValueOnSameLine(const Value& root);
  bool hasCommentForValue(const Value& value);
  static JSONCPP_STRING normalizeEOL(const JSONCPP_STRING& text);

  typedef std::vector<JSONCPP_STRING> ChildValues;

  ChildValues childValues_;
  JSONCPP_OSTREAM* document_;
  JSONCPP_STRING indentString_;
  unsigned int rightMargin_;
  JSONCPP_STRING indentation_;
  bool addChildValues_ : 1;
  bool indented_ : 1;
};

#if defined(JSON_HAS_INT64)
JSONCPP_STRING JSON_API valueToString(Int value);
JSONCPP_STRING JSON_API valueToString(UInt value);
#endif // if defined(JSON_HAS_INT64)
JSONCPP_STRING JSON_API valueToString(LargestInt value);
JSONCPP_STRING JSON_API valueToString(LargestUInt value);
JSONCPP_STRING JSON_API valueToString(double value);
JSONCPP_STRING JSON_API valueToString(bool value);
JSONCPP_STRING JSON_API valueToQuotedString(const char* value);

/// \brief Output using the StyledStreamWriter.
/// \see Json::operator>>()
JSON_API JSONCPP_OSTREAM& operator<<(JSONCPP_OSTREAM&, const Value& root);

} // namespace Json

#if !defined(JSON_IS_AMALGAMATION)
#include "writer.h"
#include "json_tool.h"
#endif // if !defined(JSON_IS_AMALGAMATION)
#include <iomanip>
#include <memory>
#include <sstream>
#include <utility>
#include <set>
#include <cassert>
#include <cstring>
#include <cstdio>

#if defined(_MSC_VER) && _MSC_VER >= 1200 && _MSC_VER < 1800 // Between VC++ 6.0 and VC++ 11.0
#include <float.h>
#define isfinite _finite
#elif defined(__sun) && defined(__SVR4) //Solaris
#if !defined(isfinite)
#include <ieeefp.h>
#define isfinite finite
#endif
#elif defined(_AIX)
#if !defined(isfinite)
#include <math.h>
#define isfinite finite
#endif
#elif defined(__hpux)
#if !defined(isfinite)
#if defined(__ia64) && !defined(finite)
#define isfinite(x) ((sizeof(x) == sizeof(float) ? \
                     _Isfinitef(x) : _IsFinite(x)))
#else
#include <math.h>
#define isfinite finite
#endif
#endif
#else
#include <cmath>
#if !(defined(__QNXNTO__)) // QNX already defines isfinite
#define isfinite std::isfinite
#endif
#endif

#if defined(_MSC_VER)
#if !defined(WINCE) && defined(__STDC_SECURE_LIB__) && _MSC_VER >= 1500 // VC++ 9.0 and above
#define snprintf sprintf_s
#elif _MSC_VER >= 1900 // VC++ 14.0 and above
#define snprintf std::snprintf
#else
#define snprintf _snprintf
#endif
#elif defined(__ANDROID__) || defined(__QNXNTO__)
#define snprintf snprintf
#elif __cplusplus >= 201103L
#if !defined(__MINGW32__) && !defined(__CYGWIN__)
#define snprintf std::snprintf
#endif
#endif

#if defined(__BORLANDC__)  
#include <float.h>
#define isfinite _finite
#define snprintf _snprintf
#endif

#if defined(_MSC_VER) && _MSC_VER >= 1400 // VC++ 8.0
// Disable warning about strdup being deprecated.
#pragma warning(disable : 4996)
#endif

namespace Json
{

#if __cplusplus >= 201103L || (defined(_CPPLIB_VER) && _CPPLIB_VER >= 520)
typedef std::unique_ptr<StreamWriter> StreamWriterPtr;
#else
typedef std::auto_ptr<StreamWriter>   StreamWriterPtr;
#endif

static bool containsControlCharacter(const char* str) {
	while (*str) {
		if (isControlCharacter(*(str++)))
			return true;
	}
	return false;
}

static bool containsControlCharacter0(const char* str, unsigned len) {
	char const* end = str + len;
	while (end != str) {
		if (isControlCharacter(*str) || 0 == *str)
			return true;
		++str;
	}
	return false;
}

JSONCPP_STRING valueToString(LargestInt value) {
	UIntToStringBuffer buffer;
	char* current = buffer + sizeof(buffer);
	if (value == Value::minLargestInt) {
		uintToString(LargestUInt(Value::maxLargestInt) + 1, current);
		*--current = '-';
	} else if (value < 0) {
		uintToString(LargestUInt(-value), current);
		*--current = '-';
	} else {
		uintToString(LargestUInt(value), current);
	}
	assert(current >= buffer);
	return current;
}

JSONCPP_STRING valueToString(LargestUInt value) {
	UIntToStringBuffer buffer;
	char* current = buffer + sizeof(buffer);
	uintToString(value, current);
	assert(current >= buffer);
	return current;
}

#if defined(JSON_HAS_INT64)

JSONCPP_STRING valueToString(Int value) {
	return valueToString(LargestInt(value));
}

JSONCPP_STRING valueToString(UInt value) {
	return valueToString(LargestUInt(value));
}

#endif // # if defined(JSON_HAS_INT64)

namespace
{
JSONCPP_STRING valueToString(double value, bool useSpecialFloats, unsigned int precision) {
	// Allocate a buffer that is more than large enough to store the 16 digits of
	// precision requested below.
	char buffer[36];
	int len = -1;

	char formatString[6];
	sprintf(formatString, "%%.%dg", precision);

	// Print into the buffer. We need not request the alternative representation
	// that always has a decimal point because JSON doesn't distingish the
	// concepts of reals and integers.
	if (isfinite(value)) {
		len = snprintf(buffer, sizeof(buffer), formatString, value);

		// try to ensure we preserve the fact that this was given to us as a double on input
		if (!strstr(buffer, ".") && !strstr(buffer, "e")) {
			strcat(buffer, ".0");
		}

	} else {
		// IEEE standard states that NaN values will not compare to themselves
		if (value != value) {
			len = snprintf(buffer, sizeof(buffer), useSpecialFloats ? "NaN" : "null");
		} else if (value < 0) {
			len = snprintf(buffer, sizeof(buffer), useSpecialFloats ? "-Infinity" : "-1e+9999");
		} else {
			len = snprintf(buffer, sizeof(buffer), useSpecialFloats ? "Infinity" : "1e+9999");
		}
		// For those, we do not need to call fixNumLoc, but it is fast.
	}
	assert(len >= 0);
	fixNumericLocale(buffer, buffer + len);
	return buffer;
}
}

JSONCPP_STRING valueToString(double value) { return valueToString(value, false, 17); }

JSONCPP_STRING valueToString(bool value) { return value ? "true" : "false"; }

JSONCPP_STRING valueToQuotedString(const char* value) {
	if (value == NULL)
		return "";
	// Not sure how to handle unicode...
	if (strpbrk(value, "\"\\\b\f\n\r\t") == NULL &&
		!containsControlCharacter(value))
		return JSONCPP_STRING("\"") + value + "\"";
	// We have to walk value and escape any special characters.
	// Appending to JSONCPP_STRING is not efficient, but this should be rare.
	// (Note: forward slashes are *not* rare, but I am not escaping them.)
	JSONCPP_STRING::size_type maxsize =
		strlen(value) * 2 + 3; // allescaped+quotes+NULL
	JSONCPP_STRING result;
	result.reserve(maxsize); // to avoid lots of mallocs
	result += "\"";
	for (const char* c = value; *c != 0; ++c) {
		switch (*c) {
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
				// case '/':
				// Even though \/ is considered a legal escape in JSON, a bare
				// slash is also legal, so I see no reason to escape it.
				// (I hope I am not misunderstanding something.
				// blep notes: actually escaping \/ may be useful in javascript to avoid </
				// sequence.
				// Should add a flag to allow this compatibility mode and prevent this
				// sequence from occurring.
			default:
				if (isControlCharacter(*c)) {
					JSONCPP_OSTRINGSTREAM oss;
					oss << "\\u" << std::hex << std::uppercase << std::setfill('0')
						<< std::setw(4) << static_cast<int>(*c);
					result += oss.str();
				} else {
					result += *c;
				}
				break;
		}
	}
	result += "\"";
	return result;
}

// https://github.com/upcaste/upcaste/blob/master/src/upcore/src/cstring/strnpbrk.cpp
static char const* strnpbrk(char const* s, char const* accept, size_t n) {
	assert((s || !n) && accept);

	char const* const end = s + n;
	for (char const* cur = s; cur < end; ++cur) {
		int const c = *cur;
		for (char const* a = accept; *a; ++a) {
			if (*a == c) {
				return cur;
			}
		}
	}
	return NULL;
}
static JSONCPP_STRING valueToQuotedStringN(const char* value, unsigned length) {
	if (value == NULL)
		return "";
	// Not sure how to handle unicode...
	if (strnpbrk(value, "\"\\\b\f\n\r\t", length) == NULL &&
		!containsControlCharacter0(value, length))
		return JSONCPP_STRING("\"") + value + "\"";
	// We have to walk value and escape any special characters.
	// Appending to JSONCPP_STRING is not efficient, but this should be rare.
	// (Note: forward slashes are *not* rare, but I am not escaping them.)
	JSONCPP_STRING::size_type maxsize =
		length * 2 + 3; // allescaped+quotes+NULL
	JSONCPP_STRING result;
	result.reserve(maxsize); // to avoid lots of mallocs
	result += "\"";
	char const* end = value + length;
	for (const char* c = value; c != end; ++c) {
		switch (*c) {
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
				// case '/':
				// Even though \/ is considered a legal escape in JSON, a bare
				// slash is also legal, so I see no reason to escape it.
				// (I hope I am not misunderstanding something.)
				// blep notes: actually escaping \/ may be useful in javascript to avoid </
				// sequence.
				// Should add a flag to allow this compatibility mode and prevent this
				// sequence from occurring.
			default:
				if ((isControlCharacter(*c)) || (*c == 0)) {
					JSONCPP_OSTRINGSTREAM oss;
					oss << "\\u" << std::hex << std::uppercase << std::setfill('0')
						<< std::setw(4) << static_cast<int>(*c);
					result += oss.str();
				} else {
					result += *c;
				}
				break;
		}
	}
	result += "\"";
	return result;
}

// Class Writer
// //////////////////////////////////////////////////////////////////
Writer::~Writer() {}

// Class FastWriter
// //////////////////////////////////////////////////////////////////

FastWriter::FastWriter()
	: yamlCompatiblityEnabled_(false), dropNullPlaceholders_(false),
	omitEndingLineFeed_(false) {}

void FastWriter::enableYAMLCompatibility() { yamlCompatiblityEnabled_ = true; }

void FastWriter::dropNullPlaceholders() { dropNullPlaceholders_ = true; }

void FastWriter::omitEndingLineFeed() { omitEndingLineFeed_ = true; }

JSONCPP_STRING FastWriter::write(const Value& root) {
	document_ = "";
	writeValue(root);
	if (!omitEndingLineFeed_)
		document_ += "\n";
	return document_;
}

void FastWriter::writeValue(const Value& value) {
	switch (value.type()) {
		case nullValue:
			if (!dropNullPlaceholders_)
				document_ += "null";
			break;
		case intValue:
			document_ += valueToString(value.asLargestInt());
			break;
		case uintValue:
			document_ += valueToString(value.asLargestUInt());
			break;
		case realValue:
			document_ += valueToString(value.asDouble());
			break;
		case stringValue:
		{
			// Is NULL possible for value.string_? No.
			char const* str;
			char const* end;
			bool ok = value.getString(&str, &end);
			if (ok) document_ += valueToQuotedStringN(str, static_cast<unsigned>(end - str));
			break;
		}
		case booleanValue:
			document_ += valueToString(value.asBool());
			break;
		case arrayValue:
		{
			document_ += '[';
			ArrayIndex size = value.size();
			for (ArrayIndex index = 0; index < size; ++index) {
				if (index > 0)
					document_ += ',';
				writeValue(value[index]);
			}
			document_ += ']';
		} break;
		case objectValue:
		{
			Value::Members members(value.getMemberNames());
			document_ += '{';
			for (Value::Members::iterator it = members.begin(); it != members.end();
				 ++it) {
				const JSONCPP_STRING& name = *it;
				if (it != members.begin())
					document_ += ',';
				document_ += valueToQuotedStringN(name.data(), static_cast<unsigned>(name.length()));
				document_ += yamlCompatiblityEnabled_ ? ": " : ":";
				writeValue(value[name]);
			}
			document_ += '}';
		} break;
	}
}

// Class StyledWriter
// //////////////////////////////////////////////////////////////////

StyledWriter::StyledWriter()
	: rightMargin_(74), indentSize_(3), addChildValues_() {}

JSONCPP_STRING StyledWriter::write(const Value& root) {
	document_ = "";
	addChildValues_ = false;
	indentString_ = "";
	writeCommentBeforeValue(root);
	writeValue(root);
	writeCommentAfterValueOnSameLine(root);
	document_ += "\n";
	return document_;
}

void StyledWriter::writeValue(const Value& value) {
	switch (value.type()) {
		case nullValue:
			pushValue("null");
			break;
		case intValue:
			pushValue(valueToString(value.asLargestInt()));
			break;
		case uintValue:
			pushValue(valueToString(value.asLargestUInt()));
			break;
		case realValue:
			pushValue(valueToString(value.asDouble()));
			break;
		case stringValue:
		{
			// Is NULL possible for value.string_? No.
			char const* str;
			char const* end;
			bool ok = value.getString(&str, &end);
			if (ok) pushValue(valueToQuotedStringN(str, static_cast<unsigned>(end - str)));
			else pushValue("");
			break;
		}
		case booleanValue:
			pushValue(valueToString(value.asBool()));
			break;
		case arrayValue:
			writeArrayValue(value);
			break;
		case objectValue:
		{
			Value::Members members(value.getMemberNames());
			if (members.empty())
				pushValue("{}");
			else {
				writeWithIndent("{");
				indent();
				Value::Members::iterator it = members.begin();
				for (;;) {
					const JSONCPP_STRING& name = *it;
					const Value& childValue = value[name];
					writeCommentBeforeValue(childValue);
					writeWithIndent(valueToQuotedString(name.c_str()));
					document_ += " : ";
					writeValue(childValue);
					if (++it == members.end()) {
						writeCommentAfterValueOnSameLine(childValue);
						break;
					}
					document_ += ',';
					writeCommentAfterValueOnSameLine(childValue);
				}
				unindent();
				writeWithIndent("}");
			}
		} break;
	}
}

void StyledWriter::writeArrayValue(const Value& value) {
	unsigned size = value.size();
	if (size == 0)
		pushValue("[]");
	else {
		bool isArrayMultiLine = isMultineArray(value);
		if (isArrayMultiLine) {
			writeWithIndent("[");
			indent();
			bool hasChildValue = !childValues_.empty();
			unsigned index = 0;
			for (;;) {
				const Value& childValue = value[index];
				writeCommentBeforeValue(childValue);
				if (hasChildValue)
					writeWithIndent(childValues_[index]);
				else {
					writeIndent();
					writeValue(childValue);
				}
				if (++index == size) {
					writeCommentAfterValueOnSameLine(childValue);
					break;
				}
				document_ += ',';
				writeCommentAfterValueOnSameLine(childValue);
			}
			unindent();
			writeWithIndent("]");
		} else // output on a single line
		{
			assert(childValues_.size() == size);
			document_ += "[ ";
			for (unsigned index = 0; index < size; ++index) {
				if (index > 0)
					document_ += ", ";
				document_ += childValues_[index];
			}
			document_ += " ]";
		}
	}
}

bool StyledWriter::isMultineArray(const Value& value) {
	ArrayIndex const size = value.size();
	bool isMultiLine = size * 3 >= rightMargin_;
	childValues_.clear();
	for (ArrayIndex index = 0; index < size && !isMultiLine; ++index) {
		const Value& childValue = value[index];
		isMultiLine = ((childValue.isArray() || childValue.isObject()) &&
					   childValue.size() > 0);
	}
	if (!isMultiLine) // check if line length > max line length
	{
		childValues_.reserve(size);
		addChildValues_ = true;
		ArrayIndex lineLength = 4 + (size - 1) * 2; // '[ ' + ', '*n + ' ]'
		for (ArrayIndex index = 0; index < size; ++index) {
			if (hasCommentForValue(value[index])) {
				isMultiLine = true;
			}
			writeValue(value[index]);
			lineLength += static_cast<ArrayIndex>(childValues_[index].length());
		}
		addChildValues_ = false;
		isMultiLine = isMultiLine || lineLength >= rightMargin_;
	}
	return isMultiLine;
}

void StyledWriter::pushValue(const JSONCPP_STRING& value) {
	if (addChildValues_)
		childValues_.push_back(value);
	else
		document_ += value;
}

void StyledWriter::writeIndent() {
	if (!document_.empty()) {
		char last = document_[document_.length() - 1];
		if (last == ' ') // already indented
			return;
		if (last != '\n') // Comments may add new-line
			document_ += '\n';
	}
	document_ += indentString_;
}

void StyledWriter::writeWithIndent(const JSONCPP_STRING& value) {
	writeIndent();
	document_ += value;
}

void StyledWriter::indent() { indentString_ += JSONCPP_STRING(indentSize_, ' '); }

void StyledWriter::unindent() {
	assert(indentString_.size() >= indentSize_);
	indentString_.resize(indentString_.size() - indentSize_);
}

void StyledWriter::writeCommentBeforeValue(const Value& root) {
	if (!root.hasComment(commentBefore))
		return;

	document_ += "\n";
	writeIndent();
	const JSONCPP_STRING& comment = root.getComment(commentBefore);
	JSONCPP_STRING::const_iterator iter = comment.begin();
	while (iter != comment.end()) {
		document_ += *iter;
		if (*iter == '\n' &&
			(iter != comment.end() && *(iter + 1) == '/'))
			writeIndent();
		++iter;
	}

	// Comments are stripped of trailing newlines, so add one here
	document_ += "\n";
}

void StyledWriter::writeCommentAfterValueOnSameLine(const Value& root) {
	if (root.hasComment(commentAfterOnSameLine))
		document_ += " " + root.getComment(commentAfterOnSameLine);

	if (root.hasComment(commentAfter)) {
		document_ += "\n";
		document_ += root.getComment(commentAfter);
		document_ += "\n";
	}
}

bool StyledWriter::hasCommentForValue(const Value& value) {
	return value.hasComment(commentBefore) ||
		value.hasComment(commentAfterOnSameLine) ||
		value.hasComment(commentAfter);
}

// Class StyledStreamWriter
// //////////////////////////////////////////////////////////////////

StyledStreamWriter::StyledStreamWriter(JSONCPP_STRING indentation)
	: document_(NULL), rightMargin_(74), indentation_(indentation),
	addChildValues_() {}

void StyledStreamWriter::write(JSONCPP_OSTREAM& out, const Value& root) {
	document_ = &out;
	addChildValues_ = false;
	indentString_ = "";
	indented_ = true;
	writeCommentBeforeValue(root);
	if (!indented_) writeIndent();
	indented_ = true;
	writeValue(root);
	writeCommentAfterValueOnSameLine(root);
	*document_ << "\n";
	document_ = NULL; // Forget the stream, for safety.
}

void StyledStreamWriter::writeValue(const Value& value) {
	switch (value.type()) {
		case nullValue:
			pushValue("null");
			break;
		case intValue:
			pushValue(valueToString(value.asLargestInt()));
			break;
		case uintValue:
			pushValue(valueToString(value.asLargestUInt()));
			break;
		case realValue:
			pushValue(valueToString(value.asDouble()));
			break;
		case stringValue:
		{
			// Is NULL possible for value.string_? No.
			char const* str;
			char const* end;
			bool ok = value.getString(&str, &end);
			if (ok) pushValue(valueToQuotedStringN(str, static_cast<unsigned>(end - str)));
			else pushValue("");
			break;
		}
		case booleanValue:
			pushValue(valueToString(value.asBool()));
			break;
		case arrayValue:
			writeArrayValue(value);
			break;
		case objectValue:
		{
			Value::Members members(value.getMemberNames());
			if (members.empty())
				pushValue("{}");
			else {
				writeWithIndent("{");
				indent();
				Value::Members::iterator it = members.begin();
				for (;;) {
					const JSONCPP_STRING& name = *it;
					const Value& childValue = value[name];
					writeCommentBeforeValue(childValue);
					writeWithIndent(valueToQuotedString(name.c_str()));
					*document_ << " : ";
					writeValue(childValue);
					if (++it == members.end()) {
						writeCommentAfterValueOnSameLine(childValue);
						break;
					}
					*document_ << ",";
					writeCommentAfterValueOnSameLine(childValue);
				}
				unindent();
				writeWithIndent("}");
			}
		} break;
	}
}

void StyledStreamWriter::writeArrayValue(const Value& value) {
	unsigned size = value.size();
	if (size == 0)
		pushValue("[]");
	else {
		bool isArrayMultiLine = isMultineArray(value);
		if (isArrayMultiLine) {
			writeWithIndent("[");
			indent();
			bool hasChildValue = !childValues_.empty();
			unsigned index = 0;
			for (;;) {
				const Value& childValue = value[index];
				writeCommentBeforeValue(childValue);
				if (hasChildValue)
					writeWithIndent(childValues_[index]);
				else {
					if (!indented_) writeIndent();
					indented_ = true;
					writeValue(childValue);
					indented_ = false;
				}
				if (++index == size) {
					writeCommentAfterValueOnSameLine(childValue);
					break;
				}
				*document_ << ",";
				writeCommentAfterValueOnSameLine(childValue);
			}
			unindent();
			writeWithIndent("]");
		} else // output on a single line
		{
			assert(childValues_.size() == size);
			*document_ << "[ ";
			for (unsigned index = 0; index < size; ++index) {
				if (index > 0)
					*document_ << ", ";
				*document_ << childValues_[index];
			}
			*document_ << " ]";
		}
	}
}

bool StyledStreamWriter::isMultineArray(const Value& value) {
	ArrayIndex const size = value.size();
	bool isMultiLine = size * 3 >= rightMargin_;
	childValues_.clear();
	for (ArrayIndex index = 0; index < size && !isMultiLine; ++index) {
		const Value& childValue = value[index];
		isMultiLine = ((childValue.isArray() || childValue.isObject()) &&
					   childValue.size() > 0);
	}
	if (!isMultiLine) // check if line length > max line length
	{
		childValues_.reserve(size);
		addChildValues_ = true;
		ArrayIndex lineLength = 4 + (size - 1) * 2; // '[ ' + ', '*n + ' ]'
		for (ArrayIndex index = 0; index < size; ++index) {
			if (hasCommentForValue(value[index])) {
				isMultiLine = true;
			}
			writeValue(value[index]);
			lineLength += static_cast<ArrayIndex>(childValues_[index].length());
		}
		addChildValues_ = false;
		isMultiLine = isMultiLine || lineLength >= rightMargin_;
	}
	return isMultiLine;
}

void StyledStreamWriter::pushValue(const JSONCPP_STRING& value) {
	if (addChildValues_)
		childValues_.push_back(value);
	else
		*document_ << value;
}

void StyledStreamWriter::writeIndent() {
	// blep intended this to look at the so-far-written string
	// to determine whether we are already indented, but
	// with a stream we cannot do that. So we rely on some saved state.
	// The caller checks indented_.
	*document_ << '\n' << indentString_;
}

void StyledStreamWriter::writeWithIndent(const JSONCPP_STRING& value) {
	if (!indented_) writeIndent();
	*document_ << value;
	indented_ = false;
}

void StyledStreamWriter::indent() { indentString_ += indentation_; }

void StyledStreamWriter::unindent() {
	assert(indentString_.size() >= indentation_.size());
	indentString_.resize(indentString_.size() - indentation_.size());
}

void StyledStreamWriter::writeCommentBeforeValue(const Value& root) {
	if (!root.hasComment(commentBefore))
		return;

	if (!indented_) writeIndent();
	const JSONCPP_STRING& comment = root.getComment(commentBefore);
	JSONCPP_STRING::const_iterator iter = comment.begin();
	while (iter != comment.end()) {
		*document_ << *iter;
		if (*iter == '\n' &&
			(iter != comment.end() && *(iter + 1) == '/'))
			// writeIndent();  // would include newline
			*document_ << indentString_;
		++iter;
	}
	indented_ = false;
}

void StyledStreamWriter::writeCommentAfterValueOnSameLine(const Value& root) {
	if (root.hasComment(commentAfterOnSameLine))
		*document_ << ' ' << root.getComment(commentAfterOnSameLine);

	if (root.hasComment(commentAfter)) {
		writeIndent();
		*document_ << root.getComment(commentAfter);
	}
	indented_ = false;
}

bool StyledStreamWriter::hasCommentForValue(const Value& value) {
	return value.hasComment(commentBefore) ||
		value.hasComment(commentAfterOnSameLine) ||
		value.hasComment(commentAfter);
}

//////////////////////////
// BuiltStyledStreamWriter

/// Scoped enums are not available until C++11.
struct CommentStyle {
	/// Decide whether to write comments.
	enum Enum {
		None,  ///< Drop all comments.
		Most,  ///< Recover odd behavior of previous versions (not implemented yet).
		All  ///< Keep all comments.
	};
};

struct BuiltStyledStreamWriter : public StreamWriter
{
	BuiltStyledStreamWriter(
		JSONCPP_STRING const& indentation,
		CommentStyle::Enum cs,
		JSONCPP_STRING const& colonSymbol,
		JSONCPP_STRING const& nullSymbol,
		JSONCPP_STRING const& endingLineFeedSymbol,
		bool useSpecialFloats,
		unsigned int precision);
	int write(Value const& root, JSONCPP_OSTREAM* sout) JSONCPP_OVERRIDE;
private:
	void writeValue(Value const& value);
	void writeArrayValue(Value const& value);
	bool isMultineArray(Value const& value);
	void pushValue(JSONCPP_STRING const& value);
	void writeIndent();
	void writeWithIndent(JSONCPP_STRING const& value);
	void indent();
	void unindent();
	void writeCommentBeforeValue(Value const& root);
	void writeCommentAfterValueOnSameLine(Value const& root);
	static bool hasCommentForValue(const Value& value);

	typedef std::vector<JSONCPP_STRING> ChildValues;

	ChildValues childValues_;
	JSONCPP_STRING indentString_;
	unsigned int rightMargin_;
	JSONCPP_STRING indentation_;
	CommentStyle::Enum cs_;
	JSONCPP_STRING colonSymbol_;
	JSONCPP_STRING nullSymbol_;
	JSONCPP_STRING endingLineFeedSymbol_;
	bool addChildValues_ : 1;
	bool indented_ : 1;
	bool useSpecialFloats_ : 1;
	unsigned int precision_;
};
BuiltStyledStreamWriter::BuiltStyledStreamWriter(
	JSONCPP_STRING const& indentation,
	CommentStyle::Enum cs,
	JSONCPP_STRING const& colonSymbol,
	JSONCPP_STRING const& nullSymbol,
	JSONCPP_STRING const& endingLineFeedSymbol,
	bool useSpecialFloats,
	unsigned int precision)
	: rightMargin_(74)
	, indentation_(indentation)
	, cs_(cs)
	, colonSymbol_(colonSymbol)
	, nullSymbol_(nullSymbol)
	, endingLineFeedSymbol_(endingLineFeedSymbol)
	, addChildValues_(false)
	, indented_(false)
	, useSpecialFloats_(useSpecialFloats)
	, precision_(precision)
{
}
int BuiltStyledStreamWriter::write(Value const& root, JSONCPP_OSTREAM* sout)
{
	sout_ = sout;
	addChildValues_ = false;
	indented_ = true;
	indentString_ = "";
	writeCommentBeforeValue(root);
	if (!indented_) writeIndent();
	indented_ = true;
	writeValue(root);
	writeCommentAfterValueOnSameLine(root);
	*sout_ << endingLineFeedSymbol_;
	sout_ = NULL;
	return 0;
}
void BuiltStyledStreamWriter::writeValue(Value const& value) {
	switch (value.type()) {
		case nullValue:
			pushValue(nullSymbol_);
			break;
		case intValue:
			pushValue(valueToString(value.asLargestInt()));
			break;
		case uintValue:
			pushValue(valueToString(value.asLargestUInt()));
			break;
		case realValue:
			pushValue(valueToString(value.asDouble(), useSpecialFloats_, precision_));
			break;
		case stringValue:
		{
			// Is NULL is possible for value.string_? No.
			char const* str;
			char const* end;
			bool ok = value.getString(&str, &end);
			if (ok) pushValue(valueToQuotedStringN(str, static_cast<unsigned>(end - str)));
			else pushValue("");
			break;
		}
		case booleanValue:
			pushValue(valueToString(value.asBool()));
			break;
		case arrayValue:
			writeArrayValue(value);
			break;
		case objectValue:
		{
			Value::Members members(value.getMemberNames());
			if (members.empty())
				pushValue("{}");
			else {
				writeWithIndent("{");
				indent();
				Value::Members::iterator it = members.begin();
				for (;;) {
					JSONCPP_STRING const& name = *it;
					Value const& childValue = value[name];
					writeCommentBeforeValue(childValue);
					writeWithIndent(valueToQuotedStringN(name.data(), static_cast<unsigned>(name.length())));
					*sout_ << colonSymbol_;
					writeValue(childValue);
					if (++it == members.end()) {
						writeCommentAfterValueOnSameLine(childValue);
						break;
					}
					*sout_ << ",";
					writeCommentAfterValueOnSameLine(childValue);
				}
				unindent();
				writeWithIndent("}");
			}
		} break;
	}
}

void BuiltStyledStreamWriter::writeArrayValue(Value const& value) {
	unsigned size = value.size();
	if (size == 0)
		pushValue("[]");
	else {
		bool isMultiLine = (cs_ == CommentStyle::All) || isMultineArray(value);
		if (isMultiLine) {
			writeWithIndent("[");
			indent();
			bool hasChildValue = !childValues_.empty();
			unsigned index = 0;
			for (;;) {
				Value const& childValue = value[index];
				writeCommentBeforeValue(childValue);
				if (hasChildValue)
					writeWithIndent(childValues_[index]);
				else {
					if (!indented_) writeIndent();
					indented_ = true;
					writeValue(childValue);
					indented_ = false;
				}
				if (++index == size) {
					writeCommentAfterValueOnSameLine(childValue);
					break;
				}
				*sout_ << ",";
				writeCommentAfterValueOnSameLine(childValue);
			}
			unindent();
			writeWithIndent("]");
		} else // output on a single line
		{
			assert(childValues_.size() == size);
			*sout_ << "[";
			if (!indentation_.empty()) *sout_ << " ";
			for (unsigned index = 0; index < size; ++index) {
				if (index > 0)
					*sout_ << ((!indentation_.empty()) ? ", " : ",");
				*sout_ << childValues_[index];
			}
			if (!indentation_.empty()) *sout_ << " ";
			*sout_ << "]";
		}
	}
}

bool BuiltStyledStreamWriter::isMultineArray(Value const& value) {
	ArrayIndex const size = value.size();
	bool isMultiLine = size * 3 >= rightMargin_;
	childValues_.clear();
	for (ArrayIndex index = 0; index < size && !isMultiLine; ++index) {
		Value const& childValue = value[index];
		isMultiLine = ((childValue.isArray() || childValue.isObject()) &&
					   childValue.size() > 0);
	}
	if (!isMultiLine) // check if line length > max line length
	{
		childValues_.reserve(size);
		addChildValues_ = true;
		ArrayIndex lineLength = 4 + (size - 1) * 2; // '[ ' + ', '*n + ' ]'
		for (ArrayIndex index = 0; index < size; ++index) {
			if (hasCommentForValue(value[index])) {
				isMultiLine = true;
			}
			writeValue(value[index]);
			lineLength += static_cast<ArrayIndex>(childValues_[index].length());
		}
		addChildValues_ = false;
		isMultiLine = isMultiLine || lineLength >= rightMargin_;
	}
	return isMultiLine;
}

void BuiltStyledStreamWriter::pushValue(JSONCPP_STRING const& value) {
	if (addChildValues_)
		childValues_.push_back(value);
	else
		*sout_ << value;
}

void BuiltStyledStreamWriter::writeIndent() {
	// blep intended this to look at the so-far-written string
	// to determine whether we are already indented, but
	// with a stream we cannot do that. So we rely on some saved state.
	// The caller checks indented_.

	if (!indentation_.empty()) {
		// In this case, drop newlines too.
		*sout_ << '\n' << indentString_;
	}
}

void BuiltStyledStreamWriter::writeWithIndent(JSONCPP_STRING const& value) {
	if (!indented_) writeIndent();
	*sout_ << value;
	indented_ = false;
}

void BuiltStyledStreamWriter::indent() { indentString_ += indentation_; }

void BuiltStyledStreamWriter::unindent() {
	assert(indentString_.size() >= indentation_.size());
	indentString_.resize(indentString_.size() - indentation_.size());
}

void BuiltStyledStreamWriter::writeCommentBeforeValue(Value const& root) {
	if (cs_ == CommentStyle::None) return;
	if (!root.hasComment(commentBefore))
		return;

	if (!indented_) writeIndent();
	const JSONCPP_STRING& comment = root.getComment(commentBefore);
	JSONCPP_STRING::const_iterator iter = comment.begin();
	while (iter != comment.end()) {
		*sout_ << *iter;
		if (*iter == '\n' &&
			(iter != comment.end() && *(iter + 1) == '/'))
			// writeIndent();  // would write extra newline
			*sout_ << indentString_;
		++iter;
	}
	indented_ = false;
}

void BuiltStyledStreamWriter::writeCommentAfterValueOnSameLine(Value const& root) {
	if (cs_ == CommentStyle::None) return;
	if (root.hasComment(commentAfterOnSameLine))
		*sout_ << " " + root.getComment(commentAfterOnSameLine);

	if (root.hasComment(commentAfter)) {
		writeIndent();
		*sout_ << root.getComment(commentAfter);
	}
}

// static
bool BuiltStyledStreamWriter::hasCommentForValue(const Value& value) {
	return value.hasComment(commentBefore) ||
		value.hasComment(commentAfterOnSameLine) ||
		value.hasComment(commentAfter);
}

///////////////
// StreamWriter

StreamWriter::StreamWriter()
	: sout_(NULL)
{
}
StreamWriter::~StreamWriter()
{
}
StreamWriter::Factory::~Factory()
{}
StreamWriterBuilder::StreamWriterBuilder()
{
	setDefaults(&settings_);
}
StreamWriterBuilder::~StreamWriterBuilder()
{}
StreamWriter* StreamWriterBuilder::newStreamWriter() const
{
	JSONCPP_STRING indentation = settings_["indentation"].asString();
	JSONCPP_STRING cs_str = settings_["commentStyle"].asString();
	bool eyc = settings_["enableYAMLCompatibility"].asBool();
	bool dnp = settings_["dropNullPlaceholders"].asBool();
	bool usf = settings_["useSpecialFloats"].asBool();
	unsigned int pre = settings_["precision"].asUInt();
	CommentStyle::Enum cs = CommentStyle::All;
	if (cs_str == "All") {
		cs = CommentStyle::All;
	} else if (cs_str == "None") {
		cs = CommentStyle::None;
	} else {
		throwRuntimeError("commentStyle must be 'All' or 'None'");
	}
	JSONCPP_STRING colonSymbol = " : ";
	if (eyc) {
		colonSymbol = ": ";
	} else if (indentation.empty()) {
		colonSymbol = ":";
	}
	JSONCPP_STRING nullSymbol = "null";
	if (dnp) {
		nullSymbol = "";
	}
	if (pre > 17) pre = 17;
	JSONCPP_STRING endingLineFeedSymbol = "";
	return new BuiltStyledStreamWriter(
		indentation, cs,
		colonSymbol, nullSymbol, endingLineFeedSymbol, usf, pre);
}
static void getValidWriterKeys(std::set<JSONCPP_STRING>* valid_keys)
{
	valid_keys->clear();
	valid_keys->insert("indentation");
	valid_keys->insert("commentStyle");
	valid_keys->insert("enableYAMLCompatibility");
	valid_keys->insert("dropNullPlaceholders");
	valid_keys->insert("useSpecialFloats");
	valid_keys->insert("precision");
}
bool StreamWriterBuilder::validate(Json::Value* invalid) const
{
	Json::Value my_invalid;
	if (!invalid) invalid = &my_invalid;  // so we do not need to test for NULL
	Json::Value& inv = *invalid;
	std::set<JSONCPP_STRING> valid_keys;
	getValidWriterKeys(&valid_keys);
	Value::Members keys = settings_.getMemberNames();
	size_t n = keys.size();
	for (size_t i = 0; i < n; ++i) {
		JSONCPP_STRING const& key = keys[i];
		if (valid_keys.find(key) == valid_keys.end()) {
			inv[key] = settings_[key];
		}
	}
	return 0u == inv.size();
}
Value& StreamWriterBuilder::operator[](JSONCPP_STRING key)
{
	return settings_[key];
}
// static
void StreamWriterBuilder::setDefaults(Json::Value* settings)
{
	//! [StreamWriterBuilderDefaults]
	(*settings)["commentStyle"] = "All";
	(*settings)["indentation"] = "\t";
	(*settings)["enableYAMLCompatibility"] = false;
	(*settings)["dropNullPlaceholders"] = false;
	(*settings)["useSpecialFloats"] = false;
	(*settings)["precision"] = 17;
	//! [StreamWriterBuilderDefaults]
}

JSONCPP_STRING writeString(StreamWriter::Factory const& builder, Value const& root) {
	JSONCPP_OSTRINGSTREAM sout;
	StreamWriterPtr const writer(builder.newStreamWriter());
	writer->write(root, &sout);
	return sout.str();
}

JSONCPP_OSTREAM& operator<<(JSONCPP_OSTREAM& sout, Value const& root) {
	StreamWriterBuilder builder;
	StreamWriterPtr const writer(builder.newStreamWriter());
	writer->write(root, &sout);
	return sout;
}

} // namespace Json


#pragma pack(pop)

#if defined(JSONCPP_DISABLE_DLL_INTERFACE_WARNING)
#pragma warning(pop)
#endif // if defined(JSONCPP_DISABLE_DLL_INTERFACE_WARNING)

#endif // JSON_WRITER_H_INCLUDED
