#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <iterator>

namespace jlib {

/**
 * @brief check if container c contains t
 * @note C must be a container of type T
 */
template <class C, class T>
inline bool is_contain(const C& c, const T& t) {
	for (const auto& i : c) {
		if (i == t) { return true; }
	}
	return false;
}

/**
 * @brief Sub 是 All 的子集，返回 All 内 Sub 的补集
 * @note All 和 Sub 必须为同样类型的容器（不支持initializer-list）
 */
template <class C>
inline C get_other(const C & All, const C& Sub) {
	C res, tmp;
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

/**
 * @brief t 是 v 的子集，返回 v 内 t 的补集
 */
template <class V>
std::vector<std::wstring> get_other(const V& v, const std::wstring& t) {
	std::vector<std::wstring> ret = {};
	for (auto i : v) { if (i != t) { ret.push_back(t); } }
	return ret;
}

/**
 * @brief t 是 v 的子集，返回 v 内 t 的补集
 */
template <class V>
std::vector<std::string> get_other(const V& v, const std::string& t) {
	std::vector<std::string> ret = {};
	for (auto i : v) { if (i != t) { ret.push_back(t); } }
	return ret;
}

}
