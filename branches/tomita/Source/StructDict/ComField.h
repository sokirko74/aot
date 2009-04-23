// Field.h : Declaration of the CField

#ifndef __COMFIELD_H_
#define __COMFIELD_H_

#include "../StructDictLib/Field.h"


#include "resource.h"       // main symbols

class ATL_NO_VTABLE CComField : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CComField, &CLSID_Field>,
	public IDispatchImpl<IField, &IID_IField, &LIBID_STRUCTDICTLib>
{
public:
	CField* m_pField;


DECLARE_REGISTRY_RESOURCEID(IDR_FIELD)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CComField)
	COM_INTERFACE_ENTRY(IField)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IField
public:
	STDMETHOD(AddSignat)();
	STDMETHOD(get_SignatCount)(/*[out, retval]*/ BYTE *pVal);
	STDMETHOD(get_Signats)(/*[in]*/ BYTE SignatNo, /*[out, retval]*/ ISignat* *pVal);
	STDMETHOD(put_Signats)(/*[in]*/ BYTE SignatNo, /*[in]*/ ISignat* newVal);
	STDMETHOD(get_IsApplicToActant)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(put_IsApplicToActant)(/*[in]*/ BOOL newVal);
	STDMETHOD(get_OrderId)(/*[out, retval]*/ LONG *pVal);
	STDMETHOD(put_OrderId)(/*[in]*/ LONG newVal);
	STDMETHOD(get_Type)(/*[out, retval]*/ BYTE *pVal);
	STDMETHOD(put_Type)(/*[in]*/ BYTE newVal);
	STDMETHOD(get_FieldStr)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_FieldStr)(/*[in]*/ BSTR newVal);
	STDMETHOD(DeleteLastSignat)(void);
};

#endif //__FIELD_H_
