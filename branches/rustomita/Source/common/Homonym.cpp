#include "LemWord.h"
#include "Homonym.h"
#include "util_classes.h"
#include "../AgramtabLib/agramtab_.h"
#include "../LemmatizerLib/Lemmatizers.h"

CHomonym::CHomonym(const	CAgramtab* pGramTab):CAncodePattern(pGramTab)
{
	m_iCmpnLen = 0;
	m_bCmplLem = false;
	m_lPradigmID = -1;
	m_bDelete = false;
	m_bRussianOdin = false;

	m_bOborot1 = false;
	m_bOborot2 = false;
	m_bInOb = false;
	m_OborotNo = -1;

	m_lFreqHom = 0;
}


void	CHomonym::SetLemma(string Lemma)
{
	GerMakeUpper(Lemma);
	m_strLemma = Lemma;
};



bool	CHomonym::HasSetOfGrammemsExact(QWORD Grammems) const
{
	for (int i = 0; i < m_GramCodes.length(); i += 2)
	{
		QWORD g;
		if (!GetGramTab()->GetGrammems(m_GramCodes.c_str()+i, g))
		{
				assert (false);
		};
		if ((g & Grammems) ==  Grammems)
			return true;

	};

	return false;;
};



bool CHomonym::IsOb1() const
{ 
	return m_bOborot1;	
}

bool CHomonym::IsOb2() const
{ 
	return m_bOborot2;	
}

bool CHomonym::IsIsOb() const
{
	return m_bInOb;	
}


bool CHomonym::IsLemma(const char* lemma) const 
{
	if (lemma == 0) return false;
	return m_strLemma == lemma;

};

bool CHomonym::IsSynNoun() const
{
	return GetGramTab()->IsSynNoun(m_iPoses, m_strLemma.c_str() );
};

bool  CHomonym::IsMorphNoun() const
{
	return GetGramTab()->IsMorphNoun(m_iPoses);
};

bool	CHomonym::IsLeftNounModifier() const
{
	return GetGramTab()->is_left_noun_modifier(m_iPoses, m_iGrammems);
};


string	CHomonym::GetGrammemsStr() const
{
	return GetGrammemsByAncodes();
};





bool CHomonym::ProcessLemmaAndGrammems(const char* CurrStr)
{
	StringTokenizer tok(CurrStr," ");

	//  if it a punctuation mark then return true, and do not try to find lemma and morph. information
	if( !tok() ) return true;


	// lem-sign
	{
		const char* s = tok.val();
		if	(		(strlen(s) != 3) 
				||	(		(s[0] != '+') 
						&&	(s[0] != '-') 
						&&	(s[0] != '?') 
					)
			)

			
		{
			ErrorMessage( Format("Bad lem-sign in \"%s\" ",CurrStr) );
			return false;
		}
		m_LemSign = s[0];
		m_CommonGramCode = s+1;
	}

	// lemma
	if( !tok() ) return false;
	SetLemma(tok.val());
	if( !m_strLemma.empty() )
	{
		m_iCmpnLen = strcspn (m_strLemma.c_str(),"-");
		m_bCmplLem =  ((BYTE)m_iCmpnLen != m_strLemma.length());
	}

	// ancode
	if( !tok() ) return false;
	m_GramCodes = tok.val();
	
	
	// paradigm ID
	if( !tok() ) return false;
	m_lPradigmID = atoi(tok.val());

	// frequence
	if (!tok() ) return false;
	m_lFreqHom = atoi(tok.val());
    if (0 == m_lFreqHom) 
				m_lFreqHom = 1; 

	return true;
}






void CHomonym::DeleteOborotMarks()
{
		m_bOborot1 = false;
		m_bOborot2 = false;
		m_bInOb = false;
		m_OborotNo = -1;
};

void  CHomonym::SetMorphUnknown()
{
    CAncodePattern::SetMorphUnknown();
    m_lPradigmID = -1;
}

void  CHomonym::SetHomonym(const CFormInfo* F)
{
    m_GramCodes = F->GetSrcAncode();
    m_CommonGramCode   = F->GetCommonAncode();
    m_lPradigmID = F->GetParadigmId();
    m_LemSign = F->GetLemSign();
    m_strLemma = F->GetWordForm(0);
    InitAncodePattern();
}


