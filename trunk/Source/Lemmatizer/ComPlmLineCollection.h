// PLMLineCollection.h : Declaration of the CCOMPLMLineCollection

#ifndef __COMPLMLINECOLLECTION_H_
#define __COMPLMLINECOLLECTION_H_

//#include "Lemmatizer.h"       // main symbols
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

	STDMETHOD(get_Count)(/*[out, retval]*/ long *pVal)
	{
		*pVal = m_Items.size();
		return S_OK;
	}
	STDMETHOD(get_Item)(/*[in]*/ long pos, /*[out, retval]*/ BSTR *pVal)
	{
		if (pos >= m_Items.size())
			return E_FAIL;
		*pVal = _bstr_t(m_Items[pos].c_str()).copy();
		return S_OK;
	}
	STDMETHOD(put_Item)(/*[in]*/ long pos, /*[in]*/ BSTR newVal)
	{
		if (pos >= m_Items.size())
			return E_FAIL;
		m_Items[pos] = (const char*)_bstr_t(newVal);
		return S_OK;
	}
	STDMETHODIMP CCOMPLMLineCollection::InsertLine(int LineNo, BSTR LineStr)
	{
	   // TODO: Add your implementation code here
		if (LineNo >= m_Items.size())
			m_Items.push_back((const char*)_bstr_t(LineStr));
		else
			m_Items.insert( m_Items.begin()+LineNo, (const char*)_bstr_t(LineStr));
		return S_OK;
	}

	STDMETHOD(Remove)(/*[in]*/ long pos)
	{
		if (pos >= m_Items.size())
			return E_FAIL;
		m_Items.erase(m_Items.begin() + pos);
		return S_OK;
	}
	STDMETHOD(ProcessPlmLines)(/*[in]*/ IUnknown* piGraphmatFile);
	STDMETHOD(SaveToFile)(/*[in]*/ BSTR filename);
private:
	CCOMLemmatizer*		m_pComLemmatizer;

public:
	STDMETHOD(ProcessHyphenWords)(IUnknown* piGraphan);
	STDMETHOD(CopyItems)(IUnknown* piPlmLinesFrom);
};

#endif //__COMPLMLINECOLLECTION_H_
