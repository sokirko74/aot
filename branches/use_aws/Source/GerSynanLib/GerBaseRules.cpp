#include "GerSynan.h"
#include "GerWord.h"
#include "GerSentence.h"



bool	CGerSentence::CanBeRelativeAntecedent(const CSynHomonym& H) const
{
	return			H.IsMorphNoun() 

				// das ist der Rat eines, der die Lage kennt
			||	(		H.HasPos(gPRONOMEN)
				  &&	H.IsLemma("EINER")
				)
			||	(		H.HasPos(gPRO_BEG)
				  &&	(			H.IsLemma("JEDER") 
							||		H.IsLemma("JENER") 
							||		H.IsLemma("ALLER") 
						)
				);
};

void CGerSentence::InitClauseVectorRules() 
{
	#define Abbr  m_vectorMainEncloseRules.push_back(CClauseRule((FirstGrRoule)&CGerSentence::
		
		Abbr RuleForSubClause, "RuleForSubClause"));
		Abbr RuleForRelativeClause, "RuleForRelativeClause"));
		Abbr RuleForUnitingEmptyClauseWithStrongLeftBorder, "RuleForUnitingEmptyClauseWithStrongLeftBorder"));
		Abbr RuleForPartzipialKonstruktion, "RuleForPartzipialKonstruktion"));
		//Abbr RuleForUmZuKonstruktion, "RuleForUmZuKonstruktion"));
		Abbr RuleForInfinitive, "RuleForInfinitive"));
		Abbr RuleForModalVerb, "RuleForModalVerb"));
		Abbr RuleForDividedVerbForms, "RuleForDividedVerbForms"));
		Abbr RuleForEmptyClauses, "RuleForEmptyClauses"));
	#undef Abbr
}



static void DeleteHomonym_UM_AsPreposition(CClause* C, int WordNo)
{
	
	int HomonymNo = C->GetWords()[WordNo].GetHomonymByPOS(gKON);
	assert (HomonymNo != -1);
	C->GetWords()[WordNo].SetAllOtherHomsDel(HomonymNo);
	C->m_pSent->DeleteMarkedHomonymsWithClauses(WordNo);

};

bool CGerSentence::RuleForSubClause(int iClauseNum)
{
	int iWord = GetClause(iClauseNum).m_iFirstWord;

	if ( IsEnclosedClause (iClauseNum) )  return false;


	//  if the right border  is ambigious, then exit
	if(GetMinClauseByLastWord(m_Clauses[iClauseNum].m_iLastWord) !=  iClauseNum)
		return false;


	int iNext = GetMaxClauseByFirstWord(m_Clauses[iClauseNum].m_iLastWord+1);
	if(iNext == -1)
		return false;

	CClause* pAbstClause1 = &GetClause(iNext);


	if(pAbstClause1->m_vectorTypes.empty())
		return false;

	if( pAbstClause1->size() < 2 )
		return false;

	int iFirstWord = pAbstClause1->m_iFirstWord;
	bool bSubConj =  (GetWords()[iFirstWord].m_strWord[0] == ',') 
					&& pAbstClause1->HasSubConj();
	if (!bSubConj) 
		return false;

	int UM_ConjNo  =  pAbstClause1->FindClauseSubordConj("UM");
	if ( UM_ConjNo != -1)
	{	
		// "um" should start an infinitive sentence 
		if (!pAbstClause1->HasType(INFINITIVSATZ_T))
			return false;

		// delete  a homonym "um" as a preposition
		assert (pAbstClause1->m_vectorConjs[UM_ConjNo].size() == 1);
		DeleteHomonym_UM_AsPreposition(pAbstClause1, pAbstClause1->m_vectorConjs[UM_ConjNo].m_iFirstWord);

		// delete all clause types except infinitive
		for (int i=pAbstClause1->m_vectorTypes.size()-1; i>=0; i--)
			if (pAbstClause1->m_vectorTypes[i].m_Type != INFINITIVSATZ_T)
				pAbstClause1->m_vectorTypes.erase(pAbstClause1->m_vectorTypes.begin() + i);

	}

					
	return EncloseClauseAsWord(iNext, iClauseNum);
}

int GetDeterminerHomonym(const CGerSentence& S, size_t WordNo, int LastUnusedStarter)
{
	const CSynWord& W = S.m_Words[WordNo];

	bool	bNextIsPrep =		(WordNo+1 < S.m_Words.size())
							&&	S.m_Words[WordNo+1].GetHomonymByPOS(gPRP) != -1;

	for (size_t i=0; i<W.m_Homonyms.size(); i++)
	{
		const CSynHomonym& H = W.m_Homonyms[i];
		QWORD dummy;
		if	(		is_article_for_weak_declination(H)
				||	is_article_for_mixed_declination(H)
				||	IsPrepositionArticle(W.m_strUpperWord.c_str(), dummy)
				||	(		bNextIsPrep 
						&&	H.HasPos(gPRP) 
						&&	!H.IsLemma("BIS")
						&&	(		!WordNo 
								||	(LastUnusedStarter+1 !=  WordNo) // Das laut um Hilfe schreiende Kind
							)
					)
			)
			return i;
	};
	return -1;
};

bool CGerSentence::RuleForPartzipialKonstruktion(int iClauseNum)
{
	const CClause& C = m_Clauses[iClauseNum];
	int LastUnusedStarter = -1;
	int LastUnusedComma = -1;

	for (int WordNo=C.m_iFirstWord; WordNo< C.m_iLastWord; WordNo = PassSubClauses(C, WordNo))
	{
		const CSynWord& W = m_Words[WordNo];
		// saving last unused article, possessive pronoun or compound prepostion
		int DetermHomonym = GetDeterminerHomonym(*this,WordNo, LastUnusedStarter);
		if ( DetermHomonym != -1 )
		{
			if (!C.GetMaxLastHost(WordNo, DetermHomonym))
				LastUnusedStarter = WordNo;
			continue;
		};
		if (W.m_bComma)
		{
			if (!C.GetMaxLastHost(WordNo))
				LastUnusedComma = WordNo;
			continue;
		};
		

		if (LastUnusedStarter == -1) continue;
		//  a participle sentence should contain at least two words...
		if (LastUnusedStarter+1 == WordNo) continue;

		// getting participle homonym
		int PartizipHomonymNo = -1;
		for (int h=0; h <W.m_Homonyms.size(); h++)
			if	(		W.m_Homonyms[h].HasPos(gPA1)
					||	W.m_Homonyms[h].HasPos(gPA2)
					||	W.m_Homonyms[h].m_bAdjWithActiveValency // "aller im Inland befindlichen Stucke"
				)
			if (W.m_Homonyms[h].IsLeftNounModifier())
			{
				PartizipHomonymNo = h;
				break;
			};
		if (PartizipHomonymNo == -1) continue;


		// creating a virtual sentence from article, participle and noun, 
		// check whether we can build a group, that covers this sentence
		assert(WordNo < C.m_iLastWord);
		CGerSentence S(GetOpt());
		S.m_Words.push_back(m_Words[LastUnusedStarter]);
		S.m_Words.push_back(m_Words[WordNo]);
		S.m_Words.push_back(m_Words[WordNo+1]);

		if (		(m_Words[WordNo+1].GetHomonymByPOS(gSUB) == -1)
				&&	(m_Words[WordNo+1].GetHomonymByPOS(gEIG) == -1)
				&&	(WordNo+2 < m_Words.size())
			)
			S.m_Words.push_back(m_Words[WordNo+2]);

		S.m_Clauses.push_back(CClause(&S,0,S.m_Words.size() - 1));
		S.RunSyntaxInClauses(AllRules);
		const CGroup* G = S.m_Clauses[0].GetMaxLastHost(1);
		if (!G) continue;
		if (G->size() != S.m_Words.size()) continue;



		{  // if there is already a group, which covers all these words 
           // may be except the starter,
		  // then continue (ignore these case)
			const CGroup* G = C.GetMaxLastHost(LastUnusedStarter+1);
			if (	G 
				&& (G->m_iLastWord >= WordNo+1)
				&& (G->m_iFirstWord <= LastUnusedStarter+1)
				) 
			continue;
		}

		// creating subclause
		SClauseType ClType(PARTIZIPIALSATZ_T, WordNo, PartizipHomonymNo);
		int StartSubClauseWordNo = LastUnusedStarter+1;
		if (LastUnusedComma > LastUnusedStarter)
			StartSubClauseWordNo = LastUnusedComma;

		if (CreateEnclosedNotStrongClause(iClauseNum, StartSubClauseWordNo, WordNo, ClType))
			return true;
		
	};
	return false;
};


// enclose an infinitive clause to the previous one, preferring the clauses, which have
// the predicate "sein",  for example:
// (1) "Der Dienst ist, sofern ich verstehe, zu leisten."
// (2) "Es gebe eine Reihe von Kunden , die AOL nur wegen der fehlenden Möglichkeit, Bankgeschäfte zu tätigen , nicht abonniert hatten"
// For Example (1) we should include "zu leisten" to the main clause, because of "ist".
bool CGerSentence::RuleForInfinitive(int iClauseNum)
{
	CClause& InfinitiveClause = m_Clauses[iClauseNum];
	if (!InfinitiveClause.HasType(INFINITIVSATZ_T)) return false;
	for (size_t i=0; i < InfinitiveClause.m_vectorTypes.size(); i++)
		if (!InfinitiveClause.m_vectorTypes[i].m_Root.IsEmpty())
		{
			const SClauseType&  type = InfinitiveClause.m_vectorTypes[i];
			const CSynHomonym& H = m_Words[type.m_Root.m_WordNo].m_Homonyms[type.m_Root.m_HomonymNo];
			if (H.IsLemma("SEIN") || H.IsLemma("HABEN"))
				return false;
		};

	int MainClauseNo = -1;
	for (size_t i=0; i < m_Clauses.size(); i++)
		if	(m_Clauses[i].m_iLastWord+1 == InfinitiveClause.m_iFirstWord)
		{
			MainClauseNo = i;
			bool bSeinCase = false;
			for (size_t k=0;  k < m_Clauses[i].m_vectorTypes.size(); k++)
				if (!m_Clauses[i].m_vectorTypes[k].m_Root.IsEmpty())
				{
					const SClauseType&  type = m_Clauses[i].m_vectorTypes[k];
					const CSynHomonym& H = m_Words[type.m_Root.m_WordNo].m_Homonyms[type.m_Root.m_HomonymNo];
					if (H.IsLemma("SEIN"))
					{
						bSeinCase = true;
						break; // it is the best case
					};
				}
			if (bSeinCase)
				break;
		};
	if (MainClauseNo == -1)			return false;

	InfinitiveClause.m_RelativeWord.SetEmpty();
	return EncloseClauseAsWord(iClauseNum, MainClauseNo);
}

// uniting two adjacent clause, if the left-hand clause has only one root which is modal verb,
// and the right-hand clause has only one root, which is infinitive without "zu", for example
// Ich kann zu Hause oder im Office bleiben
bool CGerSentence::RuleForModalVerb(int iClauseNum)
{
	const CClause* pMainClause = &m_Clauses[iClauseNum];

	if (pMainClause->m_vectorTypes.size() != 1) 
		return false;
	{
		const SClauseType&  type = pMainClause->m_vectorTypes[0];
		if (type.m_Root.IsEmpty()) return false;
		const CSynHomonym& H = m_Words[type.m_Root.m_WordNo].m_Homonyms[type.m_Root.m_HomonymNo];
		if	(		!H.IsLemma("MÜSSEN")
				&&	!H.IsLemma("KÖNNEN")
				&&	!H.IsLemma("DÜRFEN")
				&&	!H.IsLemma("MÖGEN")
				&&	!H.IsLemma("WOLLEN")
				&&	!H.IsLemma("SOLLEN")
				&&	!H.IsLemma("LASSEN")
			) 
		return false;

	};

	int iNext = GetMaxClauseByFirstWord(pMainClause->m_iLastWord+1);
	if (iNext == -1) return false;
	CClause& InfinitiveClause = m_Clauses[iNext];
	if (InfinitiveClause.m_vectorTypes.size() != 1) 
		return false;
	{
		const SClauseType&  type = InfinitiveClause.m_vectorTypes[0];
		if (type.m_Root.IsEmpty()) return false;
		const CSynHomonym& H = m_Words[type.m_Root.m_WordNo].m_Homonyms[type.m_Root.m_HomonymNo];
		if (!H.HasPos(gVER) || !H.HasGrammem(gInfinitiv)) return false;
	};
	InfinitiveClause.m_RelativeWord.SetEmpty();
	UniteClauses(iClauseNum, iNext, LeftClauseParams);			
	return false;
}



//  "um"  is used without a comma 
bool CGerSentence::RuleForUmZuKonstruktion(int iClauseNum)
{
	const CClause& C = m_Clauses[iClauseNum];
	if (!C.HasType(VERBSATZ_T)) return false;
	if (GetMaxChildByLastWord(C,C.m_iLastWord)  != -1) return false;

	int LastWordNo = C.m_iLastWord;
	for ( ; LastWordNo > C.m_iFirstWord; LastWordNo--)
		if (m_Words[LastWordNo].m_bWord)
			break;

	

	const CSynWord& LastWord = m_Words[LastWordNo];
	int HomonymNo = LastWord.GetHomonymByGrammem(gZuVerbForm);
	if (HomonymNo == -1) return false;

	int LastUM = -1;
	for (int WordNo=LastWordNo; WordNo>C.m_iFirstWord; WordNo--)
	{
		
		if	(		m_Words[WordNo].IsWordUpper("UM") 
				&&	(GetMinClauseByWordNo(WordNo) == iClauseNum)
			) 
		{
			LastUM = WordNo;
			break;
		};
	}
	
	if	(LastUM != -1)
		if	(CreateEnclosedNotStrongClause(iClauseNum, LastUM, LastWordNo, SClauseType(INFINITIVSATZ_T, LastWordNo, HomonymNo)))
		{
			CClause* newClause = FindClauseByPeriod(CPeriod(LastUM, LastWordNo));
			assert (newClause);
			DeleteHomonym_UM_AsPreposition(newClause, LastUM);
			return true;	
		};


	return false;


}

bool CGerSentence::RuleForDividedVerbForms(int iClauseNum)
{
	const CClause& Clause = GetClause(iClauseNum);
	int SeparatedVerbPart = -1;
	ParametersSourceEnum whose_chars_to_copy = LeftClauseParams;

	//  first find a verb with a separated praefix
	// "Bei den Banken trete die langfristige Kreditgeschafte, falls solide Grundsatze beachtet wurden, zuruck."
	for (size_t WordNo=Clause.m_iFirstWord; WordNo <= Clause.m_iLastWord; WordNo++)
		if (m_Words[WordNo].m_TrennbarePraefixWordNo != -1) 
		{
			SeparatedVerbPart = m_Words[WordNo].m_TrennbarePraefixWordNo;
			break;
		};

	if (SeparatedVerbPart == -1) 
	{
		//   find a verb, which has an auxiliary verb in the next clause
		for (size_t WordNo=Clause.m_iFirstWord; WordNo <= Clause.m_iLastWord; WordNo++)
		{
			if (GetMinClauseByWordNo(WordNo)  != iClauseNum)
				continue;

			SeparatedVerbPart = FindFirstAuxVerb(WordNo);
			if (SeparatedVerbPart > Clause.m_iLastWord)
				break;

			for (size_t i=0; i <  m_Words[WordNo].m_MainVerbs.size(); i++)
				if (m_Words[WordNo].m_MainVerbs[i] > Clause.m_iLastWord)
				{
					SeparatedVerbPart = m_Words[WordNo].m_MainVerbs[i]; 
					whose_chars_to_copy = RightClauseParams;
					break;
				};

			if (SeparatedVerbPart > Clause.m_iLastWord)
				break;
		};

		// nothing is found, this rule is not applicable
		if (SeparatedVerbPart == -1) 
			return false;
	};
	
	for (size_t iNext=0; iNext< m_Clauses.size(); iNext++)
		if (		(m_Clauses[iNext].m_iFirstWord == m_Clauses[iClauseNum].m_iLastWord+1)
				&&	CPeriod(SeparatedVerbPart).is_part_of(m_Clauses[iNext] )
			)
		{
			UniteClauses(iClauseNum, iNext, whose_chars_to_copy);
			return  true;
		}

	return false;
};

bool CGerSentence::RuleForEmptyClauses(int iClauseNum)
{
	const CClause& Clause = GetClause(iClauseNum);
	if( !Clause.m_vectorTypes.empty() ) return false;

	// check whether we can add this clause to the left
	int iPrev = GetMinClauseByLastWord(Clause.m_iFirstWord-1);


	// if the previous clause is enclosed and has a predicate at the end, then it cannot be 
	// extended to the right, then we can take the maximal clause, and try it
	//  for example
	// Österreich habe dem deutschen Volk, zu dem es sich bekenne, Dienste geleistet.
	if (iPrev != -1)
	{
		int MaxPrevClause = GetMaxClauseByLastWord(Clause.m_iFirstWord-1);
		if (MaxPrevClause != iPrev)
		{
				for (size_t i=0; i < m_Clauses[iPrev].m_vectorTypes.size(); i++)
				{
					size_t  RootWordNo = m_Clauses[iPrev].m_vectorTypes[i].m_Root.m_WordNo;
					if	(	(RootWordNo == m_Clauses[iPrev].m_iLastWord)
							||	(		(RootWordNo+1 == m_Clauses[iPrev].m_iLastWord)  // if the enclosed clause is in brackets
                                    &&	m_Words[m_Clauses[iPrev].m_iLastWord].HasDes(OPun)
								)
						)
					{
						iPrev = MaxPrevClause;
						break;
					};
				};
		};
	};


	if(			( iPrev != -1 ) 
				// the clause cannot have a subord. conjunction
			&&  (		!Clause.HasLeftStarter()
					||	(		Clause.IsRelative()
							&&	(Clause.m_iLastWord+1 == m_Words.size())
							&&	Clause.m_vectorTypes.empty()
						)
				)
					// check whether the left border is ambiguous
			&&  !IsEnclosedClause(iPrev)
					// the clause cannot start with a semicolumn
			&&  !IsStrictLeftClauseBorder(Clause)
		)
	{
		// "Hingegen erhalt die Liste , welche die relative Mehrheit im ganzen Lande erhalt,"  +   "zwei Drittel aller Mandate."   
		// = "Hingegen erhalt die Liste , welche die relative Mehrheit im ganzen Lande erhalt, zwei Drittel aller Mandate."  
		UniteClauses(iPrev, iClauseNum, LeftClauseParams);			
		return true;	
	}
	else
	{
		// check whether we can add this clause to the right
		int iNext = GetMaxClauseByFirstWord(m_Clauses[iClauseNum].m_iLastWord+1);
		if (		// check whether the right border exists
					( iNext != -1 )
					// the right clause cannot have a subord. conjunction
				&& 	(		!m_Clauses[iNext].HasLeftStarter()	)
					// check whether the right border is ambiguous
				&&  (GetMinClauseByLastWord(Clause.m_iLastWord) == iClauseNum)
					// the right clause cannot start with a semicolumn
				&& !IsStrictLeftClauseBorder(m_Clauses[iNext])
			)
		{
			// "Sein Vater, Rechtsanwalt," +  "war uberzeugter Kommunist" ->  "Sein Vater, Rechtsanwalt, war uberzeugter Kommunist "
			UniteClauses(iClauseNum, iNext, RightClauseParams);
			return  true;
		}
	} 



	return false;
}
