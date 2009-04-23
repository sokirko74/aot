// Domen.h : Declaration of the CComDomen

#ifndef __COMDOMEN_H_
#define __COMDOMEN_H_

#include "../common/utilit.h"
#include "../StructDictLib/Domen.h"      
#include "StructDict.h" 
#include "resource.h"      

class ATL_NO_VTABLE CComDomen : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CComDomen, &CLSID_Domain>,
	public IDispatchImpl<IDomain, &IID_IDomain, &LIBID_STRUCTDICTLib>
{
public:
	CDomen* m_pDomen;


DECLARE_REGISTRY_RESOURCEID(IDR_DOMEN)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CComDomen)
	COM_INTERFACE_ENTRY(IDomain)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IDomen
public:
	STDMETHOD(get_IsDelim)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(put_IsDelim)(/*[in]*/ BOOL newVal);
	STDMETHOD(FindItem)(/*[in]*/ BSTR ItemStr, /*[retval, out]*/ LONG* index);
	STDMETHOD(GetItemsCount)(/*[retval, out]*/ LONG * result);
	STDMETHOD(get_Items)(/*[in]*/ LONG index, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_IsFree)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(put_IsFree)(/*[in]*/ BOOL newVal);
	STDMETHOD(get_Format)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Format)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Parts)(/*[in]*/ BYTE PartNo, /*[out, retval]*/ BYTE *pVal);
	STDMETHOD(put_Parts)(/*[in]*/ BYTE PartNo, /*[in]*/ BYTE newVal);
	STDMETHOD(get_PartsSize)(/*[out, retval]*/ BYTE *pVal);
	STDMETHOD(get_Source)(/*[out, retval]*/ BYTE *pVal);
	STDMETHOD(put_Source)(/*[in]*/ BYTE newVal);
	STDMETHOD(get_DomStr)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_DomStr)(/*[in]*/ BSTR newVal);
};

#endif //__DOMEN_H_
