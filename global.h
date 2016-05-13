#pragma once

#include <string>
#include "utf8.h"
#include "mtverify.h"
#include "FileOper.h"
#include "log.h"
#include "chrono_wrapper.h"
#include "MyWSAError.h"
//#include "observer_macro.h"
#include "micro_getter_setter.h"
#include "micro_singleton.h"

namespace jlib {

inline std::wstring get_exe_path()
{
	wchar_t path[1024] = { 0 };
	GetModuleFileName(nullptr, path, 1024);
	std::wstring::size_type pos = std::wstring(path).find_last_of(L"\\/");
	return std::wstring(path).substr(0, pos);
}


class auto_timer : public boost::noncopyable
{
private:
	int m_timer_id;
	DWORD m_time_out;
	HWND m_hWnd;

public:

	auto_timer(HWND hWnd, int timerId, DWORD timeout) : m_hWnd(hWnd), m_timer_id(timerId), m_time_out(timeout)
	{
		KillTimer(hWnd, m_timer_id);
	}

	~auto_timer()
	{
		SetTimer(m_hWnd, m_timer_id, m_time_out, nullptr);
	}
};

};
