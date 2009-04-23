// COMSentence.h : Declaration of the CCOMSentence

#ifndef __SENTENCE_H_
#define __SENTENCE_H_


#include "StdAfx.h"


#include "resource.h"       // main symbols
#include "COMClause.h"
#include "ComRelationIterator.h"
#include "../SynCommonLib/Word.h"
#include "../SynCommonLib/Sentence.h"

#define SENT_ERROR -2

/////////////////////////////////////////////////////////////////////////////
// CCOMSentence
class ATL_NO_VTABLE CCOMSentence : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCOMSentence, &CLSID_Sentence>,
	public IDispatchImpl<ISentence, &IID_ISentence, &LIBID_SYNANLib>
{
public:
	CSentence*	m_pSentence;	


DECLARE_REGISTRY_RESOURCEID(IDR_SENTENCE)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CCOMSentence)
	COM_INTERFACE_ENTRY(ISentence)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// ISentence
public:
	STDMETHOD(GetOborotStrByOborotId)(/*[in]*/ long OborotId, /*[out, retval]*/ BSTR* pVal);
	STDMETHOD(get_PrimitiveClause)(long lClauseNum, IClause* *pVal);
	STDMETHOD(get_PrimitiveClausesCount)(long *pVal);
	STDMETHOD(PrimitiveClauseNoByClauseNo)(/*[in]*/long lClauseNo, /*[out, retval]*/long* lPrClauseNo);
	STDMETHOD(ClauseNoByPrimitiveClauseNo)(/*[in]*/long lPrClauseNo, /*[out, retval]*/long* lClauseNo);
	STDMETHOD(GetHomonymByClauseVar)(/*[in]*/long iWord, /*[in]*/long iVar, long iClause, /*[out, retval]*/IHomonym** piHom);
	STDMETHOD(CreateRelationsIterator)(/*[out, retval]*/ IRelationsIterator** piRelsIt);
	STDMETHOD(get_Clause)(long lClauseNum, IClause* *pVal);
	STDMETHOD(get_ClausesCount)(long *pVal);
	STDMETHOD(get_Word)(long lWordNum, IWord* *pVal);
	STDMETHOD(get_WordsNum)(long *pVal);



};

#endif //__SENTENCE_H_
