
#ifndef __COMTEXTFIELD_H_
#define __COMTEXTFIELD_H_



#include "../StructDictLib/TextField.h"

#include "resource.h"       // main symbols

class ATL_NO_VTABLE CComTextField : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CComTextField, &CLSID_TextField>,
	public IDispatchImpl<ITextField, &IID_ITextField, &LIBID_STRUCTDICTLib>

{
public:
	CTextField* m_pField;

DECLARE_REGISTRY_RESOURCEID(IDR_TEXTFIELD)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CComTextField)
	COM_INTERFACE_ENTRY(ITextField)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// ITextField
public:
	STDMETHOD(get_BracketLeafId)(/*[out, retval]*/ BYTE *pVal);
	STDMETHOD(put_BracketLeafId)(/*[in]*/ BYTE newVal);
	STDMETHOD(get_LeafId)(/*[out, retval]*/ BYTE *pVal);
	STDMETHOD(put_LeafId)(/*[in]*/ BYTE newVal);
	STDMETHOD(get_FieldNo)(/*[out, retval]*/ BYTE *pVal);
	STDMETHOD(put_FieldNo)(/*[in]*/ BYTE newVal);
	STDMETHOD(get_EndLine)(/*[out, retval]*/ DWORD *pVal);
	STDMETHOD(put_EndLine)(/*[in]*/ DWORD newVal);
	STDMETHOD(get_StartLine)(/*[out, retval]*/ DWORD *pVal);
	STDMETHOD(put_StartLine)(/*[in]*/ DWORD newVal);
};


#endif //__TEXTFIELD_H_
