// COMClauseVariant.h : Declaration of the CCOMClauseVariant

#ifndef __CLAUSEVARIANT_H_
#define __CLAUSEVARIANT_H_

#include "StdAfx.h"
#include "resource.h"       // main symbols
#include "../SynCommonLib/Clause.h"
#include "Unit.h"



/////////////////////////////////////////////////////////////////////////////
// CCOMClauseVariant
class ATL_NO_VTABLE CCOMClauseVariant : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCOMClauseVariant, &CLSID_ClauseVariant>,
	public IDispatchImpl<IClauseVariant, &IID_IClauseVariant, &LIBID_SYNANLib>
{
public:
DECLARE_REGISTRY_RESOURCEID(IDR_CLAUSEVARIANT)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CCOMClauseVariant)
	COM_INTERFACE_ENTRY(IClauseVariant)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IClauseVariant
public:
	STDMETHOD(get_UnitsCount)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_Group)(long lGroupNum,/*[out, retval]*/ IGroup* *pVal);	
	STDMETHOD(get_GroupsCount)(/*[out, retval]*/ long *pVal);

public:
	STDMETHOD(get_ClauseRootNo)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_Unit)(/*[in]*/long lUnitNum, /*[out, retval]*/ ISyntaxUnit* *pVal);
	STDMETHOD(get_Predicate)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_SubjectsCount)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_Subjects)(/*[in]*/long SubjectNo, /*[out, retval]*/ long *pVal);


	int GetUnitNumByWordNum(int iWordNum) const;
	BOOL InitSynVariant(const CClause* pClause, CSVI pSynVar);
	CSVI	m_pSynVariant;
private:	
	const CClause*	m_pClause;
	SClauseType	m_ClauseTypeOfThisSynVar;

	const	CSyntaxOpt*	GetOptions()  const
	{
			assert (m_pClause);
			assert (m_pClause->m_pSent);
			return  m_pClause->m_pSent->m_pSyntaxOptions;
	};


public:
	STDMETHOD(get_VariantWeight)(int* pVal);
};

#endif //__CLAUSEVARIANT_H_
