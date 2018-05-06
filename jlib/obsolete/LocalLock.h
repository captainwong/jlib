#pragma once

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

	BOOL TryLock()
	{
		return ::TryEnterCriticalSection(&m_cs);
	}

	LPCRITICAL_SECTION GetLockObject() { return &m_cs; }
private:
	CRITICAL_SECTION m_cs;
	//BOOL bNullInit;
};

