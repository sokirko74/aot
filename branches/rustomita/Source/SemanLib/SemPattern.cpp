#include "stdafx.h"
#include  "SemPattern.h"
#include "SemanticStructure.h"

CSemPattern::CSemPattern ()
{
		m_Direction ="";
		SetEmpty();
};

const CRossHolder*	CSemPattern::GetRossHolder()  const
{
	return (CRossHolder*)m_pRossDoc;
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

bool		CSemPattern::IsEmpty() const 
{
	return m_GramCorteges.size() + m_LexFets.size() + m_ObligLexFets.size() == 0;
};

long CSemPattern::GetSynRel(long CortegeNo) const
{
	return m_pRossDoc->GetSynRel(m_GramCorteges[CortegeNo]); 
};

void CSemPattern::InitSemPattern(const CRossHolder* pRossDoc, WORD UnitNo, BYTE LeafId, BYTE BracketLeafId)
{
	m_PatternValency.m_LeafId =  LeafId;
	m_PatternValency.m_BracketLeafId = BracketLeafId;
	m_PatternValency.m_UnitNo = UnitNo;
	m_PatternValency.m_RossHolder = pRossDoc;
	m_pRossDoc = pRossDoc;
};

long CSemPattern::GetSynFet(long CortegeNo) const
{
	return m_pRossDoc->GetSynFet(m_GramCorteges[CortegeNo]); 
};

string  CSemPattern::GetSynFetStr(long CortegeNo) const
{
	const char* s =  m_pRossDoc->GetDomItemStrInner(GetSynFet(CortegeNo));
	if (s) return s;
	return "";
};

string  CSemPattern::GetSynRelStr(long CortegeNo) const
{
	const char* s =  m_pRossDoc->GetDomItemStrInner(GetSynRel(CortegeNo));
	if (s) return s;
	return "";
};

bool  CSemPattern::HasSemFet(const string& SemFet) const
{
	for (long i=0; i < m_ActantSemFets.size(); i++)
		for (long k=0; k < m_ActantSemFets[i].size(); k++)
			if (SemFet == m_ActantSemFets[i][k])
				return true;
		
	return false;
};

void  CSemPattern::InsertReverseSynOToTheBegining() 
{
	for (long i=0; i < m_GramCorteges.size(); i++)
	{
		for (long k=9; k > 0; k--)
			m_GramCorteges[i].m_DomItemNos[k] =  m_GramCorteges[i].m_DomItemNos[k-1];
		m_GramCorteges[i].m_DomItemNos[0] =  m_pRossDoc->ReverseSynONo;

	};
		
};

void CSemPattern::TracePattern () const
{
	string Q = Format ("%s\n",m_PatternValency.m_RelationStr.c_str());
	rml_TRACE (Q.c_str());
	for (size_t i=0; i< m_GramCorteges.size(); i++)
	{
		Q = "";
		for (size_t k=0; k<3; k++)
			if (m_GramCorteges[i].m_DomItemNos[k] != -1)
			{
				string S (m_pRossDoc->GetDomItemStrInner(m_GramCorteges[i].m_DomItemNos[k]));;
				Q += string (" ") + S;
			};
	     Q += string ("\n");
		 rml_TRACE (Q.c_str());
	};
};

bool CSemPattern::LoadSemFromDict()
{
	size_t ActantsCount = 0;
	bool IsFound = false;

	assert (m_PatternValency.m_RossHolder != 0);
	const CDictionary* Ross = m_PatternValency.m_RossHolder->GetRoss();
	m_ActantSemFets.clear ();
	WORD UnitNo = m_PatternValency.m_UnitNo;
	//assert  (UnitNo !=  ErrUnitNo);
	if (UnitNo ==  ErrUnitNo) 
		return false;

	if (Ross->IsEmptyArticle(UnitNo))	return false;
	//rml_TRACE  ("Load pattern of %s\n", (const char*) Ross->GetEntryStr(UnitNo));
	long EnfCortegeNo = Ross->GetUnitEndPos(UnitNo);

	for (size_t i = Ross->GetUnitStartPos(UnitNo); i<= EnfCortegeNo; i++)
	{
		TCortege C = GetCortege(Ross,i);

		if (     (C.m_FieldNo == m_PatternValency.m_RossHolder->SemFetFieldNo) 
			&& (C.m_LeafId == m_PatternValency.m_LeafId) 
			&& (C.m_BracketLeafId == m_PatternValency.m_BracketLeafId) 
			&& (C.m_LevelId > 0)
			)
		{
			if (C.m_LevelId  > m_ActantSemFets.size())   
				m_ActantSemFets.push_back(vector<string>());

			string s = m_PatternValency.m_RossHolder->GetDomItemStrInner(C.m_DomItemNos[0]);
			if (C.m_LevelId - 1 >= m_ActantSemFets.size())
			{
				string mess = "Ошибка нумерации SF в статье ";
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
	WORD UnitNo = m_PatternValency.m_UnitNo;
	assert  (UnitNo !=  ErrUnitNo);
	if (UnitNo ==  ErrUnitNo) 
		return false;
	

	if (Ross->IsEmptyArticle(UnitNo))	return false;
	//rml_TRACE  ("Load pattern of %s\n", (const char*) Ross->GetEntryStr(UnitNo));
	long EnfCortegeNo = Ross->GetUnitEndPos(UnitNo);


	for (size_t i = Ross->GetUnitStartPos(UnitNo); i<= EnfCortegeNo; i++)
	{
		TCortege C = GetCortege(Ross,i);
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
			string S =   m_PatternValency.m_RossHolder->GetDomItemStrInner(C.m_DomItemNos[0]);
			EngRusMakeUpper(S);
			m_LexFets.push_back(S);
			IsFound = true;
		}
		else
		if (C.m_FieldNo == m_PatternValency.m_RossHolder->ObligLexFetFieldNo) 
		{
			string S =   m_PatternValency.m_RossHolder->GetDomItemStrInner(C.m_DomItemNos[0]);
			EngRusMakeUpper(S);
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

