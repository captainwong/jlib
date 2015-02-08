// LocalLock.h: interface for the CLocalLock class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOCALLOCK_H__FD10F2F9_1DF2_46A6_8261_1FA9E2AB061C__INCLUDED_)
#define AFX_LOCALLOCK_H__FD10F2F9_1DF2_46A6_8261_1FA9E2AB061C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLocalLock  
{
public:
	CLocalLock(LPCRITICAL_SECTION lpCriticalSection)
		:m_lpCriticalSection(lpCriticalSection)
	{
		EnterCriticalSection(m_lpCriticalSection);
	}
	~CLocalLock()
	{
		LeaveCriticalSection(m_lpCriticalSection);
	}
private:
	LPCRITICAL_SECTION m_lpCriticalSection;
};

class CLock
{
public:
	/*CLock(PCRITICAL_SECTION cs) : m_cs(cs), bNullInit(FALSE)
	{
		if (m_cs == NULL) {
			bNullInit = TRUE;
			m_cs = new CRITICAL_SECTION();
		}
		InitializeCriticalSection(m_cs);
	}*/
	CLock()/* : m_cs(NULL), bNullInit(FALSE)*/
	{
		/*if (m_cs == NULL) {
			m_cs = new CRITICAL_SECTION();
		}*/
		InitializeCriticalSection(&m_cs);
	}
	~CLock()
	{
		DeleteCriticalSection(&m_cs);
		/*if (bNullInit) {
			delete m_cs;
		}*/
	}
	void Lock()
	{
		EnterCriticalSection(&m_cs);
	}
	void UnLock()
	{
		LeaveCriticalSection(&m_cs);
	}

	const LPCRITICAL_SECTION GetObject() { return &m_cs; }
private:
	CRITICAL_SECTION m_cs;
	//BOOL bNullInit;
};


#endif // !defined(AFX_LOCALLOCK_H__FD10F2F9_1DF2_46A6_8261_1FA9E2AB061C__INCLUDED_)
