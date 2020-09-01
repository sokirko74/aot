#pragma once 

#include "resource.h"       // main symbols
#include "Synan_i.h"

class ATL_NO_VTABLE CCOMHomonym : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCOMHomonym, &CLSID_Homonym>,
	public IDispatchImpl<IHomonym, &IID_IHomonym, &LIBID_SYNANLib>
{
public:
	CCOMHomonym();
	~CCOMHomonym();

DECLARE_REGISTRY_RESOURCEID(IDR_HOMONYM)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CCOMHomonym)
	COM_INTERFACE_ENTRY(IHomonym)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IHomonym
public:
	STDMETHOD(get_CommonGrammemsStr)(BSTR* pVal);
	STDMETHOD(get_Poses)(long *pVal);
	STDMETHOD(get_LemSign)( BSTR *pVal);
	STDMETHOD(get_Grammems)( hyper *pVal);
	STDMETHOD(get_Lemma)( BSTR *pVal);
	STDMETHOD(GetOborotId)( long* OborotId);
	STDMETHOD(GetOborotGF)( BSTR *pVal);
	STDMETHOD(get_SimplePrepsCount)( long *pVal);
	STDMETHOD(GetOborDictIdOfSimplePrep)(long iNum, long *pVal);
	STDMETHOD(get_IsInOb)( BOOL *pVal);
	STDMETHOD(get_IsOb2)( BOOL *pVal);
	STDMETHOD(get_IsOb1)( BOOL *pVal);
	STDMETHOD(get_POSStr)( BSTR *pVal);
	STDMETHOD(get_GramCodes)( BSTR *pVal);
	STDMETHOD(get_ParadigmID)( long *pVal);
	STDMETHOD(get_GramDescriptionStr)( BSTR *pVal);

	BOOL Init(const CSynHomonym& pHom, CSentence* pSent);
	

protected:
	CSynHomonym* m_pHomonym;
	CSentence* m_pSent;
	const CSyntaxOpt* GetOpt() const;

};
