// ComSemRelation.h : Declaration of the CComSemRelation

#ifndef __COMSEMRELATION_H_
#define __COMSEMRELATION_H_

#include "resource.h"       // main symbols
#include "../SemanLib/SemanticRusStructure.h"

/////////////////////////////////////////////////////////////////////////////
// CComSemRelation
class ATL_NO_VTABLE CComSemRelation : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CComSemRelation, &CLSID_ComSemRelation>,
	public IDispatchImpl<IComSemRelation, &IID_IComSemRelation, &LIBID_SEMANLib>
{
	
public:
	const CRusSemStructure* m_pRusStr;
	const CSemRelation* m_pRelation;
	CComSemRelation();

DECLARE_REGISTRY_RESOURCEID(IDR_COMSEMRELATION)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CComSemRelation)
	COM_INTERFACE_ENTRY(IComSemRelation)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IComSemRelation
public:
	STDMETHOD(get_Name)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_TargetNodeNo)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_SourceNodeNo)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_WordRealization)(BSTR* pVal);
};

#endif //__COMSEMRELATION_H_
