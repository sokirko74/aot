// COMClauseType.h : Declaration of the CCOMClauseType

#ifndef __COMCLAUSETYPE_H_
#define __COMCLAUSETYPE_H_

#include "resource.h"       // main symbols
#include "../SynCommonLib/Clause.h"
#include "SynAn.h"

/////////////////////////////////////////////////////////////////////////////
// CCOMClauseType
class ATL_NO_VTABLE CCOMClauseType : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCOMClauseType, &CLSID_ClauseRoot>,
	public IDispatchImpl<IClauseRoot, &IID_IClauseRoot, &LIBID_SYNANLib>
{
public:
	CCOMClauseType()
	{
	}

	~CCOMClauseType()
	{
	}


DECLARE_REGISTRY_RESOURCEID(IDR_COMCLAUSETYPE)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CCOMClauseType)
	COM_INTERFACE_ENTRY(IClauseRoot)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

protected:
	const SClauseType*			m_pType;
	const CSyntaxOpt*		m_pOptions;

// ICOMClauseType
public:
	STDMETHOD(get_Description)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_RootHomonymNo)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_RootWordNo)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_Type)(/*[out, retval]*/ int *pVal);
	BOOL Init(const SClauseType* pType, const CSyntaxOpt*	pOptions)
	{
		if( !pType )
			return FALSE;
		m_pOptions = pOptions;
		m_pType = pType;
		return TRUE;
	}
};

#endif //__COMCLAUSETYPE_H_
