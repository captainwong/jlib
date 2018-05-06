#pragma once
#include <vector>
#include <string>

namespace jlib {
template <class V, class T>
bool is_contain(const V& v, const T& t) {
	for (auto i : v) { if (i == t) { return true; } }
	return false;
}

template <class V>
std::vector<std::wstring> get_other(const V& v, const std::wstring& t) {
	std::vector<std::wstring> ret = {};
	for (auto i : v) { if (i != t) { ret.push_back(t); } }
	return ret;
}

template <class V>
std::vector<std::string> get_other(const V& v, const std::string& t) {
	std::vector<std::string> ret = {};
	for (auto i : v) { if (i != t) { ret.push_back(t); } }
	return ret;
}

}
