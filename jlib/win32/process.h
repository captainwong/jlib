#pragma once

#include <Windows.h>
#include <tlhelp32.h>
#include <string>
#include <vector>
#include <functional>
#include <unordered_set>
#include "lasterror.h"
#include "UnicodeTool.h"

namespace jlib
{

namespace win32
{

/**
* @brief 启动程序
* @param path 启动命令
* @param wait_app_exit 是否阻塞等待程序退出
* @param show 是否显示程序
* @return 启动程序失败返回0xFFFFFFFF；当 wait_app_exit 为真，返回程序的进程返回值；否则返回程序进程ID；
*/
inline DWORD daemon(const std::wstring& path, bool wait_app_exit = true, bool show = true) {
	STARTUPINFOW si = { 0 };
	si.cb = sizeof(si);
	si.dwFlags |= STARTF_USESHOWWINDOW;
	si.wShowWindow = show ? SW_SHOW : SW_HIDE;
	PROCESS_INFORMATION pi;
	DWORD dwCreationFlags = show ? 0 : CREATE_NO_WINDOW;
	BOOL bRet = CreateProcessW(NULL, (LPWSTR)(path.c_str()), NULL, NULL, FALSE, dwCreationFlags, NULL, NULL, &si, &pi);
	if (bRet) {
		if (wait_app_exit) {
			WaitForSingleObject(pi.hProcess, INFINITE);
			DWORD dwExit;
			::GetExitCodeProcess(pi.hProcess, &dwExit);
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
			return dwExit;
		} else {
			return pi.dwProcessId;
		}
	} else {
		auto msg = formatLastError("");
		(void)msg.length();
	}
	return 0xFFFFFFFF;
}

/**
* @brief 启动程序
* @param path 启动命令
* @param wait_app_exit 是否阻塞等待程序退出
* @param show 是否显示程序
* @return 启动程序失败返回0xFFFFFFFF；当 wait_app_exit 为真，返回程序的进程返回值；否则返回程序进程ID；
*/
inline DWORD daemon(const std::string& path, bool wait_app_exit = true, bool show = true) {
	return daemon(win32::mbcs_to_utf16(path), wait_app_exit, show);
}

/**
* @brief 获取父进程ID
* @return 成功返回父进程ID，否则返回0
*/
inline DWORD getppid()
{
	HANDLE hSnapshot;
	PROCESSENTRY32W pe32;
	DWORD ppid = 0, pid = GetCurrentProcessId();

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, pid);
	__try {
		if (hSnapshot == INVALID_HANDLE_VALUE) __leave;

		ZeroMemory(&pe32, sizeof(pe32));
		pe32.dwSize = sizeof(pe32);
		if (!Process32FirstW(hSnapshot, &pe32)) __leave;

		do {
			if (pe32.th32ProcessID == pid) {
				ppid = pe32.th32ParentProcessID;
				break;
			}
		} while (Process32NextW(hSnapshot, &pe32));

	} __finally {
		if (hSnapshot != INVALID_HANDLE_VALUE) CloseHandle(hSnapshot);
	}
	return ppid;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
// https://docs.microsoft.com/zh-cn/windows/win32/toolhelp/taking-a-snapshot-and-viewing-processes

struct ProcessInfo {

	struct ModuleInfo {
		std::wstring name;
		std::wstring path;
		DWORD pid;
		DWORD base_address;
		DWORD base_size;
	};

	struct ThreadInfo {
		DWORD tid;
		DWORD base_priority;
		DWORD delta_priority;
	};


	std::wstring name;
	std::wstring path;
	DWORD pid;
	DWORD ppid;
	DWORD thread_count;
	DWORD priority_base;
	DWORD priority_class;
	std::vector<ModuleInfo> modules;
	std::vector<ThreadInfo> threads;

	template <typename JsonValue>
	JsonValue toJson() const {
		JsonValue v;
		v["name"] = utf16_to_mbcs(name);
		v["path"] = utf16_to_mbcs(path);
		v["pid"] = (size_t)pid;
		v["ppid"] = (size_t)ppid;
		v["thread_count"] = (size_t)thread_count;
		v["priority_base"] = (size_t)priority_base;
		v["priority_class"] = (size_t)priority_class;

		auto& ms = v["modules"];
		for (const auto& m : modules) {
			JsonValue jm;
			jm["name"] = utf16_to_mbcs(m.name);
			jm["path"] = utf16_to_mbcs(m.path);
			jm["pid"] = (size_t)m.pid;
			jm["base_address"] = (size_t)m.base_address;
			jm["base_size"] = (size_t)m.base_size;
			ms.append(jm);
		}

		auto& ts = v["threads"];
		for (const auto& t : threads) {
			JsonValue jt;
			jt["tid"] = (size_t)t.tid;
			jt["base_priority"] = (size_t)t.base_priority;
			jt["delta_priority"] = (size_t)t.delta_priority;
			ts.append(jt);
		}

		return v;
	}
};

typedef std::vector<ProcessInfo> ProcessInfos;

template <typename JsonValue>
inline JsonValue toJson(const ProcessInfos& pinfos) {
	JsonValue v;
	for (const auto& pinfo : pinfos) {
		v.append(pinfo.toJson<JsonValue>());
	}
	return v;
}

typedef std::function<void(const std::wstring&)> ErrorOutputFunc;

inline void dummyErrorOutputFunc(const std::wstring& msg) {
	printf("%ls\n", msg.data());
}

inline void outputLastErrorHelper(const std::wstring& msg, ErrorOutputFunc func = nullptr) {
	if (!func) { return; }
	func(formatLastError(msg));
}

inline std::vector<ProcessInfo::ModuleInfo> getProcessModules(DWORD dwPID, ErrorOutputFunc output = dummyErrorOutputFunc)
{
	std::vector<ProcessInfo::ModuleInfo> modules = {};

	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32;

	// Take a snapshot of all modules in the specified process.
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	if (hModuleSnap == INVALID_HANDLE_VALUE) {
		outputLastErrorHelper((L"CreateToolhelp32Snapshot (of modules)"), output);
		return(modules);
	}

	// Set the size of the structure before using it.
	me32.dwSize = sizeof(MODULEENTRY32);

	// Retrieve information about the first module,
	// and exit if unsuccessful
	if (!Module32First(hModuleSnap, &me32)) {
		outputLastErrorHelper((L"Module32First"), output);  // show cause of failure
		CloseHandle(hModuleSnap);           // clean the snapshot object
		return(modules);
	}

	// Now walk the module list of the process,
	// and display information about each module
	do {
		ProcessInfo::ModuleInfo info;
		info.name = me32.szModule;
		info.path = me32.szExePath;
		info.pid = me32.th32ProcessID;
		info.base_address = (DWORD)me32.modBaseAddr;
		info.base_size = me32.modBaseSize;
		modules.emplace_back(info);
	} while (Module32Next(hModuleSnap, &me32));

	CloseHandle(hModuleSnap);
	return(modules);
}

inline std::vector<ProcessInfo::ThreadInfo> getProcessThreads(DWORD dwOwnerPID, ErrorOutputFunc output = dummyErrorOutputFunc)
{
	std::vector<ProcessInfo::ThreadInfo> threads = {};

	HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
	THREADENTRY32 te32;

	// Take a snapshot of all running threads  
	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, dwOwnerPID);
	if (hThreadSnap == INVALID_HANDLE_VALUE)
		return(threads);

	// Fill in the size of the structure before using it. 
	te32.dwSize = sizeof(THREADENTRY32);

	// Retrieve information about the first thread,
	// and exit if unsuccessful
	if (!Thread32First(hThreadSnap, &te32)) {
		outputLastErrorHelper((L"Thread32First"), output); // show cause of failure
		CloseHandle(hThreadSnap);          // clean the snapshot object
		return(threads);
	}

	// Now walk the thread list of the system,
	// and display information about each thread
	// associated with the specified process
	do {
		if (te32.th32OwnerProcessID == dwOwnerPID) {
			ProcessInfo::ThreadInfo tinfo;
			tinfo.tid = te32.th32ThreadID;
			tinfo.base_priority = te32.tpBasePri;
			tinfo.delta_priority = te32.tpDeltaPri;
			threads.emplace_back(tinfo);
		}
	} while (Thread32Next(hThreadSnap, &te32));

	CloseHandle(hThreadSnap);
	return(threads);
}

static const wchar_t* PROCESS_FILTER[] = {
	L"[System Process]",
	L"ApplicationFrameHost.exe",
	L"AppVShNotify.exe",
	L"audiodg.exe",
	L"backgroundTaskHost.exe",
	L"ChsIME.exe",
	L"CompPkgSrv.exe",
	L"conhost.exe",
	L"csrss.exe",
	L"ctfmon.exe",
	L"dasHost.exe",
	L"dllhost.exe",
	L"dwm.exe",
	L"fontdrvhost.exe",
	L"GameBarFTServer.exe",
	L"LockApp.exe",
	L"LogonUI.exe",
	L"lsass.exe",
	L"lsm.exe",
	L"Memory Compression",
	L"Microsoft.Photos.exe",
	L"msdtc.exe",
	L"mstsc.exe",
	L"pacjsworker.exe",
	L"PresentationFontCache.exe",
	L"rdpclip.exe",
	L"Registry",
	L"RemindersServer.exe",
	L"rundll32.exe",
	L"RuntimeBroker.exe",
	L"schtasks.exe",
	L"SearchFilterHost.exe",
	L"SearchIndexer.exe",
	L"SearchProtocolHost.exe",
	L"SearchUI.exe",
	L"SecurityHealthService.exe",
	L"SecurityHealthSystray.exe",
	L"services.exe",
	L"SettingSyncHost.exe",
	L"ShellExperienceHost.exe",
	L"sihost.exe",
	L"SkypeApp.exe",
	L"SkypeBackgroundHost.exe",
	L"SkypeBridge.exe",
	L"smartscreen.exe",
	L"smss.exe",
	L"SgrmBroker.exe",
	L"spoolsv.exe",
	L"StartMenuExperienceHost.exe",
	L"svchost.exe",
	L"System",
	L"SystemSettingsBroker.exe",
	L"TabTip.exe",
	L"taskhost.exe",
	L"taskhostw.exe",
	L"TiWorker.exe",
	L"TrustedInstaller.exe",
	L"Video.UI.exe",
	L"WindowsInternal.ComposableShell.Experiences.TextInput.InputApp.exe",
	L"wininit.exe",
	L"winlogon.exe",
	L"WinStore.App.exe",
	L"WmiPrvSE.exe",
	L"wmpnetwk.exe",
	L"WUDFHost.exe",
	L"YourPhone.exe",
};

inline ProcessInfos getProcessesInfo(ErrorOutputFunc output = dummyErrorOutputFunc, bool withModules = false, bool withThreads = false, const wchar_t** filter = PROCESS_FILTER, size_t filter_count = _countof(PROCESS_FILTER))
{
	// pre-process filter
	std::unordered_set<std::wstring> filterset;
	for (size_t i = 0; i < filter_count; i++) {
		filterset.insert(filter[i]);
	}

	HANDLE hProcessSnap;
	HANDLE hProcess;
	PROCESSENTRY32 pe32;
	DWORD dwPriorityClass;

	ProcessInfos pinfos = {};

	// Take a snapshot of all processes in the system.
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE) {
		outputLastErrorHelper(L"CreateToolhelp32Snapshot (of processes)", output);
		return(pinfos);
	}

	// Set the size of the structure before using it.
	pe32.dwSize = sizeof(PROCESSENTRY32);

	// Retrieve information about the first process,
	// and exit if unsuccessful
	if (!Process32First(hProcessSnap, &pe32)) {
		outputLastErrorHelper((L"Process32First"), output); // show cause of failure
		CloseHandle(hProcessSnap);          // clean the snapshot object
		return(pinfos);
	}

	// Now walk the snapshot of processes, and
	// display information about each process in turn
	do {
		if (filterset.find(pe32.szExeFile) != filterset.end()) { continue; }

		ProcessInfo pinfo = {};
		pinfo.name = pe32.szExeFile;

		// Retrieve the priority class.
		dwPriorityClass = 0;
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
		if (hProcess == NULL)
			outputLastErrorHelper((L"OpenProcess"), output);
		else {
			dwPriorityClass = GetPriorityClass(hProcess);
			if (!dwPriorityClass)
				outputLastErrorHelper((L"GetPriorityClass"), output);
		}

		pinfo.pid = pe32.th32ProcessID;
		pinfo.ppid = pe32.th32ParentProcessID;
		pinfo.thread_count = pe32.cntThreads;
		pinfo.priority_base = pe32.pcPriClassBase;

		if (dwPriorityClass) {
			pinfo.priority_class = dwPriorityClass;
		}

		// List the modules and threads associated with this process
		pinfo.modules = getProcessModules(pe32.th32ProcessID, output);

		// try to get process exe path
		if (!pinfo.modules.empty()) {
			pinfo.path = pinfo.modules.front().path;
		}
		if (!withModules) {
			pinfo.modules.clear();
		}
		if (withThreads) {
			pinfo.threads = getProcessThreads(pe32.th32ProcessID, output);
		}

		// try to get process exe path again
		if (pinfo.path.empty()) {
			if (!hProcess) {
				hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pe32.th32ProcessID);
			}
			if (hProcess) {
				wchar_t path[4096]; DWORD len = 4096;
				if (QueryFullProcessImageNameW(hProcess, 0, path, &len)) {
					pinfo.path = path;
				} else {
					outputLastErrorHelper(L"QueryFullProcessImageNameW", output);
				}
			} else {
				outputLastErrorHelper(L"OpenProcess", output);
			}
		}

		pinfos.emplace_back(pinfo);

		if(hProcess){ CloseHandle(hProcess); }
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	return(pinfos);
}


} // namespace win32

} // namespace jlib
