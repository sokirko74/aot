
#include "GerSynan.h"
#include "GerSentence.h"
#include "GerFormatCaller.h"
#include "GerWord.h"



CSentence* NewSentenceGerman (const CSyntaxOpt* pSyntaxOptions)
{
	return new CGerSentence(pSyntaxOptions);
};

CGerSentence::CGerSentence(const CSyntaxOpt* pSyntaxOptions)  : CSentence(pSyntaxOptions)
{
	InitClauseVectorRules();

}

CGerSentence::~CGerSentence()
{

};

void CGerSentence::ReadNextFromPlmLinesLanguageSpecific()
{
};





// new

bool CGerSentence::RunSyntaxInClauses(ESynRulesSet)
{
	try
	{
		if( m_pSyntaxOptions == NULL )
			return false;
		
		for(int i = 0 ; i < GetClausesCount() ; i++ )
		{
			BuildGLRGroupsInClause(GetClause(i));
		}

		
		return true;
	}
	catch(...)
	{
		OutputErrorString("Failed RunSyntaxInClause");
		return false;
	}
}


void CGerSentence::DeleteHomOneToThousand()
{
	for (int i = 0; i < m_Words.size(); i++)
	{
		m_Words[i].SetHomonymsDel(false);
		for (int j = 0; j < m_Words[i].GetHomonymsCount(); j++)
		{
			CSynHomonym& HomF = m_Words[i].GetSynHomonym(j);
			for (int k = 0; k < m_Words[i].GetHomonymsCount(); k++)
			{
				CSynHomonym& HomS = m_Words[i].GetSynHomonym(k);
				if (HomF.m_lFreqHom > 0 && HomS.m_lFreqHom > 0)
				{
					int iFrq = HomF.m_lFreqHom/HomS.m_lFreqHom;
					if (iFrq >= 1000) HomS.m_bDelete = true;
				}
			}
		}

		m_Words[i].DeleteMarkedHomonymsBeforeClauses();
	}
}

void CGerSentence::AddWeightForSynVariantsInClauses()
{
	for (int i = 0; i < GetClausesCount(); i++)
	{
		CClause&  pClause = GetClause(i);
		
		pClause.AddVariantWeightWithHomOneToFifty();
	}
}

bool  CanBeSubstantive(const CSynWord& W)
{
	for( int i = 0 ; i <W.m_Homonyms.size() ; i++ )
		if (( W.m_Homonyms[i].m_iGrammems & gAllCases)  > 0)
			return true;	

	return false;
}

void CGerSentence::DeleteHomonymsUsingGrossSchriebung()
{
	for (size_t WordNo = 0; WordNo<m_Words.size(); WordNo++)
	{
		CSynWord& W = m_Words[WordNo];
		if (!W.m_bWord) continue;
		
		bool bHasBeSubstantive = CanBeSubstantive(W);

		for (size_t k=0; k<W.m_Homonyms.size(); k++)
		{			
			// if only one homonym left, we should exit
			if (W.m_Homonyms.size() == 1) break;

			CSynHomonym& H = W.m_Homonyms[k];
			if (W.m_Register == LowLow)	
			{
				//  word is lowercase 
				if	(		H.IsMorphNoun()  // delete noun interpetation
						||	(		H.HasPos(gPRONOMEN) //  possessive pronouns  as nouns should be uppercase 
														//  for example: "Meiner ist 18 Meter lang"
								&&	H.HasGrammem(gPossessiv) 
							)
					) 
				{
					W.EraseHomonym( k );
					k--;
					continue;
				};
			}

			if (W.m_Register == UpLow)
			{
				//  the first char of word is uppercase 
				if (		(WordNo > 0)
						&&	bHasBeSubstantive
						&&  (( H.m_iGrammems & gAllCases)  == 0) // this homonym is not a substantive
						&&	(		W.m_bPredicted
								||	(			(WordNo > 0)
										&&	m_Words[WordNo-1].m_bWord
									)

							)

					) 
					{
						// We delete not-noun interpetation only if this word can be 
						// a noun. We hope that this can prevent from deleting crucial ambiguity in cases,   when 
						// somebody uses uppercase words in titles or in geographical terms. For example:
						// "in der Brandenburgischen Akademie"
						W.EraseHomonym(  k );
						k--;
						continue;
					};

				
			};

			if	(		H.HasGrammem (gAbbreviation)
					&&	(W.m_strWord.length() > 1)
					&&  is_lower_alpha(W.m_strWord[1], morphGerman)
				)
			{
				/*
				delete an abbreviation interpretation if the second char is lowercase
				for example:
				 1. IMs, IM - is an abbreviation
				 2. Im im - cannot be an abbreviation

				*/
					W.EraseHomonym( k);
					k--;
					continue;
			};


		};
			
		

	};

};

/*
this function solves simple cases of ambiguity for 
 1. "der" as pronoun and "der" as  article
 2. "einer" as pronoun and "einer" as  article
 "der Tag" -> article
 "der gute Tag" -> article
 "einer Frau" -> article
 "einer wird das kaum erfreulich finden" -> pronomen
  du bist mir einer -> pronomen
  das ist der Rat eines, der die Lage kennt  -> pronomen
*/

void CGerSentence::SolveAmbiguityDeterminerAndPronoun()
{
	for (size_t  ClauseNo  =0; ClauseNo < m_Clauses.size(); ClauseNo++)
	{
		CClause& C = m_Clauses[ClauseNo];
		for (size_t WordNo = C.m_iFirstWord; WordNo<= C.m_iLastWord; WordNo++)
		{
			CSynWord& W = m_Words[WordNo];

			if (!W.m_bWord) continue;

			// ambiguity  in relative words should be already solved
			if (WordNo == C.m_RelativeWord.m_WordNo) continue;


			
			int DetermHomonymNo = W.GetHomonymByPOS(gART);
			int PronounHomonymNo = W.GetHomonymByPOS(gPRONOMEN);
			if	(		(DetermHomonymNo == -1) 
					||	(PronounHomonymNo == -1) 
				)
			continue;

			// if it is the last word or  it is followed by punctuation or a unambigious verb,
			// then it must be a pronoun
			if	(		( WordNo == C.m_iLastWord )
                    ||	m_Words[WordNo+1].HasDes(OPun)
					||  (		(m_Words[WordNo+1].m_Homonyms.size() == 1)
							&&	m_Words[WordNo+1].m_Homonyms[0].HasPos(gVER)
						)
				)
			{
				// delete article homonym
				C.DeleteHomonym(WordNo,DetermHomonymNo );
				continue;
			}

			// if it is followed unambigious noun or adjeckive,
			// then it should be an article, I think that conterexamples could be imagined, but 
			// they are highly infrequent.
			if	(		(m_Words[WordNo+1].m_Homonyms.size() == 1)
					&&	(
								m_Words[WordNo+1].m_Homonyms[0].IsLeftNounModifier()
							||	m_Words[WordNo+1].m_Homonyms[0].IsMorphNoun()	
						)
				)
			{
				// delete pronoun homonym
				C.DeleteHomonym(WordNo,PronounHomonymNo );
				continue;
			};
			
			
		};
	};

};





EClauseType CGerSentence::GetClauseTypeByAncodePattern (const CAncodePattern& Pattern) const
{
    if (Pattern.HasPos(gVER))
        return VERBSATZ_T;
    else
        if (Pattern.HasPos(gZU_INFINITIV))
            return INFINITIVSATZ_T;
	
	return UnknownPartOfSpeech;
};


inline size_t GetCommonPrefixSize(const char* s1,const char* s2)
{
	size_t count = 0;
	assert(s1);
	assert(s2);
	while ((*s1 == *s2) && *s1 && *s2)
	{
		s1++;
		s2++;
		count++;
		
	};
	return count;
};
bool	CGerSentence::IsProfession(const CSynHomonym& H) const
{
    assert (false);
    return false;
};



// go through  the sentence
// if a wordform occurs, which is  simultaneously a finite verb and a participle then split 
// the word into two homonyms: a participle and a verb 
// For example, "zerstцrte" can be PA2 and VER sft,prt,1,sin,

void CGerSentence::ParticipleAndVerbInOneForm() 
{
	for (size_t WordNo = 0; WordNo< m_Words.size(); WordNo++)
	{
		CSynWord& W = m_Words[WordNo];
		for (size_t h=0; h < W.m_Homonyms.size(); h++)
		{
			CSynHomonym& H = W.m_Homonyms[h];
			if (H.m_GramCodes == "??") continue;
			if (!H.HasPos(gPA2) || !H.HasPos(gVER)) continue;

			string VerbGramCodes;
			string PartGramCodes;
			
			for (long i=0; i < H.m_GramCodes.length(); i+=2)
			{
				string gram = H.m_GramCodes.substr(i, 2);
				BYTE POS = GetOpt()->GetGramTab()->GetPartOfSpeech(gram.c_str());
				if (POS == gPA2)
					PartGramCodes += gram;
				else 
					VerbGramCodes += gram;
			};
			assert(!VerbGramCodes.empty() && !PartGramCodes.empty());

			H.m_GramCodes = VerbGramCodes;
			InitHomonymMorphInfo(H);

			CSynHomonym PartHom = H;
			PartHom.m_GramCodes = PartGramCodes;
			InitHomonymMorphInfo(PartHom);
			W.m_Homonyms.push_back(PartHom);

			W.BuildTerminalSymbolsByWord();

		};
		
	 };
};




bool	CGerSentence::SetClauseBorderIfThereAreTwoPotentialPredicates(int FWrd, int LWrd)
{
	return false;
};


void CGerSentence::FindReflexivePronomen()
{
	for (size_t  ClauseNo  =0; ClauseNo < m_Clauses.size(); ClauseNo++)
	{
		CClause& C = m_Clauses[ClauseNo];

		
		//  collecting all subjects in set<string> Subjects
		for (SVI it = C.m_SynVariants.begin(); it != C.m_SynVariants.end(); it++)
		{
			CMorphVariant& V = *it;
			if (V.m_ClauseTypeNo == -1) continue;
			if (C.m_vectorTypes[V.m_ClauseTypeNo].m_Root.IsEmpty()) continue;
			int MainWordNo = C.m_vectorTypes[V.m_ClauseTypeNo].m_Root.m_WordNo;
			assert (CPeriod(MainWordNo).is_part_of(C));
			int ReflVerb = V.UnitNoByWordNo(MainWordNo); 
			if (V.m_SynUnits[ReflVerb].HasGrammem(gModal))
			{
				//  if the main verb is modal, then search for the first infinitive,
				//  for example:
				//  "Du sollst dir wahrscheinlich eine Jacke anziehen."
				//  "weil sie sich zwei Dienste nicht leisten wollten"
				for (int i  = V.m_vectorGroups.get_main_word(0);i < V.m_SynUnits.size(); i  = V.m_vectorGroups.get_next_main_word(i))
					if (		V.m_SynUnits[i].HasGrammem(gInfinitiv) 
							&& (i !=  ReflVerb)
						)
					{
						ReflVerb = i;
						break;
					};
			}

			{
				//  auxiliary verbs with reflexive verbs
				// "Ich habe mich entschieden"
				const CSynWord& W = m_Words[V.GetUnitFirstWord(ReflVerb)];
				if (!W.m_MainVerbs.empty())
				{
					ReflVerb = V.UnitNoByWordNo(*W.m_MainVerbs.begin());
					assert (ReflVerb != -1);
					if (ReflVerb == -1) continue;
				};
			};

			set<string> Subjects;

			
			for (size_t i=0; i< V.m_Subjects.size(); i++)
				Subjects.insert( m_Words[V.GetUnitFirstWord(V.m_Subjects[i])].m_strUpperWord );

			bool bHomonymWasDeleted = false;
			for (int i  = V.m_vectorGroups.get_main_word(0);i < V.m_SynUnits.size(); i  = V.m_vectorGroups.get_next_main_word(i))
			{
				if (V.m_SynUnits[i].m_Type != EWord) continue;
				const string& Pronoun = m_Words[V.GetUnitFirstWord(i)].m_strUpperWord;
				
				if (!V.m_SynUnits[i].HasGrammem(gReflexiv)) continue;
				// checking subject and reflexive pronoun
				if	(		(	(		(Pronoun == "MIR")
									||	(Pronoun == "MICH")	
								)
								&& (Subjects.find("ICH") != Subjects.end())
							)
						|| (	(		(Pronoun == "DIR")
									||	(Pronoun == "DICH")	
								)
								&& (Subjects.find("DU") != Subjects.end())
							)
						|| (		(Pronoun == "UNS")
								&& (Subjects.find("WIR") != Subjects.end())
							)
						|| (		(Pronoun == "EUCH")
								&& (Subjects.find("IHR") != Subjects.end())
							)
						|| (Pronoun == "SICH")
					)
				{
					V.m_SynUnits[ReflVerb].m_bReflexive = true;
					if (		(		V.m_SynUnits[ReflVerb].HasGrammem(gSichAcc)
									&&	V.m_SynUnits[i].HasGrammem(gAkkusativ)
								)
							||	(		V.m_SynUnits[ReflVerb].HasGrammem(gSichDat)
									&&	V.m_SynUnits[i].HasGrammem(gDativ)
								)
						)
					{	
						int WordNo = V.GetUnitFirstWord(i);
						m_Words[WordNo].SetAllOtherHomsDel(V.m_SynUnits[i].m_iHomonymNum);
						C.DeleteMarkedHomonyms(WordNo);
						bHomonymWasDeleted = true;
						break;
					};
					

				}
				else
				// imperative + possibly reflexive verb:
				// "Schдme dich!"
				if (		V.m_SynUnits[ReflVerb].HasGrammem(gImperativ)
						&&	(		V.m_SynUnits[ReflVerb].HasGrammem(gSichAcc)
								||	V.m_SynUnits[ReflVerb].HasGrammem(gSichDat)		
							)
						&&	Subjects.empty()
						&&	(		(Pronoun == "DIR")
								||	(Pronoun == "DICH")	
								||	(Pronoun == "EUCH")
							)
					)
					V.m_SynUnits[ReflVerb].m_bReflexive = true;

			}
			if (bHomonymWasDeleted) break;

		};

	};

	

};


bool CGerSentence::BuildClauses()
{
	//UnitOrdinalNumbersWithFullStop();
	DeleteHomonymsIfTooManyPredictedWords();
	ParticipleAndVerbInOneForm();
	DeleteHomonymsUsingGrossSchriebung();
	
	BuildTrennbarePraefixe();
	BuildAnalyticalForms();
	m_bPanicMode = IsPanicSentence();
	if (m_bPanicMode)
	{
		rml_TRACE("!!!!!!!!!!  Entering panic mode !!!!!!!!!!!1\n");
	};

	BuildGLRGroupsInSentence();

	CGerSentence SavedSentence(GetOpt());
	SavedSentence = *this;
	m_bShouldUseTwoPotentialRule = !m_bPanicMode;
TryWithoutTwoPotentialRule:
	bool bRes = true;


	assert ( GetClausesCount() == 0 );


	DeleteHomOneToThousand();

	FindGraPairs();

	CloneHomonymsForOborots();

	AscribeSimplePrepositionInterpretations(gPRP);

	FindAllTermins();

	ProcessGrammarModels();

	bool SecondTryOfCoverageKillHomonyms = false;
	vector<CSynWord> SaveWords = m_Words;
	vector<SFoundTermin> SaveFoundTermins = m_vectorTermins;
BuildInitialClausesLabel:

	if(! BuildInitialClauses() )
	{
		return false;

	};

	SolveAmbiguityDeterminerAndPronoun();

	assert ( IsValid() );

	TraceClauses();

	//  we need synvariants in order to build the first clause structure
	RunSyntaxInClauses(AllRules);

	TraceClauses();

	OneRunOfClauseRules(m_vectorMainEncloseRules);

	TraceClauses();

	
	// после первого вызова RunSyntaxInClause нужно удалить омонимы, которые противоречат найденным терминам
	
	DeleteMarkedHomonymsWithClauses(CPeriod(0,(int)m_Words.size()-1));

	TraceClauses();


	RunSyntaxInClauses(AllRules);

	FindReflexivePronomen();
	assert ( IsValid() );
	
	
	RecalculateRelationsCoordinates();	
	AssignClauseNoToWords();
	assert ( IsValid() );

	AddWeightForSynVariantsInClauses();
	assert ( IsValid() );



	if (!m_bPanicMode)
	{
		for ( int tt = 0; tt < GetClausesCount(); tt++ )
			if ( 0 == GetClause(tt).GetSynVariantsCount() )
				if (m_bShouldUseTwoPotentialRule)
			{
				*this = SavedSentence;
				m_bShouldUseTwoPotentialRule = false;
				goto TryWithoutTwoPotentialRule;
			}



		if (m_pSyntaxOptions->m_KillHomonymsMode == CoverageKillHomonyms)
		{
			vector<size_t> RootsCount;
			set<size_t> RelativeWords;
			for ( int ClauseNo = 0; ClauseNo < GetClausesCount(); ClauseNo++ )
			{
				RootsCount.push_back(GetClause(ClauseNo).m_vectorTypes.size() );
				if (!GetClause(ClauseNo).m_RelativeWord.IsEmpty())
					RelativeWords.insert(GetClause(ClauseNo).m_RelativeWord.m_WordNo);
			};

			/*
				if at least one clause root was deleted 
				or there is at leastz one clause with no root or no synvarinats
				then the program processes the whole sentence on more time
			*/
		    if (    KillHomonymsInAllSentence()
			   && !SecondTryOfCoverageKillHomonyms
			   )
		   {
			   for ( int ClauseNo = 0; ClauseNo < GetClausesCount(); ClauseNo++ )
			   {
				   const CClause& Clause = GetClause(ClauseNo);
				   if (			Clause.m_vectorTypes.empty()  
							||	(RootsCount[ClauseNo] !=  Clause.m_vectorTypes.size())
					   )
				   {
					  SecondTryOfCoverageKillHomonyms = true;
					  /*
						We should restore initial m_Words vector, since it was changen by 
						CGerSentence::BuildAnalyticalForms, but we should restore the homonyms
						which were deleted by words KillHomonymsInAllSentence,
						so m_Words = SaveWords is not relevant
					  */
					  vector<CSynWord> p;
					  for (int i =0; i < SaveWords.size(); i++)
					  {
							int WordNo=0;
							for (; WordNo< m_Words.size(); WordNo++)
							  if (			(SaveWords[i].m_GraphematicalUnitOffset == m_Words[WordNo].m_GraphematicalUnitOffset)
									&&		(SaveWords[i].m_strWord == m_Words[WordNo].m_strWord)
								  )
								  break;

							if	(		WordNo < m_Words.size() 

									&&	(RelativeWords.find(WordNo) == RelativeWords.end())
											// we should not delete homonyms from a relative word for the second pass,
											// since the second pass can change clause structure, and a clause will change its status
											// "Die israelische Regierung beschlieЯt, die durch die israelische Militдrverwaltung geleisteten цffentlichen Dienste auf die gleiche Stufe zu stellen "

								)
								p.push_back(m_Words[WordNo]);
							else
								p.push_back(SaveWords[i]);

					  };
					  m_Words = p;
					  m_vectorTermins = SaveFoundTermins;
					  goto  BuildInitialClausesLabel;
				   };
			   };
		   };


		};
	};


	assert ( IsValid() );


	return true;	
}






bool	CGerSentence::AllHomonymsArePredicates(const CSynWord& W) const
{
	return false;
};

