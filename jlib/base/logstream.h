#pragma once

#include "config.h"
#include "noncopyable.h"
#include "stringpiece.h"
#include "cast.h"
#include <assert.h>
#include <string.h>
#include <algorithm>
#include <limits>
#include <type_traits>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

namespace jlib
{

namespace detail
{

static constexpr int SMALL_BUFFER = 4096;
static constexpr int LARGE_BUFFER = 4096 * 1000;

template <int SIZE>
class FixedBuffer : noncopyable
{
private:
	const char *end() const { return data_ + sizeof data_; }

	// Must be outline function for cookies.
	static void cookieStart() {}
	static void cookieEnd() {}

	void (*cookie_)();
	char data_[SIZE];
	char *cur_;

public:
    FixedBuffer() : cur_(data_) { setCookie(cookieStart); }
    ~FixedBuffer() { setCookie(cookieEnd); }

    void append(const char* buf, size_t len) {
        if (implicit_cast<size_t>(avail()) > len) {
            memcpy(cur_, buf, len); cur_ += len;
        }
    }

    const char *data() const { return data_; }
	int length() const { return static_cast<int>(cur_ - data_); }

	// write to data_ directly
	char *current() { return cur_; }
	int avail() const { return static_cast<int>(end() - cur_); }
	void add(size_t len) { cur_ += len; }

	void reset() { cur_ = data_; }
	void bzero() { memset(data_, 0, sizeof data_); }

	// for used by GDB
	const char *debugString() { *cur_ = '\0'; return data_; }
	void setCookie(void (*cookie)()) { cookie_ = cookie; }

	// for used by unit test
	std::string toString() const { return std::string(data_, length()); }
	StringPiece toStringPiece() const { return StringPiece(data_, length()); }
}; // class FixedBuffer


template class FixedBuffer<SMALL_BUFFER>;
template class FixedBuffer<LARGE_BUFFER>;


// Efficient Integer to String Conversions, by Matthew Wilson.
template <typename T>
inline size_t convert(char buf[], T value)
{
    static const char digits[] = "9876543210123456789";
    static const char* zero = digits + 9;
    static_assert(sizeof(digits) == 20, "wrong number of digits");

    T i = value;
    char *p = buf;

    do {
        int lsd = static_cast<int>(i % 10);
        i /= 10; *p++ = zero[lsd];
    } while (i != 0);

    if (value < 0) { *p++ = '-'; }
    *p = '\0';
    std::reverse(buf, p);

    return p - buf;
}

static size_t convertHex(char buf[], uintptr_t value)
{
    static const char digitsHex[] = "0123456789ABCDEF";
    static_assert(sizeof(digitsHex) == 17, "wrong number of digitsHex");
    
    uintptr_t i = value;
    char *p = buf;

    do {
        int lsd = static_cast<int>(i % 16);
        i /= 16; *p++ = digitsHex[lsd];
    } while (i != 0);

    *p = '\0';
    std::reverse(buf, p);

    return p - buf;
}

} // namespace detail


class LogStream : noncopyable
{
    typedef LogStream self;

public:
    typedef detail::FixedBuffer<detail::SMALL_BUFFER> Buffer;

    self& operator<<(bool v) {
        if (v) { buffer_.append("true", 4); }
        else { buffer_.append("false", 5); }
		return *this;
	}

	self& operator<<(short v) { *this << static_cast<int>(v); return *this; }
	self& operator<<(unsigned short v) { *this << static_cast<unsigned int>(v); return *this; }
	self& operator<<(int v) { formatInteger(v); return *this; };
	self& operator<<(unsigned int v) { formatInteger(v); return *this; }
	self& operator<<(long v) { formatInteger(v); return *this; };
	self& operator<<(unsigned long v) { formatInteger(v); return *this; };
	self& operator<<(long long v) { formatInteger(v); return *this; };
	self& operator<<(unsigned long long v) { formatInteger(v); return *this; };

	self& operator<<(const void * p) {
        uintptr_t v = reinterpret_cast<uintptr_t>(p);
        if (buffer_.avail() >= MAX_NUMERIC_SIZE) {
            char* buf = buffer_.current();
            buf[0] = '0'; buf[1] = 'x';
            size_t len = detail::convertHex(buf + 2, v);
            buffer_.add(len + 2);
        }
        return *this;
    }

	self& operator<<(float v) { *this << static_cast<double>(v); return *this; }

	self& operator<<(double v) {
        if (buffer_.avail() >= MAX_NUMERIC_SIZE) {
            int len = snprintf(buffer_.current(), MAX_NUMERIC_SIZE, "%.12g", v);
            buffer_.add(len);
        }
        return *this;
    }

	// self& operator<<(long double);

	self& operator<<(char v) { buffer_.append(&v, 1); return *this; }

	// self& operator<<(signed char);
	// self& operator<<(unsigned char);

	self& operator<<(const char *str) {
		if (str) { buffer_.append(str, strlen(str));
		} else { buffer_.append("(null)", 6); }
		return *this;
	}

	self& operator<<(const unsigned char *str) { return operator<<(reinterpret_cast<const char *>(str)); }
	self& operator<<(const std::string& v) { buffer_.append(v.c_str(), v.size()); return *this; }
	self& operator<<(const StringPiece& v) { buffer_.append(v.data(), v.size()); return *this; }
	self& operator<<(const Buffer& v) { *this << v.toStringPiece(); return *this; }

	void append(const char *data, int len) { buffer_.append(data, len); }
	const Buffer& buffer() const { return buffer_; }
	void resetBuffer() { buffer_.reset(); }

private:
	void staticCheck() {
        static_assert(MAX_NUMERIC_SIZE - 10 > std::numeric_limits<double>::digits10,
            "MAX_NUMERIC_SIZE is large enough");
        static_assert(MAX_NUMERIC_SIZE - 10 > std::numeric_limits<long double>::digits10,
            "MAX_NUMERIC_SIZE is large enough");
        static_assert(MAX_NUMERIC_SIZE - 10 > std::numeric_limits<long>::digits10,
            "MAX_NUMERIC_SIZE is large enough");
        static_assert(MAX_NUMERIC_SIZE - 10 > std::numeric_limits<long long>::digits10,
            "MAX_NUMERIC_SIZE is large enough");
    }

	template <typename T>
	void formatInteger(T v) {
        if (buffer_.avail() > MAX_NUMERIC_SIZE) {
            size_t len = detail::convert(buffer_.current(), v);
            buffer_.add(len);
        }
    }

	Buffer buffer_;

	static constexpr int MAX_NUMERIC_SIZE = 32;
}; // class LogStream


class Format
{
public:
    template <typename T>
    Format(const char* fmt, T val) {
        static_assert(std::is_arithmetic<T>::value == true, "Must be arithmetic type");
        length_ = snprintf(buf_, sizeof(buf_), fmt, val);
        assert(static_cast<size_t>(length_) < sizeof(buf_));
    }

    const char* data() const { return buf_; }
    int length() const { return length_; }

private:
    char buf_[32];
    int length_;
};

// Explicit instantiations

template Format::Format(const char* fmt, char);
template Format::Format(const char* fmt, short);
template Format::Format(const char* fmt, unsigned short);
template Format::Format(const char* fmt, int);
template Format::Format(const char* fmt, unsigned int);
template Format::Format(const char* fmt, long);
template Format::Format(const char* fmt, unsigned long);
template Format::Format(const char* fmt, long long);
template Format::Format(const char* fmt, unsigned long long);
template Format::Format(const char* fmt, float);
template Format::Format(const char* fmt, double);


// Format quantity n in SI units (k, M, G, T, P, E).
// The returned string is atmost 5 characters long.
// Requires n >= 0
/*
 Format a number with 5 characters, including SI units.
 [0,     999]
 [1.00k, 999k]
 [1.00M, 999M]
 [1.00G, 999G]
 [1.00T, 999T]
 [1.00P, 999P]
 [1.00E, inf)
*/
static std::string formatSI(int64_t s)
{
    double n = static_cast<double>(s);
    char buf[64];
    if (s < 1000)
        snprintf(buf, sizeof(buf), "%" PRId64, s);
    else if (s < 9995)
        snprintf(buf, sizeof(buf), "%.2fk", n / 1e3);
    else if (s < 99950)
        snprintf(buf, sizeof(buf), "%.1fk", n / 1e3);
    else if (s < 999500)
        snprintf(buf, sizeof(buf), "%.0fk", n / 1e3);
    else if (s < 9995000)
        snprintf(buf, sizeof(buf), "%.2fM", n / 1e6);
    else if (s < 99950000)
        snprintf(buf, sizeof(buf), "%.1fM", n / 1e6);
    else if (s < 999500000)
        snprintf(buf, sizeof(buf), "%.0fM", n / 1e6);
    else if (s < 9995000000)
        snprintf(buf, sizeof(buf), "%.2fG", n / 1e9);
    else if (s < 99950000000)
        snprintf(buf, sizeof(buf), "%.1fG", n / 1e9);
    else if (s < 999500000000)
        snprintf(buf, sizeof(buf), "%.0fG", n / 1e9);
    else if (s < 9995000000000)
        snprintf(buf, sizeof(buf), "%.2fT", n / 1e12);
    else if (s < 99950000000000)
        snprintf(buf, sizeof(buf), "%.1fT", n / 1e12);
    else if (s < 999500000000000)
        snprintf(buf, sizeof(buf), "%.0fT", n / 1e12);
    else if (s < 9995000000000000)
        snprintf(buf, sizeof(buf), "%.2fP", n / 1e15);
    else if (s < 99950000000000000)
        snprintf(buf, sizeof(buf), "%.1fP", n / 1e15);
    else if (s < 999500000000000000)
        snprintf(buf, sizeof(buf), "%.0fP", n / 1e15);
    else
        snprintf(buf, sizeof(buf), "%.2fE", n / 1e18);
    return buf;
}

// Format quantity n in IEC (binary) units (Ki, Mi, Gi, Ti, Pi, Ei).
// The returned string is atmost 6 characters long.
// Requires n >= 0
/*
 [0, 1023]
 [1.00Ki, 9.99Ki]
 [10.0Ki, 99.9Ki]
 [ 100Ki, 1023Ki]
 [1.00Mi, 9.99Mi]
*/
static std::string formatIEC(int64_t s)
{
	static constexpr double Ki = 1024.0;
	static constexpr double Mi = Ki * 1024.0;
	static constexpr double Gi = Mi * 1024.0;
	static constexpr double Ti = Gi * 1024.0;
	static constexpr double Pi = Ti * 1024.0;
	static constexpr double Ei = Pi * 1024.0;

    double n = static_cast<double>(s);
    char buf[64];

    if (n < Ki)
        snprintf(buf, sizeof buf, "%" PRId64, s);
    else if (n < Ki * 9.995)
        snprintf(buf, sizeof buf, "%.2fKi", n / Ki);
    else if (n < Ki * 99.95)
        snprintf(buf, sizeof buf, "%.1fKi", n / Ki);
    else if (n < Ki * 1023.5)
        snprintf(buf, sizeof buf, "%.0fKi", n / Ki);

    else if (n < Mi * 9.995)
        snprintf(buf, sizeof buf, "%.2fMi", n / Mi);
    else if (n < Mi * 99.95)
        snprintf(buf, sizeof buf, "%.1fMi", n / Mi);
    else if (n < Mi * 1023.5)
        snprintf(buf, sizeof buf, "%.0fMi", n / Mi);

    else if (n < Gi * 9.995)
        snprintf(buf, sizeof buf, "%.2fGi", n / Gi);
    else if (n < Gi * 99.95)
        snprintf(buf, sizeof buf, "%.1fGi", n / Gi);
    else if (n < Gi * 1023.5)
        snprintf(buf, sizeof buf, "%.0fGi", n / Gi);

    else if (n < Ti * 9.995)
        snprintf(buf, sizeof buf, "%.2fTi", n / Ti);
    else if (n < Ti * 99.95)
        snprintf(buf, sizeof buf, "%.1fTi", n / Ti);
    else if (n < Ti * 1023.5)
        snprintf(buf, sizeof buf, "%.0fTi", n / Ti);

    else if (n < Pi * 9.995)
        snprintf(buf, sizeof buf, "%.2fPi", n / Pi);
    else if (n < Pi * 99.95)
        snprintf(buf, sizeof buf, "%.1fPi", n / Pi);
    else if (n < Pi * 1023.5)
        snprintf(buf, sizeof buf, "%.0fPi", n / Pi);

    else if (n < Ei * 9.995)
        snprintf(buf, sizeof buf, "%.2fEi", n / Ei);
    else
        snprintf(buf, sizeof buf, "%.1fEi", n / Ei);
    return buf;
}

} // namespace jlib
