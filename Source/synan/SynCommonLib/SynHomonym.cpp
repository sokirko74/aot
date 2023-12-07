#include "SynHomonym.h"
#include "Sentence.h"


CSynHomonym::CSynHomonym(MorphLanguageEnum l) : CHomonym(l) {
	ResetSelfMembers();
}

void CSynHomonym::ResetSelfMembers() {
	m_CoordConjNo = -1;
	m_bAdvAdj = false;
	m_bGoodHomonym = false;
	m_bCanSynDependOnAdj = false;
	m_bCanSynDependOnAdv = false;
	m_bCanSubdueInfinitive = false;
	m_bCanSubdueInstr = false;
	m_bNounHasAdjectiveDeclination = false;
	m_bMonth = false;
	m_bPassive = false;
	m_bPerfectAnomalie = false;
	m_bInfinitiveConstruction = false;
	m_bAdjWithActiveValency = false;
	
	// function scope members
	m_bDelete = false;
}

CSynHomonym::CSynHomonym(const CHomonym& h) : CHomonym(h) {
	ResetSelfMembers();
}

void CSynHomonym::SetSentence(const CSentence* s) {
	m_pSent = s;
}

const	CSyntaxOpt* CSynHomonym::GetOpt() const
{
	return m_pSent->GetOpt();
};

const COborotForSyntax* CSynHomonym::GetOborotPtr()  const
{
	assert(m_OborotNo != -1);
	return &GetOpt()->GetOborDic()->m_Entries[m_OborotNo];
};

bool CSynHomonym::CompareWithPredefinedWords(const CLemmaList& ll) const
{
	return ll.has_lemma_with_poses(m_iPoses, GetLemma());

}
