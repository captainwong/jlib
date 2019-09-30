#pragma once


// mainly inspired by Giovanni Dicanio
// with some simplifications and changes
// see his original license below:


////////////////////////////////////////////////////////////////////////////////
//
//      *** Modern C++ Wrappers Around Windows Registry C API ***
// 
//               Copyright (C) by Giovanni Dicanio 
//  
// First version: 2017, January 22nd
// Last update: 2019, March 26th
// 
// E-mail: <giovanni.dicanio AT gmail.com>
// 
// Registry key handles are safely and conveniently wrapped 
// in the RegKey resource manager C++ class.
// 
// Errors are signaled throwing exceptions of class RegException.
// 
// Unicode UTF-16 strings are represented using the std::wstring class; 
// ATL's CString is not used, to avoid dependencies from ATL or MFC.
// 
// This is a header-only self-contained reusable module.
//
// Compiler: Visual Studio 2015
// Code compiles clean at /W4 on both 32-bit and 64-bit builds.
// 
// ===========================================================================
//
// The MIT License(MIT)
//
// Copyright(c) 2017 Giovanni Dicanio
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
////////////////////////////////////////////////////////////////////////////////



#include <Windows.h>
#include <strsafe.h>
#include <string>
#include <memory>
#include <vector>
#include <stdexcept>
#include <utility>
#include <assert.h>
#include "../base/noncopyable.h"


namespace jlib
{
namespace win32
{
namespace reg
{

class RegException : public std::runtime_error
{
public:
	explicit RegException(const std::string& msg, DWORD errorCode, const std::string& desc)
		: std::runtime_error(msg)
		, lastError_(errorCode)
		, description_(desc)
	{}

	DWORD errorCode() const { return lastError_; }
	std::string description() const { return description_; }

private:
	DWORD lastError_ = ERROR_SUCCESS;
	std::string description_ = {};
};


inline void throwRegException(const std::string& msg, DWORD errorCode)
{
	LPVOID lpMsgBuf;

	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)& lpMsgBuf,
		0, NULL);

	std::string desc(reinterpret_cast<const char*>(lpMsgBuf));
	LocalFree(lpMsgBuf);

	throw RegException(msg, errorCode, desc);
}


#define THROW_REG_EXCEPTION(msg, err)											\
do {																			\
	std::string errMsg = msg; errMsg += " SourceFile: "; errMsg += __FILE__;	\
	errMsg += " Line: "; errMsg += std::to_string(__LINE__);					\
	throwRegException(errMsg, err);												\
} while(0);


class RegKey : public noncopyable
{
public:
	RegKey() = default;
	explicit RegKey(HKEY hKey) noexcept : hKey_(hKey) {}
	~RegKey() noexcept { close(); }

	RegKey& operator=(RegKey&& rhs) noexcept {
		// prevent self-move-assign
		if ((this != &rhs) && (hKey_ != rhs.hKey_)) {
			close(); hKey_ = rhs.hKey_; rhs.hKey_ = nullptr;
		}
		return *this;
	}

	// properties

	HKEY hKey() const noexcept { return hKey_; }
	bool valid() const noexcept { return hKey_ != nullptr; }
	explicit operator bool() const noexcept { return valid(); }
	bool isPredefined() const noexcept;


	// operations

	void close() noexcept;
	void open(HKEY hKeyParent, const std::wstring& subKey, REGSAM desiredAccess = KEY_READ | KEY_WRITE);
	void create(HKEY hKeyParent, const std::wstring& subKey, REGSAM desiredAccess = KEY_READ | KEY_WRITE);
	void create(HKEY hKeyParent, const std::wstring& subKey, REGSAM desiredAccess, DWORD options, SECURITY_ATTRIBUTES* sa, DWORD* disposition);


	// value getters

	DWORD getDwordValue(const std::wstring& valueName);
	ULONGLONG getQwordValue(const std::wstring& valueName);
	std::wstring getStringValue(const std::wstring& valueName);

	enum class ExpandStringOption {
		DonotExpand,
		Expand,
	};

	std::wstring getExpandStringValue(const std::wstring& valueName, ExpandStringOption option = ExpandStringOption::DonotExpand);
	std::vector<std::wstring> getMultiStringValue(const std::wstring& valueName);
	std::vector<BYTE> getBinaryValue(const std::wstring& valueName);


	// value setters

	void setDwordValue(const std::wstring& valueName, DWORD data);
	void setQwordValue(const std::wstring& valueName, const ULONGLONG& data);
	void setStringValue(const std::wstring& valueName, const std::wstring& data);
	void setExpandStringValue(const std::wstring& valueName, const std::wstring& data);
	void setMultiStringValue(const std::wstring& valueName, const std::vector<std::wstring>& data);
	void setBinaryValue(const std::wstring& valueName, const std::vector<BYTE>& data);
	void setBinaryValue(const std::wstring& valueName, const void* data, DWORD dataSize);


	// query

	DWORD queryValueType(const std::wstring& valueName);
	std::vector<std::wstring> querySubKeys();
	//                    value-name    value-type
	std::vector<std::pair<std::wstring, DWORD>> queryValues();


	// delete

	void deleteValue(const std::wstring& valueName);
	void deleteKey(const std::wstring& subKey, REGSAM desiredAccess);
	void deleteTree(const std::wstring& subKey);


	static std::wstring regTypeToString(DWORD regType);

private:
	HKEY hKey_ = nullptr;
};


// implementation

inline bool RegKey::isPredefined() const noexcept
{
	// Predefined keys
	// https://msdn.microsoft.com/en-us/library/windows/desktop/ms724836(v=vs.85).aspx

	return ((hKey_ == HKEY_CURRENT_USER)
			|| (hKey_ == HKEY_LOCAL_MACHINE)
			|| (hKey_ == HKEY_CLASSES_ROOT)
			|| (hKey_ == HKEY_CURRENT_CONFIG)
			|| (hKey_ == HKEY_CURRENT_USER_LOCAL_SETTINGS)
			|| (hKey_ == HKEY_PERFORMANCE_DATA)
			|| (hKey_ == HKEY_PERFORMANCE_NLSTEXT)
			|| (hKey_ == HKEY_PERFORMANCE_TEXT)
			|| (hKey_ == HKEY_USERS));
}

inline void RegKey::close() noexcept
{
	if (valid()) {
		if (!isPredefined()) {
			::RegCloseKey(hKey_);
		}

		hKey_ = nullptr;
	}
}

inline void RegKey::open(HKEY hKeyParent, const std::wstring& subKey, REGSAM desiredAccess)
{
	HKEY hKey{ nullptr };
	DWORD retCode = ::RegOpenKeyExW(
		hKeyParent,
		subKey.c_str(),
		REG_NONE,           // default options
		desiredAccess,
		&hKey
	);

	if (retCode != ERROR_SUCCESS) {
		THROW_REG_EXCEPTION("RegOpenKeyEx failed.", retCode);
	}

	close();
	hKey_ = hKey;
}

inline void RegKey::create(const HKEY hKeyParent, const std::wstring& subKey, const REGSAM desiredAccess)
{
	constexpr DWORD kDefaultOptions = REG_OPTION_NON_VOLATILE;

	create(hKeyParent, subKey, desiredAccess, kDefaultOptions,
		   nullptr, // no security attributes,
		   nullptr  // no disposition 
	);
}

inline void RegKey::create(HKEY hKeyParent, const std::wstring& subKey, REGSAM desiredAccess, DWORD options,
							 SECURITY_ATTRIBUTES* securityAttributes, DWORD* disposition)
{
	HKEY hKey{ nullptr };
	DWORD retCode = ::RegCreateKeyExW(
		hKeyParent,
		subKey.c_str(),
		0,          // reserved
		REG_NONE,   // user-defined class type parameter not supported
		options,
		desiredAccess,
		securityAttributes,
		&hKey,
		disposition
	);

	if (retCode != ERROR_SUCCESS) {
		THROW_REG_EXCEPTION("RegCreateKeyEx failed.", retCode);
	}

	close();
	hKey_ = hKey;
}

inline DWORD RegKey::getDwordValue(const std::wstring& valueName)
{
	assert(valid());

	DWORD data{};                   // to be read from the registry
	DWORD dataSize = sizeof(data);  // size of data, in bytes

	const DWORD flags = RRF_RT_REG_DWORD;
	DWORD retCode = ::RegGetValueW(
		hKey_,
		nullptr, // no subkey
		valueName.c_str(),
		flags,
		nullptr, // type not required
		&data,
		&dataSize
	);

	if (retCode != ERROR_SUCCESS) {
		THROW_REG_EXCEPTION("Cannot get DWORD value: RegGetValue failed.", retCode);
	}

	return data;
}

inline ULONGLONG RegKey::getQwordValue(const std::wstring& valueName)
{
	assert(valid());

	ULONGLONG data{};               // to be read from the registry
	DWORD dataSize = sizeof(data);  // size of data, in bytes

	const DWORD flags = RRF_RT_REG_QWORD;
	DWORD retCode = ::RegGetValueW(
		hKey_,
		nullptr, // no subkey
		valueName.c_str(),
		flags,
		nullptr, // type not required
		&data,
		&dataSize
	); 
	
	if (retCode != ERROR_SUCCESS) {
		THROW_REG_EXCEPTION("Cannot get QWORD value: RegGetValue failed.", retCode);
	}

	return data;
}

inline std::wstring RegKey::getStringValue(const std::wstring& valueName)
{
	assert(valid());

	// Get the size of the result string
	DWORD dataSize = 0; // size of data, in bytes
	const DWORD flags = RRF_RT_REG_SZ;
	DWORD retCode = ::RegGetValueW(
		hKey_,
		nullptr, // no subkey
		valueName.c_str(),
		flags,
		nullptr, // type not required
		nullptr, // output buffer not needed now
		&dataSize
	);

	if (retCode != ERROR_SUCCESS) {
		THROW_REG_EXCEPTION("Cannot get size of string value: RegGetValue failed.", retCode);
	}

	// Allocate a string of proper size.
	// Note that dataSize is in bytes and includes the terminating NUL;
	// we have to convert the size from bytes to wchar_ts for wstring::resize.
	std::wstring result;
	result.resize(dataSize / sizeof(wchar_t));

	// Call RegGetValue for the second time to read the string's content
	retCode = ::RegGetValueW(
		hKey_,
		nullptr,    // no subkey
		valueName.c_str(),
		flags,
		nullptr,    // type not required
		&result[0], // output buffer
		&dataSize
	);

	if (retCode != ERROR_SUCCESS) {
		THROW_REG_EXCEPTION("Cannot get string value: RegGetValue failed.", retCode);
	}

	// Remove the NUL terminator scribbled by RegGetValue from the wstring
	result.resize((dataSize / sizeof(wchar_t)) - 1);

	return result;
}

inline std::wstring RegKey::getExpandStringValue(const std::wstring& valueName, ExpandStringOption expandOption)
{
	assert(valid());

	DWORD flags = RRF_RT_REG_EXPAND_SZ;

	// Adjust the flag for RegGetValue considering the expand string option specified by the caller   
	if (expandOption == ExpandStringOption::DonotExpand) {
		flags |= RRF_NOEXPAND;
	}

	// Get the size of the result string
	DWORD dataSize = 0; // size of data, in bytes
	DWORD retCode = ::RegGetValueW(
		hKey_,
		nullptr,    // no subkey
		valueName.c_str(),
		flags,
		nullptr,    // type not required
		nullptr,    // output buffer not needed now
		&dataSize
	);

	if (retCode != ERROR_SUCCESS) {
		THROW_REG_EXCEPTION("Cannot get size of expand string value: RegGetValue failed.", retCode);
	}

	// Allocate a string of proper size.
	// Note that dataSize is in bytes and includes the terminating NUL.
	// We must convert from bytes to wchar_ts for wstring::resize.
	std::wstring result;
	result.resize(dataSize / sizeof(wchar_t));

	// Call RegGetValue for the second time to read the string's content
	retCode = ::RegGetValueW(
		hKey_,
		nullptr,    // no subkey
		valueName.c_str(),
		flags,
		nullptr,    // type not required
		&result[0], // output buffer
		&dataSize
	);

	if (retCode != ERROR_SUCCESS) {
		THROW_REG_EXCEPTION("Cannot get expand string value: RegGetValue failed.", retCode);
	}

	// Remove the NUL terminator scribbled by RegGetValue from the wstring
	result.resize((dataSize / sizeof(wchar_t)) - 1);

	return result;
}

inline std::vector<std::wstring> RegKey::getMultiStringValue(const std::wstring& valueName)
{
	assert(valid());

	// Request the size of the multi-string, in bytes
	DWORD dataSize = 0;
	const DWORD flags = RRF_RT_REG_MULTI_SZ;
	DWORD retCode = ::RegGetValueW(
		hKey_,
		nullptr,    // no subkey
		valueName.c_str(),
		flags,
		nullptr,    // type not required
		nullptr,    // output buffer not needed now
		&dataSize
	);

	if (retCode != ERROR_SUCCESS) {
		THROW_REG_EXCEPTION("Cannot get size of multi-string value: RegGetValue failed.", retCode);
	}

	// Allocate room for the result multi-string.
	// Note that dataSize is in bytes, but our vector<wchar_t>::resize method requires size 
	// to be expressed in wchar_ts.
	std::vector<wchar_t> data;
	data.resize(dataSize / sizeof(wchar_t));

	// Read the multi-string from the registry into the vector object
	retCode = ::RegGetValueW(
		hKey_,
		nullptr,    // no subkey
		valueName.c_str(),
		flags,
		nullptr,    // no type required
		&data[0],   // output buffer
		&dataSize
	);

	if (retCode != ERROR_SUCCESS) {
		THROW_REG_EXCEPTION("Cannot get multi-string value: RegGetValue failed.", retCode);
	}

	// Resize vector to the actual size returned by GetRegValue.
	// Note that the vector is a vector of wchar_ts, instead the size returned by GetRegValue
	// is in bytes, so we have to scale from bytes to wchar_t count.
	data.resize(dataSize / sizeof(wchar_t));

	// Parse the double-NUL-terminated string into a vector<wstring>, 
	// which will be returned to the caller
	std::vector<std::wstring> result;
	const wchar_t* currStringPtr = &data[0];
	while (*currStringPtr != L'\0') {
		// Current string is NUL-terminated, so get its length calling wcslen
		const size_t currStringLength = wcslen(currStringPtr);

		// Add current string to the result vector
		result.push_back(std::wstring{ currStringPtr, currStringLength });

		// Move to the next string
		currStringPtr += currStringLength + 1;
	}

	return result;
}

inline std::vector<BYTE> RegKey::getBinaryValue(const std::wstring& valueName)
{
	assert(valid());

	// Get the size of the binary data
	DWORD dataSize = 0; // size of data, in bytes
	const DWORD flags = RRF_RT_REG_BINARY;
	DWORD retCode = ::RegGetValueW(
		hKey_,
		nullptr,    // no subkey
		valueName.c_str(),
		flags,
		nullptr,    // type not required
		nullptr,    // output buffer not needed now
		&dataSize
	);

	if (retCode != ERROR_SUCCESS) {
		THROW_REG_EXCEPTION("Cannot get size of binary data: RegGetValue failed.", retCode);
	}

	// Allocate a buffer of proper size to store the binary data
	std::vector<BYTE> data(dataSize);

	// Call RegGetValue for the second time to read the data content
	retCode = ::RegGetValueW(
		hKey_,
		nullptr,    // no subkey
		valueName.c_str(),
		flags,
		nullptr,    // type not required
		&data[0],   // output buffer
		&dataSize
	);

	if (retCode != ERROR_SUCCESS) {
		THROW_REG_EXCEPTION("Cannot get binary data: RegGetValue failed.", retCode);
	}

	return data;
}

inline void RegKey::setDwordValue(const std::wstring& valueName, DWORD data)
{
	assert(valid());

	DWORD retCode = ::RegSetValueExW(
		hKey_,
		valueName.c_str(),
		0, // reserved
		REG_DWORD,
		reinterpret_cast<const BYTE*>(&data),
		sizeof(data)
	);

	if (retCode != ERROR_SUCCESS) {
		THROW_REG_EXCEPTION("Cannot write DWORD value: RegSetValueEx failed.", retCode);
	}
}

inline void RegKey::setQwordValue(const std::wstring& valueName, const ULONGLONG& data)
{
	assert(valid());

	DWORD retCode = ::RegSetValueExW(
		hKey_,
		valueName.c_str(),
		0, // reserved
		REG_QWORD,
		reinterpret_cast<const BYTE*>(&data),
		sizeof(data)
	);

	if (retCode != ERROR_SUCCESS) {
		THROW_REG_EXCEPTION("Cannot write QWORD value: RegSetValueEx failed.", retCode);
	}
}

inline void RegKey::setStringValue(const std::wstring& valueName, const std::wstring& data)
{
	assert(valid());

	// String size including the terminating NUL, in bytes
	const DWORD dataSize = static_cast<DWORD>((data.length() + 1) * sizeof(wchar_t));

	DWORD retCode = ::RegSetValueExW(
		hKey_,
		valueName.c_str(),
		0, // reserved
		REG_SZ,
		reinterpret_cast<const BYTE*>(data.c_str()),
		dataSize
	);

	if (retCode != ERROR_SUCCESS) {
		THROW_REG_EXCEPTION("Cannot write string value: RegSetValueEx failed.", retCode);
	}
}

inline void RegKey::setExpandStringValue(const std::wstring& valueName, const std::wstring& data)
{
	assert(valid());

	// String size including the terminating NUL, in bytes
	const DWORD dataSize = static_cast<DWORD>((data.length() + 1) * sizeof(wchar_t));

	DWORD retCode = ::RegSetValueExW(
		hKey_,
		valueName.c_str(),
		0, // reserved
		REG_EXPAND_SZ,
		reinterpret_cast<const BYTE*>(data.c_str()),
		dataSize
	);

	if (retCode != ERROR_SUCCESS) {
		THROW_REG_EXCEPTION("Cannot write expand string value: RegSetValueEx failed.", retCode);
	}
}

inline void RegKey::setMultiStringValue(const std::wstring& valueName, const std::vector<std::wstring>& data)
{
	assert(valid());

	static auto buildMultiString = [](const std::vector<std::wstring>& data) -> std::vector<wchar_t> {
		// Special case of the empty multi-string
		if (data.empty()) {
			// Build a vector containing just two NULs
			return std::vector<wchar_t>(2, L'\0');
		}

		// Get the total length in wchar_ts of the multi-string
		size_t totalLen = 0;
		for (const auto& s : data) {
			// Add one to current string's length for the terminating NUL
			totalLen += (s.length() + 1);
		}

		// Add one for the last NUL terminator (making the whole structure double-NUL terminated)
		totalLen++;

		// Allocate a buffer to store the multi-string
		std::vector<wchar_t> multiString;
		multiString.reserve(totalLen);

		// Copy the single strings into the multi-string
		for (const auto& s : data) {
			multiString.insert(multiString.end(), s.begin(), s.end());

			// Don't forget to NUL-terminate the current string
			multiString.push_back(L'\0');
		}

		// Add the last NUL-terminator
		multiString.push_back(L'\0');

		return multiString;
	};

	// First, we have to build a double-NUL-terminated multi-string from the input data
	const std::vector<wchar_t> multiString = buildMultiString(data);

	// Total size, in bytes, of the whole multi-string structure
	const DWORD dataSize = static_cast<DWORD>(multiString.size() * sizeof(wchar_t));

	DWORD retCode = ::RegSetValueExW(
		hKey_,
		valueName.c_str(),
		0, // reserved
		REG_MULTI_SZ,
		reinterpret_cast<const BYTE*>(&multiString[0]),
		dataSize
	);

	if (retCode != ERROR_SUCCESS) {
		THROW_REG_EXCEPTION("Cannot write multi-string value: RegSetValueEx failed.", retCode);
	}
}

inline void RegKey::setBinaryValue(const std::wstring& valueName, const std::vector<BYTE>& data)
{
	assert(valid());

	// Total data size, in bytes
	const DWORD dataSize = static_cast<DWORD>(data.size());

	DWORD retCode = ::RegSetValueExW(
		hKey_,
		valueName.c_str(),
		0, // reserved
		REG_BINARY,
		&data[0],
		dataSize
	);

	if (retCode != ERROR_SUCCESS) {
		THROW_REG_EXCEPTION("Cannot write binary data value: RegSetValueEx failed.", retCode);
	}
}

inline void RegKey::setBinaryValue(const std::wstring& valueName, const void* data, DWORD dataSize)
{
	assert(valid());

	DWORD retCode = ::RegSetValueExW(
		hKey_,
		valueName.c_str(),
		0, // reserved
		REG_BINARY,
		static_cast<const BYTE*>(data),
		dataSize
	);

	if (retCode != ERROR_SUCCESS) {
		THROW_REG_EXCEPTION("Cannot write binary data value: RegSetValueEx failed.", retCode);
	}
}

inline DWORD RegKey::queryValueType(const std::wstring& valueName)
{
	assert(valid());

	DWORD typeId{};     // will be returned by RegQueryValueEx

	DWORD retCode = ::RegQueryValueExW(
		hKey_,
		valueName.c_str(),
		nullptr,    // reserved
		&typeId,
		nullptr,    // not interested
		nullptr     // not interested
	);

	if (retCode != ERROR_SUCCESS) {
		THROW_REG_EXCEPTION("Cannot get the value type: RegQueryValueEx failed.", retCode);
	}

	return typeId;
}

inline std::vector<std::wstring> RegKey::querySubKeys()
{
	assert(valid());

	// Get some useful enumeration info, like the total number of subkeys
	// and the maximum length of the subkey names
	DWORD subKeyCount{};
	DWORD maxSubKeyNameLen{};
	DWORD retCode = ::RegQueryInfoKeyW(
		hKey_,
		nullptr,    // no user-defined class
		nullptr,    // no user-defined class size
		nullptr,    // reserved
		&subKeyCount,
		&maxSubKeyNameLen,
		nullptr,    // no subkey class length
		nullptr,    // no value count
		nullptr,    // no value name max length
		nullptr,    // no max value length
		nullptr,    // no security descriptor
		nullptr     // no last write time
	);

	if (retCode != ERROR_SUCCESS) {
		THROW_REG_EXCEPTION("RegQueryInfoKey failed while preparing for subkey enumeration.", retCode);
	}

	// NOTE: According to the MSDN documentation, the size returned for subkey name max length
	// does *not* include the terminating NUL, so let's add +1 to take it into account
	// when I allocate the buffer for reading subkey names.
	maxSubKeyNameLen++;

	// Preallocate a buffer for the subkey names
	auto nameBuffer = std::make_unique<wchar_t[]>(maxSubKeyNameLen);

	// The result subkey names will be stored here
	std::vector<std::wstring> subkeyNames;

	// Enumerate all the subkeys
	for (DWORD index = 0; index < subKeyCount; index++) {
		// Get the name of the current subkey
		DWORD subKeyNameLen = maxSubKeyNameLen;
		retCode = ::RegEnumKeyExW(
			hKey_,
			index,
			nameBuffer.get(),
			&subKeyNameLen,
			nullptr, // reserved
			nullptr, // no class
			nullptr, // no class
			nullptr  // no last write time
		);

		if (retCode != ERROR_SUCCESS) {
			THROW_REG_EXCEPTION("Cannot enumerate subkeys: RegEnumKeyEx failed.", retCode);
		}

		// On success, the ::RegEnumKeyEx API writes the length of the
		// subkey name in the subKeyNameLen output parameter 
		// (not including the terminating NUL).
		// So I can build a wstring based on that length.
		subkeyNames.push_back(std::wstring{ nameBuffer.get(), subKeyNameLen });
	}

	return subkeyNames;
}

std::vector<std::pair<std::wstring, DWORD>> RegKey::queryValues()
{
	assert(valid());

	// Get useful enumeration info, like the total number of values
	// and the maximum length of the value names
	DWORD valueCount{};
	DWORD maxValueNameLen{};
	DWORD retCode = ::RegQueryInfoKeyW(
		hKey_,
		nullptr,    // no user-defined class
		nullptr,    // no user-defined class size
		nullptr,    // reserved
		nullptr,    // no subkey count
		nullptr,    // no subkey max length
		nullptr,    // no subkey class length
		&valueCount,
		&maxValueNameLen,
		nullptr,    // no max value length
		nullptr,    // no security descriptor
		nullptr     // no last write time
	);

	if (retCode != ERROR_SUCCESS) {
		THROW_REG_EXCEPTION("RegQueryInfoKey failed while preparing for value enumeration.", retCode);
	}

	// NOTE: According to the MSDN documentation, the size returned for value name max length
	// does *not* include the terminating NUL, so let's add +1 to take it into account
	// when I allocate the buffer for reading value names.
	maxValueNameLen++;

	// Preallocate a buffer for the value names
	auto nameBuffer = std::make_unique<wchar_t[]>(maxValueNameLen);

	// The value names and types will be stored here
	std::vector<std::pair<std::wstring, DWORD>> valueInfo;

	// Enumerate all the values
	for (DWORD index = 0; index < valueCount; index++) {
		// Get the name and the type of the current value
		DWORD valueNameLen = maxValueNameLen;
		DWORD valueType{};
		retCode = ::RegEnumValueW(
			hKey_,
			index,
			nameBuffer.get(),
			&valueNameLen,
			nullptr,    // reserved
			&valueType,
			nullptr,    // no data
			nullptr     // no data size
		);

		if (retCode != ERROR_SUCCESS) {
			THROW_REG_EXCEPTION("Cannot enumerate values: RegEnumValue failed.", retCode);
		}

		// On success, the RegEnumValue API writes the length of the
		// value name in the valueNameLen output parameter 
		// (not including the terminating NUL).
		// So we can build a wstring based on that.
		valueInfo.push_back(
			std::make_pair(std::wstring{ nameBuffer.get(), valueNameLen }, valueType)
		);
	}

	return valueInfo;
}

inline void RegKey::deleteValue(const std::wstring& valueName)
{
	assert(valid());

	DWORD retCode = ::RegDeleteValueW(hKey_, valueName.c_str());
	if (retCode != ERROR_SUCCESS) {
		THROW_REG_EXCEPTION("RegDeleteValue failed.", retCode);
	}
}

inline void RegKey::deleteKey(const std::wstring& subKey, const REGSAM desiredAccess)
{
	assert(valid());

	DWORD retCode = ::RegDeleteKeyExW(hKey_, subKey.c_str(), desiredAccess, 0);
	if (retCode != ERROR_SUCCESS) {
		THROW_REG_EXCEPTION("RegDeleteKeyEx failed.", retCode);
	}
}

inline void RegKey::deleteTree(const std::wstring& subKey)
{
	assert(valid());

	DWORD retCode = ::RegDeleteTreeW(hKey_, subKey.c_str());
	if (retCode != ERROR_SUCCESS) {
		THROW_REG_EXCEPTION("RegDeleteTree failed.", retCode);
	}
}

inline std::wstring RegKey::regTypeToString(const DWORD regType)
{
	switch (regType)
	{
	case REG_SZ:        return L"REG_SZ";
	case REG_EXPAND_SZ: return L"REG_EXPAND_SZ";
	case REG_MULTI_SZ:  return L"REG_MULTI_SZ";
	case REG_DWORD:     return L"REG_DWORD";
	case REG_QWORD:     return L"REG_QWORD";
	case REG_BINARY:    return L"REG_BINARY";

	default:            return L"Unknown/unsupported registry type";
	}
}

} // namespace reg
} // namespace win32
} // namespace jlib
