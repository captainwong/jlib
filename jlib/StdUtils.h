#pragma once
#include <algorithm>
#include <iterator>

namespace jlib {


/**
* @breif �ж�T���͵�����C�Ƿ����ʵ��T
* @param c T���͵�����
* @param t T���͵�ʵ��
* @return bool �Ƿ����
*/
template <class C, class T>
inline bool is_contain(const C& c, const T& t) {
	for (const auto& i : C) {
		if (i == t) { return true; }
	}
	return false;
}


/**
* @brief ������ͬ���͵�����ʵ��All��Sub��All����Sub����ȡAll�з�Sub������Ԫ�ؼ���
* @param All ȫ��
* @param Sub �Ӽ�
* @return All�з�Sub������Ԫ�ؼ���
*/
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

}
