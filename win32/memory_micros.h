#pragma once

/////////*********************自定义宏********************////////////////////////
//安全删除普通堆内存
#define SAFEDELETEP(p) {if(p){delete (p); (p)=NULL;}}

#define SAFEDELETEARR(pArr) {if((pArr)){delete[] (pArr); (pArr)=NULL;}}

//安全删除对话框类堆内存
#define SAFEDELETEDLG(pDlg) {\
	if ((pDlg)) {\
		if (::IsWindow(pDlg->m_hWnd)) {	\
			(pDlg)->DestroyWindow(); \
		}\
		delete (pDlg); \
		(pDlg) = NULL; \
	}\
}

//关闭核心对象句柄
#define CLOSEHANDLE(h){\
	if (h != INVALID_HANDLE_VALUE && h != NULL) {\
		::CloseHandle(h); \
		h = INVALID_HANDLE_VALUE; \
	}\
}

#define CLOSESOCKET(s){\
	if (s != INVALID_SOCKET) {	\
		::closesocket(s); \
		s = INVALID_SOCKET; \
	}\
}