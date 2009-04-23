// COMSentencesCollection.h : Declaration of the CCOMSentencesCollection

#ifndef __COM_SENTENCESCOLLECTION_H_
#define __COM_SENTENCESCOLLECTION_H_


#include "resource.h"       // main symbols
#include "SynAn.h"
#include "COMSentence.h"
#include "../SynCommonLib/SentencesCollection.h"
#include <windows.h>


/////////////////////////////////////////////////////////////////////////////
// CCOMSentencesCollection
class ATL_NO_VTABLE CCOMSentencesCollection : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCOMSentencesCollection, &CLSID_SentencesCollection>,
	public IDispatchImpl<ISentencesCollection, &IID_ISentencesCollection, &LIBID_SYNANLib>,
	public CSentencesCollection
{
public:

DECLARE_REGISTRY_RESOURCEID(IDR_SENTENCESCOLLECTION)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CCOMSentencesCollection)
	COM_INTERFACE_ENTRY(ISentencesCollection)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// ISentencesCollection
public:
	STDMETHOD(get_DeleteEqualPrimitiveClauseVariants)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(put_DeleteEqualPrimitiveClauseVariants)(/*[in]*/ BOOL newVal);
	STDMETHOD(SetOborDic)(/*[in]*/ IUnknown* Interf);
	STDMETHOD(SetLemmatizer)(/*[in]*/ IUnknown* Interf);
	STDMETHOD(get_KillHomonymsMode)(/*[out, retval]*/ UINT *pVal);
	STDMETHOD(put_KillHomonymsMode)(/*[in]*/ UINT newVal);
	//STDMETHOD(SetThesaurus)(/*[in]*/ UINT ThesId, /*[in]*/ IUnknown* Thes);
	STDMETHOD(put_SilentMode)(/*[in]*/ BOOL newVal);
	STDMETHOD(put_EnableThesauri)(/*[in]*/ BOOL newVal);
	STDMETHOD(get_EnableThesauri)(BOOL* Val);
	STDMETHOD(ClearSentences)();
	STDMETHOD(get_Sentence)(/*[in]*/long i, /*[out, retval]*/ ISentence* *pVal);
	STDMETHOD(get_SentencesCount)(/*[out, retval]*/ long *pVal);
	STDMETHOD(ProcessData)(/*[in, out]*/ IUnknown* piPlmLine);
	STDMETHOD(InitializeProcesser)();

	STDMETHOD(get_SyntaxLanguage)(int* pVal);
	STDMETHOD(put_SyntaxLanguage)(int newVal);
	STDMETHOD(get_EnableProgressBar)(BOOL* pVal);
	STDMETHOD(put_EnableProgressBar)(BOOL newVal);
};

#endif //__SENTENCESCOLLECTION_H_
