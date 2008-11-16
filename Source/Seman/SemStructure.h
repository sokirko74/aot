// SemStructure.h : Declaration of the CSemStructure

#ifndef __SEMSTRUCTURE_H_
#define __SEMSTRUCTURE_H_

#include "resource.h"       // main symbols
#include "../SemanLib/SemStructureBuilder.h"

/////////////////////////////////////////////////////////////////////////////
// CSemStructure
class ATL_NO_VTABLE CSemStructure : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSemStructure, &CLSID_SemStructure>,
	public IDispatchImpl<ISemStructure, &IID_ISemStructure, &LIBID_SEMANLib>,
	public CSemStructureBuilder
{
public:
	CSemStructure();
	~CSemStructure();

DECLARE_REGISTRY_RESOURCEID(IDR_SEMSTRUCTURE)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CSemStructure)
	COM_INTERFACE_ENTRY(ISemStructure)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// ISemStructure
public:
	STDMETHOD(InitPresemanDicts)();
	STDMETHOD(InitSemanDicts)();
	STDMETHOD(get_Relations)(/*[in]*/ long pos, /*[out, retval]*/ IComSemRelation* *pVal);
	STDMETHOD(get_RelationsCount)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_DopRelations)(/*[in]*/ long pos, /*[out, retval]*/ IComSemRelation* *pVal);
	STDMETHOD(get_DopRelationsCount)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_Nodes)(/*[in]*/  long pos, /*[out, retval]*/ IComSemNode* *pVal);
	STDMETHOD(get_NodesCount)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_ShouldBuildTclGraph)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(put_ShouldBuildTclGraph)(/*[in]*/ BOOL newVal);
	STDMETHOD(get_ModuleTimeStatistics)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(Stop)();
	STDMETHOD(InitializeIndices)();
	STDMETHOD(FindSituationsForNextSentence)(/*[out, retval]*/ BOOL* Result);
	STDMETHOD(get_LastError)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(BuildSentence)(/*[out,retval]*/ BSTR* Sentence);
	STDMETHOD(SyntRusSentence)(/*[out,retval]*/ BSTR* Sentence);
	
	STDMETHOD(TranslateToEnglish)(BSTR* Graph);
	STDMETHOD(FindSituations)(/*[in]**/BSTR text, /*[in]*/long Position, /*[in]*/BSTR PO, /*[in]*/long PanicTreeVariantCount, /*[in]*/long  ClauseVariantsCombinationNo, BSTR AllowableLexVars, BSTR* Graph);
	STDMETHOD(get_CurrentTime)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_CurrentTime)(/*[in]*/ long newVal);
	STDMETHOD(GetSyntaxTreeByText)(/*[in]*/ BSTR text, int ClauseVarNo,  BSTR* Graph);

	STDMETHOD(get_SilentMode)(BOOL* pVal);
	STDMETHOD(put_SilentMode)(BOOL newVal);
	STDMETHOD(SetLemmasToReplace)(BSTR LemmasToReplace);
	
	STDMETHOD(SaveThisSentence)(void);
	STDMETHOD(ClearSavedSentences)(void);
	STDMETHOD(Answer)(BSTR* sAnswer);
};

#endif //__SEMSTRUCTURE_H_
