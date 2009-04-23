// ComSemNode.h : Declaration of the CComSemNode

#ifndef __COMSEMNODE_H_
#define __COMSEMNODE_H_

#include "resource.h"       // main symbols
#include "../SemanLib/SemanticRusStructure.h"
/////////////////////////////////////////////////////////////////////////////
// CComSemNode
inline idlDictTypeEnum InternalDictTypeToIdl(DictTypeEnum type)
{
	return (idlDictTypeEnum)type;
};

inline DictTypeEnum IdlDictTypeToInternal(idlDictTypeEnum type)
{
	return (DictTypeEnum)type;
};


class ATL_NO_VTABLE CComSemNode : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CComSemNode, &CLSID_ComSemNode>,
	public IDispatchImpl<IComSemNode, &IID_IComSemNode, &LIBID_SEMANLib>
{
public:
	const CSemNode* m_pNode;
	const CRusSemStructure* m_pRusStr;
	CComSemNode()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_COMSEMNODE)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CComSemNode)
	COM_INTERFACE_ENTRY(IComSemNode)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IComSemNode
public:
	STDMETHOD(get_ClauseNo)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_Grammems)(/*[out, retval]*/ hyper *pVal);
	STDMETHOD(get_GramCodes)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_Words)(/*[in]*/ long pos, /*[out, retval]*/ IComSemWord* *pVal);
	STDMETHOD(get_WordsCount)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_Poses)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_UnitNo)(/*[out, retval]*/ WORD *pVal);
	STDMETHOD(get_DictType)(/*[out, retval]*/ idlDictTypeEnum *pVal);
	STDMETHOD(get_RelOperatorsCount)(int* pVal);
	STDMETHOD(get_RelOperators)(int Index, BSTR* pVal);
	STDMETHOD(get_Prep)(BSTR* pVal);
};

#endif //__COMSEMNODE_H_
