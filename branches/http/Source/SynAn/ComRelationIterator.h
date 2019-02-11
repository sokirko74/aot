// RelationsIterator.h : Declaration of the CCOMRelationsIterator

#ifndef __COM_RELATIONSITERATOR_H_
#define __COM_RELATIONSITERATOR_H_

#include "resource.h"       // main symbols
#include "ComRelation.h"
#include "../SynCommonLib/Sentence.h"
#include "../SynCommonLib/RelationsIterator.h"


// RelationsIterator.h : Declaration of the CRelationsIterator


/////////////////////////////////////////////////////////////////////////////
// CRelationsIterator
class ATL_NO_VTABLE CCOMRelationsIterator : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCOMRelationsIterator, &CLSID_RelationsIterator>,
	public IDispatchImpl<IRelationsIterator, &IID_IRelationsIterator, &LIBID_SYNANLib>,
	public CRelationsIterator

{
public:


DECLARE_REGISTRY_RESOURCEID(IDR_RELATIONSITERATOR)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CCOMRelationsIterator)
	COM_INTERFACE_ENTRY(IRelationsIterator)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IRelationsIterator
public:
	STDMETHOD(Reset)();
	STDMETHOD(get_FirmGroup)(/*[in]*/long iGroup, /*[out, retval]*/ IGroup* *pVal);
	STDMETHOD(get_FirmGroupsCount)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_Relation)(/*[in]*/ long iRel, /*[out, retval]*/ IRelation* *pVal);
	STDMETHOD(BuildRelations)();
	STDMETHOD(get_RelationsCount)(/*[out, retval]*/ long *pVal);
	STDMETHOD(AddClauseNoAndVariantNo)(/*[in]*/long iClauseNo, long iVarNo);

	BOOL Init(const CSentence* pSent)
	{
		m_pSent = pSent;
		if( !m_pSent )
			return FALSE;
		return TRUE;
	}

};

#endif //__RELATIONSITERATOR_H_
