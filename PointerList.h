// PointerList.h: interface for the CPointerList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POINTERLIST_H__3FB7FEA8_DAE8_468B_9494_92CA39593E1A__INCLUDED_)
#define AFX_POINTERLIST_H__3FB7FEA8_DAE8_468B_9494_92CA39593E1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//自定义存取指针的链表，不需要手动回收指针（删除节点时自动删除指针所指向的内存）



class CPointerList  
{
	typedef struct _ELEMENT
	{
		void *element;
		_ELEMENT *pNext;
		_ELEMENT *pPrev;
		_ELEMENT() : element(NULL), pNext(NULL), pPrev(NULL)	{	}
	}ELEMENT, *PELEMENT;
public:
	CPointerList():m_pHead(NULL), m_pTail(NULL), m_nCount(0)	{	}

	virtual ~CPointerList()	{RemoveAll();}
public:
	void* GetAt(int index){
		PELEMENT pTmp = NULL;
		if(m_nCount > 0 && index >= 0 && index < m_nCount){
			pTmp = m_pHead;
			int i = 0;		
			while(i < index && pTmp){
				pTmp = pTmp->pNext;
				i++;
			}
			if(i == index && pTmp)
				return pTmp->element;
		}
		return NULL;
	}

	int Find(void *element){
		bool find = false;
		int index = 0;
		if(m_nCount > 0){
			PELEMENT pTmp = m_pHead;
			while(pTmp){
				if(pTmp->element == element){
					find = true;
					break;
				}
				pTmp = pTmp->pNext;
				index++;
			}
		}
		return find ? index : -1;
	}

	void AddTail(void *element){
		PELEMENT pTmp = new ELEMENT;
		pTmp->element = element;
		if(m_pHead == NULL){
			m_pHead = m_pTail = pTmp;
		}else{
			pTmp->pPrev = m_pTail;
			m_pTail->pNext = pTmp;
			m_pTail = pTmp;
		}
		m_nCount++;
	}

	void AddHead(void *element){
		PELEMENT pTmp = new ELEMENT;
		pTmp->element = element;
		if(m_pHead == NULL){
			m_pHead = m_pTail = pTmp;
		}else{
			pTmp->pNext = m_pHead;
			m_pHead->pPrev = pTmp;
			m_pHead = pTmp;
		}
		m_nCount++;
	}

	void RemoveAt(int index){
		if(m_nCount > 0 && index >= 0 && index < m_nCount){
			PELEMENT pCur = GetElementAt(index);
			if(pCur){
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

	void RemoveHead(){
		if(m_nCount > 0){
			PELEMENT pTmp = m_pHead->pNext;
			delete m_pHead;
			if(pTmp)
				pTmp->pPrev = NULL;
			m_pHead = pTmp;
			m_nCount--;
		}
	}

	void RemoveTail(){
		if(m_nCount > 0){
			PELEMENT pTmp = m_pTail->pPrev;
			delete m_pTail;
			if(pTmp)
				pTmp->pNext = NULL;
			m_pTail = pTmp;
			m_nCount--;
		}
	}

	void RemoveAll(){
		if(m_nCount > 0){
			PELEMENT pTmp = m_pHead;
			while(pTmp){
				m_pHead = pTmp;
				pTmp = pTmp->pNext;
				delete m_pHead;
			}
			m_pHead = m_pTail = NULL;
			m_nCount = 0;
		}
	}

	void InsertAt(int index, void *element){
		PELEMENT pCur = GetElementAt(index);
		if(pCur){
			PELEMENT pNew = new ELEMENT;
			pNew->element = element;

			PELEMENT pPrev = pCur->pPrev;
			if(pPrev){
				pNew->pPrev = pPrev;
				pPrev->pNext = pNew;
			}else{
				m_pHead = pNew;
			}

			pNew->pNext = pCur;
			pCur->pPrev = pNew;
			m_nCount++;
		}else{
			AddHead(element);
		}
	}

	int GetCount() const{
		return m_nCount;
	}

	void *GetHead(){
		return m_pHead ? m_pHead->element : NULL;
	}

	void *GetTail(){
		return m_pTail ? m_pTail->element : NULL;
	}

	void *GetNext(void *element){
		PELEMENT pTmp = FindElement(element);
		if(pTmp)
			pTmp = pTmp->pNext;
		return pTmp ? pTmp->element : NULL;
	}

	void *GetPrev(void *element){
		PELEMENT pTmp = FindElement(element);
		if(pTmp)
			pTmp = pTmp->pPrev;
		return pTmp ? pTmp->element : NULL;
	}

private:
	PELEMENT m_pHead;
	PELEMENT m_pTail;
	int m_nCount;
protected:
	PELEMENT GetElementAt(int index){
		PELEMENT pTmp = NULL;
		if(m_nCount > 0 && index >= 0 && index < m_nCount){
			pTmp = m_pHead;
			int i = 0;		
			while(i < index){
				pTmp = pTmp->pNext;
				i++;
			}
		}
		return pTmp;
	}

	PELEMENT FindElement(void *element){
		bool find = false;
		PELEMENT pTmp = NULL;
		if(m_nCount > 0){
			pTmp = m_pHead;
			while(pTmp){
				if(pTmp->element == element){
					find = true;
					break;
				}
				pTmp = pTmp->pNext;
			}
		}
		return find ? pTmp : NULL;
	}
};

#endif // !defined(AFX_POINTERLIST_H__3FB7FEA8_DAE8_468B_9494_92CA39593E1A__INCLUDED_)
