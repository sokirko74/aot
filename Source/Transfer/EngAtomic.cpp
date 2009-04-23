#include "StdAfx.h"
#include "EngSemStructure.h"


BYTE GetOnePOS(long poses)
{
	for(int i = 0 ; i < 32 ; i++ )
		if( poses & (1 << i))
			return i;

	return UnknownPOS;
}


void CEngSemWord::SetTense(EngVerbTenseEnum	NewTense, string Maker)
{
	m_Tense = NewTense;
	m_TenseHistory.push_back(CTenseHistory (NewTense, Maker));
};

EngVerbTenseEnum CEngSemWord::GetTense() const
{
 	return m_Tense; 
};
	
void CEngSemWord::Init() 
{
	m_bDoNotChangeForm = false;
	m_Poses = 0;
	m_bMorphologicalPassiveForm = false;
	m_bImperative = false;
	m_OneselfStr = "";
	SetTense(zero_tn, "Constructor");

};
CEngSemWord::CEngSemWord() : CSemWord()	 
{
	Init();	 
};
CEngSemWord::CEngSemWord (const CSemWord& X)
{
	*((CSemWord*)this) = X;
	Init();
};
    
  // принадлежит ли данная часть речи набору частей речи, который приписан слову?
bool   CEngSemWord::HasPOS (size_t POS) const 
{
	return (m_Poses & (1<<POS)) > 0;
};



//======================================================

void CEngSemNode::Init () 
{
	RusNode = -1;
	m_bNotUseTo = false;
	m_ArticleStr = "";
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

bool CEngSemNode::IsLemma(string Lemma) const 
{
	return IsPrimitive() && (m_Words[0].m_Lemma == Lemma);
};


void CEngSemNode::SetGrammemsRich(QWORD g)	
{ 
	if (m_MainWordNo ==-1)
		SetGrammems(g);
	else
		m_Words[m_MainWordNo].SetFormGrammems( g );
};

QWORD CEngSemNode::GetGrammemsRich() const
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
void CEngSemNode::AddGrammemsRich(QWORD  grammems)
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

void CEngSemNode::DeleteGrammemsRich(QWORD g)
{
	if (m_MainWordNo == -1)
		SetGrammems(GetGrammems() & ~g);
	else
		m_Words[m_MainWordNo].SetFormGrammems(m_Words[m_MainWordNo].GetFormGrammems() & ~g);

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
				string SyntacticRelation): CSemRelation(Valency,SourceNodeNo,TargetNodeNo,SyntacticRelation)
{
	Init();
};

CEngSemRelation::CEngSemRelation (const CSemRelation& X)
{
	*((CSemRelation*)this )= X;
	Init();
};
