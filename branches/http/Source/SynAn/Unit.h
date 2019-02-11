// Unit.h : Declaration of the CCOMUnit

#ifndef __UNIT_H_
#define __UNIT_H_

#include "resource.h"       // main symbols
#include "../SynCommonLib/Sentence.h"
#include "SynAn.h"
#include "../SynCommonLib/Clause.h"
#include <windows.h>

/////////////////////////////////////////////////////////////////////////////
// CCOMUnit
class ATL_NO_VTABLE CCOMUnit : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCOMUnit, &CLSID_SyntaxUnit>,
	public IDispatchImpl<ISyntaxUnit, &IID_ISyntaxUnit, &LIBID_SYNANLib>
{
public:
	CCOMUnit()
	{
		m_pClause = NULL;
		m_pUnit = NULL;
	}

DECLARE_REGISTRY_RESOURCEID(IDR_UNIT)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CCOMUnit)
	COM_INTERFACE_ENTRY(ISyntaxUnit)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// ISyntaxUnit
public:
	STDMETHOD(get_SimplePrepStr)(long ii,/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_SimplePrepId)(/*[in]*/ long ii, /*[out, retval]*/ long *pVal);
	STDMETHOD(get_SimplePrepsCount)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_ClauseNum)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_WordNum)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_Type)(/*[out, retval]*/ int *pVal);
	STDMETHOD(get_GrammemsStr)( /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_Gramcodes)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_Grammems)(/*[out, retval]*/ hyper *pVal);

	STDMETHOD(get_ActiveClauseTypeNum)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_ActiveHomonymNum)(/*[out, retval]*/ long *pVal);

	BOOL InitUnit(const CClause* pClause, const CMorphVariant&	pSynVariant, int UnitNum, int iClauseNum  = -1);

protected:
	const CSynUnit*					m_pUnit;
	const CClause*					m_pClause;
	int								m_iClauseNum;

};

#endif //__UNIT_H_
