// PointerList.cpp: implementation of the CPointerList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PointerList.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
CPointerList::CPointerList()
{
	m_pHead = m_pTail = NULL; 
	m_nCount = 0;
}

CPointerList::~CPointerList()
{
	RemoveAll();
}

void CPointerList::AddTail(void *element)
{
	PELEMENT pTmp = new ELEMENT;
	pTmp->element = element;
	if(m_pHead == NULL)
	{
		m_pHead = m_pTail = pTmp;
	}
	else
	{
		pTmp->pPrev = m_pTail;
		m_pTail->pNext = pTmp;
		m_pTail = pTmp;
	}

	m_nCount++;

	int * p = (int*)&m_pHead;
	int *p2 = &m_nCount;
}

void CPointerList::AddHead(void *element)
{
	PELEMENT pTmp = new ELEMENT;
	pTmp->element = element;
	if(m_pHead == NULL)
	{
		m_pHead = m_pTail = pTmp;
	}
	else
	{
		pTmp->pNext = m_pHead;
		m_pHead->pPrev = pTmp;
		m_pHead = pTmp;
	}

	m_nCount++;
}

void CPointerList::RemoveAt(int index)
{
	if(m_nCount > 0 && index >= 0 && index < m_nCount)
	{
		PELEMENT pCur = GetElementAt(index);
		if(pCur)
		{
			PELEMENT pPrev = pCur->pPrev;
			PELEMENT pNext = pCur->pNext;
			delete pCur;
			pCur = NULL;

			if(pPrev)
				pPrev->pNext = pNext;
			else
				m_pHead = pNext;

			if(pNext)
				pNext->pPrev = pPrev;
			else
				m_pTail = pPrev;

			m_nCount--;
		}
	}
}

void CPointerList::RemoveHead()
{
	if(m_nCount > 0)
	{
		PELEMENT pTmp = m_pHead->pNext;
		delete m_pHead;
		if(pTmp)
			pTmp->pPrev = NULL;
		m_pHead = pTmp;
		m_nCount--;
	}
}

void CPointerList::RemoveTail()
{
	if(m_nCount > 0)
	{
		PELEMENT pTmp = m_pTail->pPrev;
		delete m_pTail;
		if(pTmp)
			pTmp->pNext = NULL;
		m_pTail = pTmp;
		m_nCount--;
	}
}

void CPointerList::RemoveAll()
{
	if(m_nCount > 0)
	{
		PELEMENT pTmp = m_pHead;
		while(pTmp)
		{
			m_pHead = pTmp;
			pTmp = pTmp->pNext;
			delete m_pHead;
		}
		m_pHead = m_pTail = NULL;
		m_nCount = 0;
	}
}

void CPointerList::InsertAt(int index, void *element)
{
	PELEMENT pCur = GetElementAt(index);

	if(pCur)
	{
		PELEMENT pNew = new ELEMENT;
		pNew->element = element;

		PELEMENT pPrev = pCur->pPrev;
		if(pPrev)
		{
			pNew->pPrev = pPrev;
			pPrev->pNext = pNew;
		}
		else
		{
			m_pHead = pNew;
		}

		pNew->pNext = pCur;
		pCur->pPrev = pNew;
		m_nCount++;
	}
	else
	{
		AddHead(element);
	}
}

int CPointerList::GetCount()
{
	return m_nCount;
}

void *CPointerList::GetHead()
{
	return m_pHead->element;
}

void *CPointerList::GetTail()
{
	return m_pTail->element;
}

void *CPointerList::GetNext(void *element)
{
	PELEMENT pTmp = FindElement(element);
	if(pTmp)
	{
		pTmp = pTmp->pNext;
	}
	return pTmp ? pTmp->element : NULL;
}

void *CPointerList::GetPrev(void *element)
{
	PELEMENT pTmp = FindElement(element);
	if(pTmp)
	{
		pTmp = pTmp->pPrev;
	}
	return pTmp ? pTmp->element : NULL;
}

int CPointerList::Find(void *element)
{
	bool find = false;
	int index = 0;
	if(m_nCount > 0)
	{
		PELEMENT pTmp = m_pHead;
		while(pTmp)
		{
			if(pTmp->element == element)
			{
				find = true;
				break;
			}
			pTmp = pTmp->pNext;
			index++;
		}
	}
	return find ? index : -1;
}

PELEMENT CPointerList::GetElementAt(int index)
{
	PELEMENT pTmp = NULL;
	if(m_nCount > 0 && index >= 0 && index < m_nCount)
	{
		pTmp = m_pHead;
		int i = 0;		
		while(i < index)
		{
			pTmp = pTmp->pNext;
			i++;
		}
	}
	return pTmp;
}

void* CPointerList::GetAt(int index)
{
	PELEMENT pTmp = NULL;
	if(m_nCount > 0 && index >= 0 && index < m_nCount)
	{
		pTmp = m_pHead;
		int i = 0;		
		while(i < index && pTmp)
		{
			pTmp = pTmp->pNext;
			i++;
		}
		if(i == index && pTmp)
			return pTmp->element;
	}
	return NULL;
}

PELEMENT CPointerList::FindElement(void *element)
{
	bool find = false;
	PELEMENT pTmp = NULL;
	if(m_nCount > 0)
	{
		pTmp = m_pHead;
		while(pTmp)
		{
			if(pTmp->element == element)
			{
				find = true;
				break;
			}
			pTmp = pTmp->pNext;
		}
	}
	return find ? pTmp : NULL;
}


