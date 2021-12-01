#pragma once 

#include "agramtab_i.h" 
#include "agram_resource.h"       // main symbols

class CAgramtab;

class ATL_NO_VTABLE CComGramTab : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IGramTab, &IID_IGramTab, &LIBID_AGRAMTABLib>
{
protected:
	CAgramtab* m_pAgramtab;
	std::string _IN(BSTR str);
	BSTR _OUT(std::string& str);

public:
	CComGramTab();
	~CComGramTab();
	const CAgramtab* GetGramTab() {return m_pAgramtab;};


DECLARE_REGISTRY_RESOURCEID(IDR_RUSGRAMTAB)

DECLARE_PROTECT_FINAL_CONSTRUCT()
BEGIN_COM_MAP(CComGramTab)
	COM_INTERFACE_ENTRY(IGramTab)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()


public:

	STDMETHOD(get_GrammemsCount)(/*[out, retval]*/ LONG *pVal);
	STDMETHOD(get_PartOfSpeechCount)(/*[out, retval]*/ BYTE *pVal);
	STDMETHOD(GetGramCodeByGrammemsAndPartofSpeechIfCan)(/*[in]*/BYTE Pos, /*[in]*/unsigned hyper grammems, /*[out, retval]*/BSTR* gramcodes, BOOL* bRes);
	STDMETHOD(GetClauseNameByType)(/*[in]*/long type, /*[out, retval]*/BSTR* name);
	STDMETHOD(GetClauseTypeByNameIfCan)(/*[in]*/BSTR bstrTypeName, /*[out]*/long* lType, /*[out,retval]*/BOOL* Result);
	STDMETHOD(GetClauseTypeByName)(/*[in]*/BSTR bstrTypeName, /*[out,retval]*/long* lType);
	STDMETHOD(ProcessPOSAndGrammemsIfCan)(/*[in]*/BSTR in, /*[out]*/ BYTE* PartOfSpeech, /*[out]*/ unsigned hyper* Grammems,  BOOL* Result);
	STDMETHOD(GleicheSubjectPredicate)(/*[in]*/BSTR gram_code1, /*[in]*/BSTR gram_code2, /*[out]*/BOOL *bRes);
	STDMETHOD(FindGrammem)(/*[in]*/ BSTR gram_codes, /*[in]*/ unsigned hyper grammems, /*[retval,out]*/ BOOL* result);
	STDMETHOD(GleicheGenderNumber)(/*[in]*/ BSTR gram_code1,  /*[in]*/ BSTR gram_code2, /*[retval, out]*/ BOOL* result);
	STDMETHOD(GetAllGrammems)(/*[in]*/ BSTR gram_code, /*[retval, out]*/ unsigned hyper* result);
	STDMETHOD(ProcessPOSAndGrammems)(/*[in]*/BSTR in, /*[out]*/ BYTE* PartOfSpeech, /*[out]*/ unsigned hyper* Grammems);
	STDMETHOD(HaveEqualPartOfSpeech)(/*[in]*/BSTR gram_code1, /*[in]*/BSTR gram_code2, BOOL* result);
	STDMETHOD(Load)();
	STDMETHOD(GetPartOfSpeechStr)(/*[in]*/ BYTE PartOfSpeech, /*[out]*/ BSTR* result);
	STDMETHOD(GrammemsToStr)(/*[in]*/ unsigned hyper grammems, /*[out]*/ BSTR* result);
	STDMETHOD(GetGrammems)(/*[in]*/ BSTR gram_code, /*[out]*/ unsigned hyper* result);
	STDMETHOD(GetPartOfSpeech)(/*[in]*/ BSTR gram_code, BYTE* result);

	STDMETHOD(GleicheCaseNumber)(/*[in]*/ BSTR gram_code1,  /*[in]*/ BSTR  gram_code2, /*[retval, out]*/ BOOL* result);
	STDMETHOD(GleicheGenderNumberCase)(BSTR common_noun_gram_code, BSTR noun_gram_code, BSTR adj_code, BOOL *result);

	
};


class CComRusGramTab : 
	public CComCoClass<CComRusGramTab, &CLSID_RusGramTab>,
	public CComGramTab
	
{
public:
	CComRusGramTab();
	DECLARE_REGISTRY_RESOURCEID(IDR_RUSGRAMTAB)
	DECLARE_PROTECT_FINAL_CONSTRUCT()
	CAgramtab* NewGramTab();
};


class CComGerGramTab : 
	public CComCoClass<CComGerGramTab, &CLSID_GerGramTab>,
	public CComGramTab
	
{
public:
	CComGerGramTab();
	DECLARE_REGISTRY_RESOURCEID(IDR_GERGRAMTAB)
	DECLARE_PROTECT_FINAL_CONSTRUCT()
	CAgramtab* NewGramTab();
};


class CComEngGramTab : 
	public CComCoClass<CComEngGramTab, &CLSID_EngGramTab>,
	public CComGramTab
	
{
public:
	CComEngGramTab();
	DECLARE_REGISTRY_RESOURCEID(IDR_ENGGRAMTAB)
	DECLARE_PROTECT_FINAL_CONSTRUCT()
	CAgramtab* NewGramTab();
};
