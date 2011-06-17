
#ifndef __COMSIGNAT_H_
#define __COMSIGNAT_H_


#include "resource.h"       // main symbols
#include "StructDict.h"       // main symbols
#include "../StructDictLib/Signat.h"       // main symbols

class ATL_NO_VTABLE CComSignat : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CComSignat, &CLSID_Signat>,
	public IDispatchImpl<ISignat, &IID_ISignat, &LIBID_STRUCTDICTLib>
	
{
public:
	CSignat*	m_pSignat;
	bool		m_bTemporary;
	CComSignat()
	{
		m_bTemporary = false;
	};
	~CComSignat()
	{
		if (m_bTemporary)
			delete m_pSignat;
	};
	

DECLARE_REGISTRY_RESOURCEID(IDR_SIGNAT)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CComSignat)
	COM_INTERFACE_ENTRY(ISignat)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// ISignat
public:
	STDMETHOD(CreateTempSignat)();
	STDMETHOD(get_FormatForDomain)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_SignatId)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_SignatId)(/*[in]*/ long newVal);
	STDMETHOD(get_DomsCount)(/*[out, retval]*/ BYTE *pVal);
	STDMETHOD(put_DomsCount)(/*[in]*/ BYTE newVal);
	STDMETHOD(get_Doms)(/*[in]*/ BYTE index, /*[out, retval]*/ BYTE *pVal);
	STDMETHOD(put_Doms)(/*[in]*/ BYTE index, /*[in]*/ BYTE newVal);
	STDMETHOD(get_DomsWithDelimsCount)(/*[out, retval]*/ BYTE *pVal);
	STDMETHOD(put_DomsWithDelimsCount)(/*[in]*/ BYTE newVal);
	STDMETHOD(get_DomsWithDelims)(/*[in]*/ BYTE index, /*[out, retval]*/ BYTE *pVal);
	STDMETHOD(put_DomsWithDelims)(/*[in]*/ BYTE index, /*[in]*/ BYTE newVal);
	STDMETHOD(get_OrderNo)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_OrderNo)(/*[in]*/ long newVal);
	STDMETHOD(get_DomainString)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_DomainString)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Name)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Name)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_FormatForPrintf)(/*[out, retval]*/ BSTR *pVal);
};


#endif //__SIGNAT_H_
