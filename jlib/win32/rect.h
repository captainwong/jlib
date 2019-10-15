#pragma once

#include <Windows.h>
#include <vector>

namespace jlib
{

namespace win32
{


inline long width(::LPCRECT rc) {
	return rc->right - rc->left;
}

inline long height(::LPCRECT rc) {
	return rc->bottom - rc->top;
}

inline void deflateRect(::LPRECT rc, int l, int t, int r, int b) {
	rc->left += l;
	rc->top += t;
	rc->right -= r;
	rc->bottom -= b;
}

inline void inflateRect(::LPRECT rc, int l, int t, int r, int b) {
	rc->left -= l;
	rc->top -= t;
	rc->right += r;
	rc->bottom += b;
}


//! 将矩形平均分割成 n 份，间距 2*gap, n 必须是平方数 x^2, x={1,2,3...}
inline std::vector<::RECT> splitRect(::LPCRECT rc, int n, int gap = 50) {
	std::vector<::RECT> v;
	for (int i = 0; i < n; i++) {
		v.push_back(*rc);
	}

	double l = sqrt(n);
	int line = int(l);

	int col_step = (int)(Width(rc) / line);
	int row_step = (int)(Height(rc) / line);

	for (int i = 0; i < n; i++) {
		v[i].left = rc->left + (i % line) * col_step;
		v[i].right = v[i].left + col_step;
		v[i].top = rc->top + (i / line) * row_step;
		v[i].bottom = v[i].top + row_step;
		deflateRect(&v[i], gap, gap, gap, gap);
	}

	return v;
};

//! 将矩形水平平均分割为 n 份矩形, 当 hgap == -1 时，分割出的矩形与源矩形保持比例
inline std::vector<::RECT> split_rect_horizontal(::LPCRECT rc, int n, int wgap = 50, int hgap = -1) {
	std::vector<::RECT> v;

	int w = (width(rc) - (n + 1) * wgap) / n;

	if (hgap == -1) {
		double ratio = (rc->right - rc->left) * 1.0 / (rc->bottom - rc->top);
		int h = static_cast<int>(w / ratio);
		hgap = (height(rc) - h) / 2;
	}

	for (int i = 0; i < n; i++) {
		::RECT r = *rc;
		r.left += i * w + (i + 1) * wgap;
		r.right = r.left + w;
		r.top = rc->top + hgap;
		r.bottom = rc->bottom - hgap;
		v.push_back(r);
	}

	return v;
}

/**
* @brief 从矩形内获取六边形顶点
* @param rc 矩形
* @param spliter 六边形的边长是矩形边长的 1/spliter
* @note rc's width / spliter = (rc's width - hexagon's side length) / 2
* @return 六边形的六个顶点坐标，从上边左侧点顺时针排列
*/
inline std::vector<::POINT> getHexagonVertexesFromRect(::LPCRECT rc, float spliter = 3.0) {
	if (!rc) { return std::vector<::POINT>(); }

	if (spliter == 0.0) { spliter = 3.0; }

	std::vector<::POINT> v;
	auto w = rc->right - rc->left;
	auto h = rc->bottom - rc->top;
	auto ww = static_cast<int>(w / spliter);
	auto hh = static_cast<int>(h / spliter);

	::POINT pt;
	pt.x = rc->left;
	pt.y = rc->top + hh;
	v.push_back(pt);

	pt.x = rc->left + ww;
	pt.y = rc->top;
	v.push_back(pt);

	pt.x = rc->right - ww;
	v.push_back(pt);

	pt.x = rc->right;
	pt.y = rc->top + hh;
	v.push_back(pt);

	pt.y = rc->bottom - hh;
	v.push_back(pt);

	pt.x = rc->right - ww;
	pt.y = rc->bottom;
	v.push_back(pt);

	pt.x = rc->left + ww;
	v.push_back(pt);

	pt.x = rc->left;
	pt.y = rc->bottom - hh;
	v.push_back(pt);

	return v;
}


}

}
