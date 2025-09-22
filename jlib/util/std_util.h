#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <type_traits> // std::void_t

namespace jlib {

/**
 * @brief check if Container contains ElementType t
 * @note C must be a container of type T
 */
template <class Container, class ElementType>
inline bool is_contain(const Container& c, const ElementType& t) {
	for (const auto& i : c) {
		if (i == t) { return true; }
	}
	return false;
}

// 利用SFINAE检测容器是否有.find()成员函数
template <class Container, class Key, class = void>
struct has_find_member : std::false_type {};

template <class Container, class Key>
struct has_find_member<Container, Key,
	std::void_t<decltype(std::declval<Container>().find(std::declval<Key>()))>>
	: std::true_type {};

// 主模板函数 require c++17
template <class Container, class Key>
bool has_key(const Container& cont, const Key& key) {
	// 如果容器有 .find() 成员函数，优先使用
	if constexpr (has_find_member<Container, Key>::value) {
		return cont.find(key) != cont.end();
	} else { // 否则，尝试使用 .count() 成员函数
		return cont.count(key) > 0;
	}
}

/**
 * @brief Sub 是 All 的子集，返回 All 内 Sub 的补集
 * @note All 和 Sub 必须为同样类型的容器（不支持initializer-list）
 */
template <class Container>
Container get_other(const Container& All, const Container& Sub) {
	Container res, tmp;
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
	for (const auto& i : v) { if (i != t) { ret.push_back(i); } }
	return ret;
}

/**
 * @brief t 是 v 的子集，返回 v 内 t 的补集
 */
template <class V>
std::vector<std::string> get_other(const V& v, const std::string& t) {
	std::vector<std::string> ret = {};
	for (const auto& i : v) { if (i != t) { ret.push_back(i); } }
	return ret;
}

}
