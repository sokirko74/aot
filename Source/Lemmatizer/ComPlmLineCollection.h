// PLMLineCollection.h : Declaration of the CCOMPLMLineCollection
#pragma once

#include "ComLemmatizers.h"       // main symbols
#include "lemmatizer_resource.h"       // main symbols
#include "../LemmatizerLib/PlmLineCollection.h"


class ATL_NO_VTABLE CCOMPLMLineCollection : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCOMPLMLineCollection, &CLSID_PLMLineCollection>,
	public IDispatchImpl<IPLMLineCollection, &IID_IPLMLineCollection, &LIBID_LEMMATIZERLib>,
	public CPlmLineCollection
{
public:
	CCOMPLMLineCollection()
	{
		m_pLemmatizer = 0;
	};
	~CCOMPLMLineCollection()
	{
		m_pLemmatizer = 0;
	};

DECLARE_REGISTRY_RESOURCEID(IDR_COMPLMLINECOLLECTION)
DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CCOMPLMLineCollection)
	COM_INTERFACE_ENTRY(IPLMLineCollection)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

public:
	STDMETHOD(Clear)();
	STDMETHOD(AttachLemmatizer)(ILemmatizer* pVal);
	STDMETHOD(get_Lemmatizer)(IUnknown** pVal);

	STDMETHOD(get_Count)(/*[out, retval]*/ long* pVal);
	STDMETHOD(get_Item)(/*[in]*/ long pos, /*[out, retval]*/ BSTR* pVal);
	STDMETHOD(put_Item)(/*[in]*/ long pos, /*[in]*/ BSTR newVal);
	STDMETHOD(AddLine)	(BSTR LineStr);
	STDMETHOD(Remove)(/*[in]*/ long pos);
	STDMETHOD(ProcessPlmLines)(/*[in]*/ IUnknown* piGraphmatFile);
	STDMETHOD(SaveToFile)(/*[in]*/ BSTR filename);
	STDMETHOD(ProcessHyphenWords)(IUnknown* piGraphan);
	STDMETHOD(CopyItems)(IUnknown* piPlmLinesFrom);
private:
	CCOMLemmatizer*		m_pComLemmatizer;

};
