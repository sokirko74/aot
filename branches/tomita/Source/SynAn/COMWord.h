// COMWord.h : Declaration of the CCOMWord

#ifndef __WORD_H_
#define __WORD_H_

#include "resource.h"       // main symbols
#include "ComHomonym.h"
#include "StdAfx.h"
#include "SynAn.h"
#include "COMClauseVariant.h"


/////////////////////////////////////////////////////////////////////////////
// CCOMWord
class ATL_NO_VTABLE CCOMWord : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCOMWord, &CLSID_Word>,
	public IDispatchImpl<IWord, &IID_IWord, &LIBID_SYNANLib>
{
public:
	CCOMWord()
	{
		m_pWord = NULL;
		m_iWordNum = -1;
	}

DECLARE_REGISTRY_RESOURCEID(IDR_WORD)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CCOMWord)
	COM_INTERFACE_ENTRY(IWord)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IWord
public:
	STDMETHOD(get_MainVerbsCount)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_MainVerbs)(long lVerbNo, long *pVal);
	STDMETHOD(get_Homonym)(/*[in]*/ long lHomNum, /*[out, retval]*/ IHomonym* *pVal);
	STDMETHOD(get_HomonymsCount)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_WordStr)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_bArtificialCreated)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(get_bDeleted)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(get_GraphDescrs)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_ClauseNo)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_HomonymByClauseVar)(/*[in]*/IClauseVariant* piClauseVariant, /*[out, retval]*/ IHomonym* *pVal);
	STDMETHOD(get_ThesType)(/*[out, retval]*/ int *pVal);
	STDMETHOD(get_IsFirstInThesaurusEntry)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(get_IsLastInThesaurusEntry)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(get_ThesaurusEntryID)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_Register)(/*[out, retval]*/ int *pVal);
	STDMETHOD(get_IsInThesaurusEntry)(/*[out, retval]*/ BOOL *pVal);

	IHomonym* GetHomonymByClauseVar(CCOMClauseVariant* pVar);
	BOOL Init(const CSynWord* pWord, int iWordNum, CSentence*  pSent)
	{
		if( !pWord )
			return FALSE;
		m_pWord = pWord;
		m_iWordNum = iWordNum;
		m_pSent = pSent;
		return TRUE;
	};

protected:	
	const CSynWord* m_pWord;
	int m_iWordNum;
	CSentence* m_pSent;

public:
};

#endif //__WORD_H_

