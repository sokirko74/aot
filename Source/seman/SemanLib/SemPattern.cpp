#include "stdafx.h"
#include  "SemPattern.h"
#include "SemanticStructure.h"

CSemPattern::CSemPattern ()
{
	m_Direction ="";
	SetEmpty();
};

const CStructDictHolder*	CSemPattern::GetRossHolder()  const
{
	return (CStructDictHolder*)m_pRossDoc;
};

void		CSemPattern::SetEmpty() 
{
	m_GramCorteges.clear();
	m_LexFets.clear();
	m_ObligLexFets.clear();
	m_ActantSemFets.clear();
	m_ArticleCorteges.clear();
	m_PatternValency.m_RossHolder = 0;
};

bool CSemPattern::IsEmpty() const 
{
	return m_GramCorteges.size() + m_LexFets.size() + m_ObligLexFets.size() == 0;
};

const std::vector<TCortege>& CSemPattern::GetGramCorteges() const {
	return m_GramCorteges;
}

void CSemPattern::AddGramCortege(const TCortege& c) {
	m_GramCorteges.push_back(c);
}

void CSemPattern::CopyGramCorteges(const CSemPattern& from) {
	m_GramCorteges = from.GetGramCorteges();
}

void CSemPattern::DeleteGramCortege(size_t index) {
	m_GramCorteges.erase(m_GramCorteges.begin() + index);
}

void CSemPattern::DeleteAllGramCortege() {
	m_GramCorteges.clear();
}

dom_item_id_t CSemPattern::GetSynRel(long cortege_no) const
{
	return m_pRossDoc->GetSynRel(m_GramCorteges[cortege_no]);
};

void CSemPattern::InitSemPattern(const CStructDictHolder* pRossDoc, uint16_t UnitNo, BYTE LeafId, BYTE BracketLeafId)
{
	m_PatternValency.m_LeafId =  LeafId;
	m_PatternValency.m_BracketLeafId = BracketLeafId;
	m_PatternValency.m_UnitNo = UnitNo;
	m_PatternValency.m_RossHolder = pRossDoc;
	m_pRossDoc = pRossDoc;
};

dom_item_id_t CSemPattern::GetSynFet(long CortegeNo) const
{
	return m_pRossDoc->GetSynFet(m_GramCorteges[CortegeNo]); 
};

std::string  CSemPattern::GetSynFetStr(long cortege_no) const
{
	return  m_pRossDoc->GetDomItemStrWrapper(GetSynFet(cortege_no));
};

std::string  CSemPattern::GetSynRelStr(long cortege_no) const
{
	return  m_pRossDoc->GetDomItemStrWrapper(GetSynRel(cortege_no));
};


bool  CSemPattern::HasSemFet(const std::string& SemFet) const
{
	for (long i=0; i < m_ActantSemFets.size(); i++)
		for (long k=0; k < m_ActantSemFets[i].size(); k++)
			if (SemFet == m_ActantSemFets[i][k])
				return true;
		
	return false;
};

void CSemPattern::TracePattern () const
{
	LOGV << m_PatternValency.m_RelationStr;
	for (auto& g: m_GramCorteges)
	{
		LOGV << m_pRossDoc->GetRoss()->WriteToString(g);
	};
};

bool CSemPattern::LoadSemFromDict()
{
	size_t ActantsCount = 0;
	bool IsFound = false;

	assert (m_PatternValency.m_RossHolder != 0);
	const CDictionary* Ross = m_PatternValency.m_RossHolder->GetRoss();
	m_ActantSemFets.clear ();
	uint16_t UnitNo = m_PatternValency.m_UnitNo;
	//assert  (UnitNo !=  ErrUnitNo);
	if (UnitNo ==  ErrUnitNo) 
		return false;

	if (Ross->IsEmptyArticle(UnitNo))	return false;
	long EnfCortegeNo = Ross->GetUnitLastPos(UnitNo);

	for (size_t i = Ross->GetUnitStartPos(UnitNo); i<= EnfCortegeNo; i++)
	{
		TCortege C = m_PatternValency.m_RossHolder->GetCortegeCopy(i);

		if (     (C.m_FieldNo == m_PatternValency.m_RossHolder->SemFetFieldNo) 
			&& (C.m_LeafId == m_PatternValency.m_LeafId) 
			&& (C.m_BracketLeafId == m_PatternValency.m_BracketLeafId) 
			&& (C.m_LevelId > 0)
			)
		{
			if (C.m_LevelId  > m_ActantSemFets.size())   
				m_ActantSemFets.push_back(std::vector<std::string>());

			std::string s = m_PatternValency.m_RossHolder->GetDomItemStrWrapper(C.GetItem(0));
			if (C.m_LevelId - 1 >= m_ActantSemFets.size())
			{
				std::string mess = "Bad field SF";
				mess += Ross->GetEntryStr(UnitNo);
				ErrorMessage (mess);
			}
			else
			{
				m_ActantSemFets[C.m_LevelId - 1].push_back (s );
				IsFound = true;
			};

		}
	};
	return IsFound;
};


//  this function can be called more than once  for one instance of  CSemPattern
//  that's why we do not clear it at the beginning
bool CSemPattern::LoadGramFromDict()
{
	size_t ActantsCount = 0;
	bool IsFound = false;

	assert (m_PatternValency.m_RossHolder != 0);
	const CDictionary* Ross = m_PatternValency.m_RossHolder->GetRoss();
	uint16_t UnitNo = m_PatternValency.m_UnitNo;
	assert  (UnitNo !=  ErrUnitNo);
	if (UnitNo ==  ErrUnitNo) 
		return false;
	

	if (Ross->IsEmptyArticle(UnitNo))	return false;
	long EnfCortegeNo = Ross->GetUnitLastPos(UnitNo);


	for (size_t i = Ross->GetUnitStartPos(UnitNo); i<= EnfCortegeNo; i++)
	{
		TCortege C = m_PatternValency.m_RossHolder->GetCortegeCopy(i);
		if	(		(C.m_LeafId != m_PatternValency.m_LeafId) 
				||	(C.m_BracketLeafId != m_PatternValency.m_BracketLeafId) 
			)
		continue;

		if (C.m_FieldNo == m_PatternValency.m_RossHolder->GramFetFieldNo) 
		{

			m_GramCorteges.push_back(C);
			IsFound = true;
		}
		else
		if (C.m_FieldNo == m_PatternValency.m_RossHolder->LexFetFieldNo) 
		{
			std::string S =   m_PatternValency.m_RossHolder->GetDomItemStrWrapper(C.GetItem(0));
			MakeUpperUtf8(S);
			m_LexFets.push_back(S);
			IsFound = true;
		}
		else
		if (C.m_FieldNo == m_PatternValency.m_RossHolder->ObligLexFetFieldNo) 
		{
			std::string S =   m_PatternValency.m_RossHolder->GetDomItemStrWrapper(C.GetItem(0));
			MakeUpperUtf8(S);
			m_ObligLexFets.push_back(S);
			IsFound = true;
		}
		else
		if (C.m_FieldNo == m_PatternValency.m_RossHolder->ArticleFieldNo) 
		{
			m_ArticleCorteges.push_back(C);
		}
	};

	return IsFound;
};

