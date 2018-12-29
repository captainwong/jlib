#pragma once
#include <algorithm>
#include <iterator>

namespace jlib {


template <class C, class T>
inline bool is_contain(const C& c, const T& t) {
	for (const auto& i : c) {
		if (i == t) { return true; }
	}
	return false;
}


template <class C>
inline typename C get_other(const C & All, const C& Sub) {
	typename C res, tmp;
	std::copy(Sub.begin(), Sub.end(), std::back_inserter(tmp));
	for (const auto& i : All) {
		bool in_Sub = false;
		for (auto iter = tmp.begin(); iter != tmp.end(); iter++) {
			if (*iter == i) {
				tmp.erase(iter);
				in_Sub = true;
				break;
			}
		}
		if (!in_Sub) {
			res.push_back(i);
		}
	}
	return res;
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
