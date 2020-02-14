//================================
// ==========   Dialing Lemmatizer (www.aot.ru)
//========== Sokirko Alexey sokirko@yandex.ru
//================================

#ifndef __COM_PARADIGM_H_
#define __COM_PARADIGM_H_

#include "Lemmatizer.h"      
#include "atlcom.h"
#include "comdef.h"

#include "../LemmatizerLib/Paradigm.h"       
#include "lemmatizer_resource.h"       // main symbols


/////////////////////////////////////////////////////////////////////////////
// CCOMParadigm
class ATL_NO_VTABLE CCOMParadigm : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCOMParadigm, &CLSID_Paradigm>,
	public IDispatchImpl<IParadigm, &IID_IParadigm, &LIBID_LEMMATIZERLib>,
	public CFormInfo
{
public:
	CCOMParadigm() : CFormInfo() {};


DECLARE_REGISTRY_RESOURCEID(IDR_COMPARADIGM)
DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CCOMParadigm)
	COM_INTERFACE_ENTRY(IParadigm)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IParadigm
public:

	STDMETHOD(get_ParadigmID)(/*[out, retval]*/ DWORD *pVal);
	STDMETHOD(put_ParadigmID)(/*[in]*/ DWORD newVal);
	STDMETHOD(get_Count)(/*[out, retval]*/ DWORD *pVal);
	STDMETHOD(get_Ancode)(/*[in]*/ DWORD pos, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_Norm)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_Form)(/*[in]*/ DWORD pos, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_TypeAncode)(BSTR *pVal);
	STDMETHOD(get_Founded)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(get_SrcAncode)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_SrcNorm)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_HomonymWeightWithForm)(/*[in]*/ DWORD pos, /*[out, retval]*/ int *pVal);
	STDMETHOD(get_HomonymWeight)(/*[out, retval]*/ int *pVal);
	STDMETHOD(get_WordWeight)(/*[out, retval]*/ int *pVal);
	STDMETHOD(get_Accent)(DWORD pos, BYTE* pVal);
	STDMETHOD(get_BaseLength)(UINT* pVal);
	STDMETHOD(get_LemmaPrefixLength)(int* pVal);
};


#endif //__PARADIGM_H_
