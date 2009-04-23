// ComRelation.h : Declaration of the CCOMRelation

#ifndef __COM_RELATION_H_
#define __COM_RELATION_H_

#include "resource.h"       
#include "ComGroup.h"
#include "SynAn.h"
#include "../SynCommonLib/RelationsIterator.h"

/////////////////////////////////////////////////////////////////////////////
// CCOMRelation
class ATL_NO_VTABLE CCOMRelation : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCOMRelation, &CLSID_Relation>,
	public IDispatchImpl<IRelation, &IID_IRelation, &LIBID_SYNANLib>,
	public CSynOutputRelation
{
public:


DECLARE_REGISTRY_RESOURCEID(IDR_RELATION)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CCOMRelation)
	COM_INTERFACE_ENTRY(IRelation)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IRelation
public:
	STDMETHOD(get_Name)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_SourceItemType)(int* type);
	STDMETHOD(get_TargetItemType)( int* type);
	STDMETHOD(get_TargetItemNo)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_SourceItemNo)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_TargetClauseNo)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_SourceClauseNo)(/*[out, retval]*/ long *pVal);

};

#endif //__RELATION_H_
