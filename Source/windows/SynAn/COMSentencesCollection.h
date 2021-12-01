#pragma once

#include "Resource.h"       // main symbols
#include "Synan_i.h"
#include "COMSentence.h"
#include <windows.h>
#include "synan/SynanLib/SentencesCollection.h"


class CSentencesCollection;
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
	STDMETHOD(SetLemmatizer)(/*[in]*/ IUnknown* Interf);
	STDMETHOD(get_KillHomonymsMode)(/*[out, retval]*/ UINT *pVal);
	STDMETHOD(put_KillHomonymsMode)(/*[in]*/ UINT newVal);
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

