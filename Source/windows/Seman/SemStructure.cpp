// SemStructure.cpp : Implementation of CSemStructure
#include "StdAfx.h"
#include "seman_i.h"
#include <comdef.h>
#include "ComSemNode.h"
#include "ComSemRelation.h"
#include "SemStructure.h"

CSemStructure::CSemStructure() 
{
	m_CurrentSentence = 0;
	m_bShouldBuildTclGraph = true;
}

CSemStructure::~CSemStructure()
{
};

STDMETHODIMP CSemStructure::InitPresemanDicts()
{
	if (!m_RusStr.m_pData->Init()) 
		return E_FAIL; 
	return S_OK;
}

STDMETHODIMP CSemStructure::InitSemanDicts()	
{
	m_RusStr.m_pData->m_bSilentMode = true;
	try {
	   m_RusStr.m_pData->Initialize();
	}
    catch (...)
	{
		return E_FAIL;
	};

	return S_OK;
}

STDMETHODIMP CSemStructure::GetSyntaxTreeByText(BSTR text, int ClauseVarNo, BSTR* Graph)
{
	try {
		std::string t = text ? (const char*)_bstr_t(text) : "";
		t = convert_from_utf8(t.c_str(), morphRussian);
		std::string ResultGraph;
		if (!m_RusStr.GetSyntaxTreeByText(t, ClauseVarNo, ResultGraph))
			return S_FALSE;

		auto s = ResultGraph;
		s = convert_to_utf8(s.c_str(), morphRussian);
		*Graph = _bstr_t(s.c_str()).copy();
		return S_OK;
	}
	catch (...)
	{
		return S_FALSE;
	};
}

STDMETHODIMP CSemStructure::get_CurrentTime(long *pVal)
{
	*pVal = m_RusStr.m_pData->m_LastUpdateTime;
	return S_OK;
}

STDMETHODIMP CSemStructure::put_CurrentTime(long newVal)
{
	m_RusStr.m_pData->m_LastUpdateTime = newVal;
	return S_OK;
}


STDMETHODIMP CSemStructure::FindSituationsForNextSentence(BOOL* Result)
{
	if (!CSemStructureBuilder::FindSituationsForNextSentence())
		*Result = FALSE;
	else
		*Result = TRUE;
	return S_OK;
}

STDMETHODIMP CSemStructure::FindSituations(BSTR utf8text, long UserTreeVariantNo, BSTR PO, long PanicTreeVariantCount, long UserClauseVariantsCombinationNo, BSTR AllowableLexVars, BSTR* Graph)
{
	std::string ResultGraph;

	std::string sText				= utf8text ? (const char*)_bstr_t(utf8text) : "";
	std::string sPO					= PO ? (const char*)_bstr_t(PO) : "";
	sPO = convert_from_utf8(sPO.c_str(), morphRussian);
	std::string sAllowableLexVars	= AllowableLexVars? (const char*)_bstr_t(AllowableLexVars) : "";
	
	bool bRes = CSemStructureBuilder::FindSituations
		(
			sText,
			UserTreeVariantNo, 
			sPO,
			PanicTreeVariantCount, 
			UserClauseVariantsCombinationNo, 
			sAllowableLexVars,
			ResultGraph
		);
	if (!bRes)
		return E_FAIL;
	ResultGraph = convert_to_utf8(ResultGraph, morphRussian);
	*Graph = _bstr_t(ResultGraph.c_str()).copy();
	return S_OK;

}


STDMETHODIMP CSemStructure::TranslateToEnglish(BSTR* Graph)
{
	std::string ResultGraph;
	bool bRes = CSemStructureBuilder::TranslateToEnglish(ResultGraph);
	if (!bRes)
		return E_FAIL;
	ResultGraph = convert_to_utf8(ResultGraph, morphRussian);
	*Graph = _bstr_t(ResultGraph.c_str()).copy();
	return S_OK;
}


STDMETHODIMP CSemStructure::BuildSentence(BSTR* Sentence)
{
	std::string ResultSentence;
	bool bRes = CSemStructureBuilder::BuildSentence(ResultSentence);
	if (!bRes)
		return E_FAIL;
	ResultSentence = convert_to_utf8(ResultSentence, morphRussian);
	*Sentence = _bstr_t(ResultSentence.c_str()).copy();
	return S_OK;
}

STDMETHODIMP CSemStructure::SyntRusSentence(BSTR* Sentence)
{
	std::string ResultSentence;
	bool bRes = CSemStructureBuilder::SyntRusSentence(ResultSentence);
	if (!bRes)
		return E_FAIL;
	ResultSentence = convert_to_utf8(ResultSentence, morphRussian);
	*Sentence = _bstr_t(ResultSentence.c_str()).copy();
	return S_OK;
}

STDMETHODIMP CSemStructure::get_LastError(BSTR *pVal)
{
	_bstr_t t = m_RusStr.m_pData->m_LastError.c_str();
	*pVal = t.copy();
	return S_OK;
}


STDMETHODIMP CSemStructure::InitializeIndices()
{
    m_RusStr.m_pData->InitializeIndices();
	return S_OK;
}


STDMETHODIMP CSemStructure::Stop()
{
	m_RusStr.m_bShouldBeStopped = true;
	return S_OK;
}

STDMETHODIMP CSemStructure::get_ModuleTimeStatistics(BSTR *pVal)
{
	*pVal = _bstr_t(m_GlobalSpan.GetStrRepresentation().c_str()).copy();
	return S_OK;
}

STDMETHODIMP CSemStructure::get_ShouldBuildTclGraph(BOOL *pVal)
{
    *pVal = m_bShouldBuildTclGraph ? TRUE : FALSE;
	return S_OK;
}

STDMETHODIMP CSemStructure::put_ShouldBuildTclGraph(BOOL newVal)
{
	m_bShouldBuildTclGraph = newVal ? true : false;
	return S_OK;
}



STDMETHODIMP CSemStructure::get_NodesCount(long *pVal)
{
	*pVal = m_RusStr.m_Nodes.size();
	return S_OK;
}

STDMETHODIMP CSemStructure::get_Nodes(long pos, IComSemNode **pVal)
{
	CComObject<CComSemNode>* p = new  CComObject<CComSemNode>;
	p->AddRef();
	p->m_pNode = &(m_RusStr.m_Nodes[pos]);
	p->m_pRusStr = &m_RusStr;
	*pVal = p;
	return S_OK;
}

STDMETHODIMP CSemStructure::get_RelationsCount(long *pVal)
{
	*pVal = m_RusStr.m_Relations.size();
	return S_OK;
}

STDMETHODIMP CSemStructure::get_Relations(long pos, IComSemRelation **pVal)
{
	CComObject<CComSemRelation>* p = new  CComObject<CComSemRelation>;
	p->AddRef();
	p->m_pRelation = &(m_RusStr.m_Relations[pos]);
	p->m_pRusStr = &m_RusStr;
	*pVal = p;
	return S_OK;
}


STDMETHODIMP CSemStructure::get_DopRelationsCount(long *pVal)
{
	*pVal = m_RusStr.GetDopRelationsSize();
	return S_OK;
}

STDMETHODIMP CSemStructure::get_DopRelations(long pos, IComSemRelation **pVal)
{
	CComObject<CComSemRelation>* p = new  CComObject<CComSemRelation>;
	p->AddRef();
	p->m_pRelation = m_RusStr.GetDopRelation(pos);
	p->m_pRusStr = &m_RusStr;
	*pVal = p;
	return S_OK;
}

STDMETHODIMP CSemStructure::get_SilentMode(BOOL* pVal)
{
	*pVal = (m_RusStr.m_pData->m_bSilentMode  ? TRUE : FALSE);

	return S_OK;
}

STDMETHODIMP CSemStructure::put_SilentMode(BOOL newVal)
{
	m_RusStr.m_pData->m_bSilentMode  = newVal== TRUE; 

	return S_OK;
}

STDMETHODIMP CSemStructure::SetLemmasToReplace(BSTR LemmasToReplace)
{
	std::string s = LemmasToReplace ? (const char*)_bstr_t(LemmasToReplace) : "";
	s = convert_from_utf8(s.c_str(), morphRussian);
	if (!m_RusStr.SetLemmasToReplace(s))
		return E_FAIL;
	else
		return S_OK;
};

STDMETHODIMP CSemStructure::SaveThisSentence(void)
{
	CSemStructureBuilder::SaveThisSentence();
	return S_OK;
}

STDMETHODIMP CSemStructure::ClearSavedSentences(void)
{
	CSemStructureBuilder::ClearSavedSentences();
	return S_OK;
}

STDMETHODIMP CSemStructure::Answer(BSTR* sAnswer)
{
	std::string result = CSemStructureBuilder::Answer();
	result = convert_to_utf8(result, morphRussian);
	*sAnswer = _bstr_t(result.c_str()).copy();
	return S_OK;
}
