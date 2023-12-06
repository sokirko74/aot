// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko



#include "Sentence.h"
#include "Word.h"
#include "assert.h"
#include "SynPlmLine.h"
#include "synan/SimpleGrammarLib/SimpleGrammar.h"
#include "morph_dict/agramtab/GerGramTab.h"


//===============================================
//================= CLASS CSynHomonym ==============
//===============================================

CSynHomonym::CSynHomonym(const CSentence* pSent) : CHomonym (pSent->GetOpt()->GetGramTab() )
{
	m_pSent = pSent;
	m_CoordConjNo  = -1;
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
}

const	CSyntaxOpt* CSynHomonym::GetOpt() const	
{
	return m_pSent->GetOpt(); 
};

bool CSynHomonym::CompareWithPredefinedWords(const CLemmaList& ll) const
{
	return ll.has_lemma_with_poses(m_iPoses, GetLemma());
	
}


//===============================================
//================= CLASS uint16_t ==================
//===============================================

CSynWord::CSynWord(CSentence* pSent) : CLemWord(pSent->m_pSyntaxOptions->GetGramTab())
{
	m_pSent = pSent;
	Reset();	
};

const	CSyntaxOpt* CSynWord::GetOpt() const	
{
	return m_pSent->GetOpt(); 
};



void CSynWord::UpdateConjInfo()
{
	m_SubordinateConjNo = -1;
	for(int i = 0 ; i < m_Homonyms.size() ; i++ )
	{
		{
			CSynHomonym& pHomonym = m_Homonyms[i];
			if (		!m_bPredicted 
					&&	(			(GetOpt()->m_Language != morphGerman)
							||		!pHomonym.HasPos(gPRP) // "bis", "als" and  so on
						)
				)

				m_SubordinateConjNo = GetOpt()->GetOborDic()->FindSubConj(pHomonym.GetLemma() );
            pHomonym.m_CoordConjNo = m_pSent->GetCoordConjNo(pHomonym.GetLemma().c_str());
		}
	}
    m_bSimilarConj = m_pSent->GetCoordConjNo(m_strUpperWord.c_str()) != -1;
};







void CSynWord::Reset()
{
    CLemWord::Reset();
    m_Homonyms.clear();

	m_iClauseNo = -1;
	m_Homonyms.clear();
	m_bInTermin = false;
	m_ThesType = NoneThes;
	m_bFirstWordInTermin = false;;
	m_bLastWordInTermin = false;
	m_iTerminID = -1;
	m_bArtificialCreated = false;
	m_iReduplication = 0;
	m_bSmallNumber = false;

	m_bSimilarConj = false;

	m_SubordinateConjNo = -1;
	m_bBadParenthesis = false;
	m_TrennbarePraefixWordNo = -1;
	m_MainVerbs.clear();
}


CSynHomonym CSynWord::CloneHomonymByAnotherHomonym(const CSynHomonym* pHomonym, uint64_t iGrammems, BYTE iTagID) const 
{
	CSynHomonym hom = *pHomonym;
	if( iTagID == UnknownPartOfSpeech )
		hom.m_iPoses = 0;
	else
		hom.m_iPoses = (1 << iTagID);
	hom.m_iGrammems = iGrammems;

	return hom;
}



void CSynWord::CloneHomonymForOborot(int HNum)
{
	assert(m_Homonyms.size() > 0);

	CSynHomonym H = CloneHomonymByAnotherHomonym( HNum == -1 ? &m_Homonyms.back() : &m_Homonyms[HNum], 0, UnknownPartOfSpeech);
	
	if( HasOborot1() )
	{
		//nim : добавить ч.р. = ВВОДН для оборота с одноименной GF 
		if (!m_bBadParenthesis)
			if ( GetOborotPtr()->HasPartOfSpeech(GetOpt()->m_RusParenthesis) )
            {
                H.m_iPoses |= (1<<GetOpt()->m_RusParenthesis);
            }
		
	}
	if(HNum == -1 && m_Homonyms.size()>1)
		H.m_lPradigmID = -1;

	// у всех остальных омонимов помуты оборота стоять не будет
	DeleteOborotMarks();
	
	//  вставляем  омоним в начало списка , чтобы   работала функция  CSynWord::GetOborotNo()
	m_Homonyms.insert(m_Homonyms.begin(), H);
}




bool CSynWord::InitializePlmLine(CSynPlmLine& pPlmWord, int HomonymNo)  const
{
	if( (HomonymNo < 0) || (HomonymNo >= m_Homonyms.size()) )
		return false;

	const CSynHomonym& pActiveHomonym = m_Homonyms[HomonymNo];
	pPlmWord.SetGrammems(pActiveHomonym.m_iGrammems) ;
	pPlmWord.SetPoses(pActiveHomonym.m_iPoses);
    pPlmWord.SetGramcodes(  pActiveHomonym.GetGramCodes() );

    pPlmWord.InitSynPlmLine(this,&pActiveHomonym);
	

	if (m_bComma) pPlmWord.SetFlag(fl_comma);
	if (HasDes(ODigits)) pPlmWord.SetFlag(fl_digit);
	if (m_bWord && HasDes(OLLE)) pPlmWord.SetFlag(fl_ile);
	if (m_bWord) pPlmWord.SetFlag(fl_le );
	if (pActiveHomonym.m_bOborot1) pPlmWord.SetFlag(fl_oborot1 );
	if (pActiveHomonym.m_bOborot2) pPlmWord.SetFlag(fl_oborot2  );
	if (HasDes(OPun)) pPlmWord.SetFlag(fl_punct);
	if (pActiveHomonym.m_bSmallNumber) pPlmWord.SetFlag(fl_small_number);
	if (m_Homonyms.size() > 1) pPlmWord.SetFlag(fl_ambiguous);
	
	if (pActiveHomonym.m_bCanSubdueInfinitive) pPlmWord.SetFlag(fl_can_subdue_infinitive  );
	if (pActiveHomonym.m_bCanSubdueInstr) pPlmWord.SetFlag(fl_can_subdue_instr );
	if (pActiveHomonym.m_bNounHasAdjectiveDeclination) pPlmWord.SetFlag(fl_noun_has_adj_declination  );
	if (HasDes(ONumChar)) pPlmWord.SetFlag(fl_dg_ch );
	if (pActiveHomonym.m_bRussianOdin) pPlmWord.SetFlag(fl_russian_odin );
	if (pActiveHomonym.IsIsOb()) pPlmWord.SetFlag(fl_in_oborot );

	if (GetOpt()->GetGramTab()->IsStandardParamAbbr(m_strUpperWord.c_str())) pPlmWord.SetFlag(fl_standard_param_abbr  );
	if (m_pSent->IsProfession(pActiveHomonym)) pPlmWord.SetFlag(fl_ranknoun  );
	if (pPlmWord.is_single_punct('.')) pPlmWord.SetFlag(fl_fullstop );
	
	
	if (GetOpt()->GetGramTab()->IsMorphNoun(pActiveHomonym.m_iPoses)) pPlmWord.SetFlag(fl_morph_noun);
	if (pActiveHomonym.IsSynNoun()) pPlmWord.SetFlag(fl_syn_noun );
	
	
	return true;
}


const COborotForSyntax*	CSynWord::GetOborotPtr() const
{
	assert ( !m_Homonyms.empty() );
	return m_Homonyms[0].GetOborotPtr();
};





void CSynWord::EraseHomonym(int iHom)
{
	//assert (m_pSent->m_Clauses.empty());

	m_Homonyms.erase(m_Homonyms.begin() + iHom);

	BuildTerminalSymbolsByWord();
}




bool CheckGrammems(const CSynHomonym& L, const CGrammarItem& I)
{
	if (I.m_MorphPattern.m_SearchStatus != NotWord)
	{
		if ((I.m_MorphPattern.m_SearchStatus == DictionaryWord) == (L.m_lPradigmID == -1))
			return false;
	};

	//  nor punctuation marks neither abbreviations can match a morphological pattern
	if (!L.m_iPoses)
		return false;
	
	if	((I.m_MorphPattern.m_Grammems & L.m_iGrammems) != I.m_MorphPattern.m_Grammems) 
		return false;

	return		(I.m_MorphPattern.m_Poses== 0) 
			||	(I.m_MorphPattern.m_Poses & L.m_iPoses) > 0; 

};

bool CSynWord::IsEqualToGrammarItem(const CSynHomonym& L, const CGrammarItem& I)
{
	if (!I.m_MorphPattern.m_GrmAttribute.empty())
	{
		if (!CheckGrammems(L,I))
			return false;
	};

    if (I.m_TokenType == LLE)
        if (!HasDes(OLLE))
            return false;
    if (I.m_TokenType == RLE)
        if (!HasDes(ORLE))
            return false;
    if (I.m_TokenType == NUM_TOKEN)
        if (!HasDes(ODigits))
            return false;
    if (I.m_TokenType == PUNCTUAT)
        if (!HasDes(OPun))
            return false;
    if (I.m_TokenType == ROMAN_NUM)
        if (!HasDes(ORoman))
            return false;



	if (!I.m_Token.empty())
		if	(			(L.GetLemma() != I.m_Token) // equal lemmas
					&&	(		(I.m_Token[0]!= '*') // or equality with right truncation
							||	(L.GetLemma().length() <= I.m_Token.length())
							||	(I.m_Token.substr(1) != L.GetLemma().substr(L.GetLemma().length()-I.m_Token.length() + 1)) 
						)
			)
			return false;

	if (I.m_pListFile != NULL)
	{
		
		const StringSet& PossibleLemmas = I.m_pListFile->m_PossibleLemmas;
		if (PossibleLemmas.find(L.GetLemma()) == PossibleLemmas.end()) // check lemma
			if	(		(L.GetLemma() == m_strUpperWord) // check the token itself
					||	!m_bPredicted
					||	PossibleLemmas.find(m_strUpperWord) == PossibleLemmas.end() 
				)
			{
				int hyphen = L.GetLemma().find('-'); // check the postfix after the last hyphen, if there is a hyphen 
				if	(		(hyphen == std::string::npos)
						|| ( PossibleLemmas.find(L.GetLemma().substr(hyphen+1)) == PossibleLemmas.end() )
					)
				return false;
			};
	};

	
	return true;
};



// we should try to call this procedure only on the first stage, 
// not each time when it is needed
void CSynWord::BuildTerminalSymbolsByWord()							
{

	if (GetOpt()->m_Language != morphGerman) return;


	const CWorkGrammar& G = GetOpt()->m_FormatsGrammar;
	const std::vector<CGrammarItem>&	TerminalSymbols = G.m_UniqueGrammarItems;

	// adding an end of stream symbol to each word
	assert (!m_Homonyms.empty());
	m_AutomatSymbolInterpetationUnion.clear();
	m_AutomatSymbolInterpetationUnion.insert(CInputSymbol(G.GetEndOfStreamSymbol(), "", ""));

	for (size_t j=0; j<m_Homonyms.size(); j++)
		m_Homonyms[j].m_AutomatSymbolInterpetation.clear();

	if (m_bDeleted) 
		return;

	for (size_t i=0; i<TerminalSymbols.size(); i++)
	{
		const CGrammarItem& I = TerminalSymbols[i];

		if (I.m_bMeta) continue;

		if (!I.m_bCanHaveManyHomonyms && (m_Homonyms.size() > 1)) continue;

		if (I.m_Register != AnyRegister)
		{
				if (I.m_Register != m_Register)
					continue;
		};
				

		for (size_t j=0; j<m_Homonyms.size(); j++)
			if (IsEqualToGrammarItem(m_Homonyms[j], I))
			{
				CInputSymbol N(i, m_Homonyms[j].GetGramCodes(), m_Homonyms[j].m_CommonGramCode);
				m_Homonyms[j].m_AutomatSymbolInterpetation.insert(N);
				m_AutomatSymbolInterpetationUnion.insert(N);
			};
	}

};


void CSynWord::InitLevelSpecific(CSynHomonym& h)
{

    m_pSent->InitHomonymLanguageSpecific(h, this);

	if	(		( m_bWord && (GetOpt()->m_Language == morphRussian))
			||	( HasDes(OLLE) && (GetOpt()->m_Language != morphRussian))
		)
	{
		m_pSent->InitHomonymMorphInfo(h);
	}
    

}


void CSynWord::SetGoodHomonym(int i )
{  
	   m_Homonyms[i].m_bGoodHomonym = true;
	   m_Homonyms[i].m_bDelete	 = false;
}

