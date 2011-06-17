// ComSemWord.h : Declaration of the CComSemWord

#ifndef __COMSEMWORD_H_
#define __COMSEMWORD_H_

#include "resource.h"       // main symbols
#include "../SemanLib/SemanticRusStructure.h"
/////////////////////////////////////////////////////////////////////////////
// CComSemWord
class ATL_NO_VTABLE CComSemWord : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CComSemWord, &CLSID_ComSemWord>,
	public IDispatchImpl<IComSemWord, &IID_IComSemWord, &LIBID_SEMANLib>
{
public:
	const CSemWord* m_pWord;
	CComSemWord()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_COMSEMWORD)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CComSemWord)
	COM_INTERFACE_ENTRY(IComSemWord)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IComSemWord
public:
	STDMETHOD(get_Grammems)(/*[out, retval]*/ hyper *pVal);
	STDMETHOD(get_Poses)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_SyntaxWordNo)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_Lemma)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_WordStr)(/*[out, retval]*/ BSTR *pVal);
};

#endif //__COMSEMWORD_H_
