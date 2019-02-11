// ParadigmCollection.h : Declaration of the CCOMParadigmCollection

#ifndef __PARADIGMCOLLECTION_H_
#define __PARADIGMCOLLECTION_H_

#include "lemmatizer_resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CCOMParadigmCollection
class ATL_NO_VTABLE CCOMParadigmCollection : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCOMParadigmCollection, &CLSID_ParadigmCollection>,
	public IDispatchImpl<IParadigmCollection, &IID_IParadigmCollection, &LIBID_LEMMATIZERLib>,
	public vector<	CComObject<CCOMParadigm>*	>
{
public:
	CCOMParadigmCollection()
	{
	}
	~CCOMParadigmCollection()
	{
		for (size_t i = 0; i < size(); i++)
			at(i)->Release();
	}
DECLARE_REGISTRY_RESOURCEID(IDR_COMPARADIGMCOLLECTION)
DECLARE_PROTECT_FINAL_CONSTRUCT()


BEGIN_COM_MAP(CCOMParadigmCollection)
	COM_INTERFACE_ENTRY(IParadigmCollection)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IParadigmCollection
public:
	STDMETHOD(get_Count)(/*[out, retval]*/ long *pVal)
	{
		*pVal = size();
		return S_OK;
	}
	STDMETHOD(get_Item)(/*[in]*/ long pos, /*[out, retval]*/ IParadigm* *pVal)
	{
		if (pos >= size())
			return E_FAIL;
		at(pos)->AddRef();
		*pVal = at(pos);
		return S_OK;
	}
};


#endif //__PARADIGMCOLLECTION_H_
