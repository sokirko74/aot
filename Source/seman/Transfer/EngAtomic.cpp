#include "StdAfx.h"
#include "EngSemStructure.h"


part_of_speech_t GetOnePOS(part_of_speech_mask_t poses)
{
	for(int i = 0 ; i < 32 ; i++ )
		if( poses & (1 << i))
			return i;

	return UnknownPOS;
}


//======================================================

void CEngSemNode::Init () 
{
	RusNode = -1;
	m_bNotUseTo = false;
	m_Article = UnknownArticle;
};

CEngSemNode::CEngSemNode() : CSemNode() 
{
	Init(); 
};

CEngSemNode::CEngSemNode (const CSemNode& X)
{
	*((CSemNode*)this )= X;
	for( int i=0; i < X.GetWordsSize();i++ )
	{
		CEngSemWord W (X.GetWord(i));
		m_Words.push_back(W);
	}
	Init();
};

EngVerbTenseEnum CEngSemNode::GetTense() const
{
	if (m_MainWordNo != -1)
		return m_Words[m_MainWordNo].GetTense();
	else
		return zero_tn;
};

BYTE CEngSemNode::GetPos() const
{
	if (m_MainWordNo != -1)
		return GetOnePOS(m_Words[m_MainWordNo].m_Poses);
	else // надеемся, что частей речи в англйискои грамтабе меньше 31
		return UnknownPOS;
};

bool CEngSemNode::IsLemma(std::string Lemma) const 
{
	return IsPrimitive() && (m_Words[0].m_Lemma == Lemma);
};


void CEngSemNode::SetGrammemsRich(uint64_t g)	
{ 
	if (m_MainWordNo ==-1)
		SetGrammems(g);
	else
		m_Words[m_MainWordNo].SetFormGrammems( g );
};

uint64_t CEngSemNode::GetGrammemsRich() const
{ 
	return m_MainWordNo ==-1 ? GetGrammems() : m_Words[m_MainWordNo].GetAllGrammems();
};

void CEngSemNode::AddOneGrammemRich(int g)
{
	if (m_MainWordNo == -1)
		AddOneGrammem(g);
	else
		m_Words[m_MainWordNo].SetFormGrammems ( m_Words[m_MainWordNo].GetFormGrammems() | _QM(g));

};
void CEngSemNode::AddGrammemsRich(uint64_t  grammems)
{
	SetGrammemsRich(GetGrammemsRich() | grammems);
};

bool CEngSemNode::HasGrammemRich(int g) const
{
	if (m_MainWordNo == -1)
		return HasOneGrammem(g);
	else
		return m_Words[m_MainWordNo].HasOneGrammem(g);

};

void CEngSemNode::DeleteGrammemsRich(uint64_t g)
{
	if (m_MainWordNo == -1)
		SetGrammems(GetGrammems() & ~g);
	else
		m_Words[m_MainWordNo].SetFormGrammems(m_Words[m_MainWordNo].GetFormGrammems() & ~g);

};

void CEngSemNode::SetArticle(ArticleEnum s, ArticleCauseEnum cause) {
	m_Article = s;
	m_ArticleCauseHistory.push_back(cause);
}

ArticleEnum CEngSemNode::GetArticle() const {
	return m_Article;
}

ArticleCauseEnum  CEngSemNode::GetLastArticleCause() const {
	if (m_ArticleCauseHistory.empty()) {
		return UnknownArticleCause;
	}
	return m_ArticleCauseHistory.back();
}

static std::string GetStringByArticleCause(ArticleCauseEnum t)
{
	switch (t) {
	case ArticleFromDict: return "an article from dictionary";
	case ArticleFromOrdNum: return "definite article because of ordinal number";
	case ZeroArticleForProperNames: return "zero article for proper names";
	case ZeroArticleBecauseOfPossessive: return "zero article because of possessive attribute";
	case DefArticleBeforeClausePredicate: return "definite article before clause predicate (topic)";
	case DefArticleBecauseOfNominalSupplement: return "definite article because of nominal supplement";
	case DefArticleForAbstractLocal: return "definite article for abstract local nouns";
	case NoIndefArticleForMassNouns: return "no indefinite article for mass nouns";
	case IndefArticleAfterAs: return "indefinite article after <as> ";
	case DefArticleBecauseDefiniteClause: return "definite article because of definite clauses";
	case DefArticleForSingleRanks: return "definite article for single <rank>-nouns";
	case OverwriteArticleForTimeNodes: return "undo article field because it is a time node ";
	case OverwriteArticleForTerminNodes: return "undo article field because of change of termin syntax";

	};
	return "unk cause!";
};


std::string CEngSemNode::GetArticleCauseHistory() const
{
	std::string Result;
	for (auto a: m_ArticleCauseHistory)
		Result += "       " + GetStringByArticleCause(a) + "\n";
	return Result;
};


//=======================================================
void CEngSemRelation::Init()
{
	m_bInterpreted = false;
	m_RusRel = -1;
	m_Type = NotBetweenClauses;
	m_PatternNoInArticle = -1;
	m_bSourceClauseIsMain = true;
	m_GramType = UnknownGram;
	m_iHoleNum = -1;
	m_bAuxFlag = false;
};

CEngSemRelation::CEngSemRelation (CValency Valency, 
	            long SourceNodeNo, 
				long TargetNodeNo, 
				std::string SyntacticRelation): CSemRelation(Valency,SourceNodeNo,TargetNodeNo,SyntacticRelation)
{
	Init();
};

CEngSemRelation::CEngSemRelation (const CSemRelation& X)
{
	*((CSemRelation*)this )= X;
	Init();
};

