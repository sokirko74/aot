#include  "GerSynan.h"
#include "GerFormatCaller.h"
#include "GerSentence.h"




/*
 this function returns the weight which shows the confidence of possible sibj-predicate relation between 
 Subj and Pred. The minimal weight is 0 (no relation).
 The fucntion checks the coordionation, if no coordination found, it return 0.
 Then it calculates weight, which depends upon the following factors:

 1.Location:
  a. If we are in a subclause, the subject should be at the beginning or near it;
  b. If we are not in a subclause, the subject should as close to the Pred as possible;

 2. Ambiguity:
  a. the more cases the possible subject  has, the less is the output weight. So forms like "du" or "ich"
  will be very confident.

  b. in main clauses pronouns before predicate, which have a proper coordiantion with  the predicate
  should take a priority to other candidates. For example:
  "Das ist der Plan." Here "Das" should be a subject, though "Plan" is less ambiguous than "Das".

  c.if predicate is "SEIN" then a subject before predicate is the most preferable,
	for example "Peter ist ein Verbrecher". Though "Verbrecher" is less ambiguous, "Peter" is
	the real subject.

 3. Between subject and predicate cannot be only one comma or bracket.



*/
int CGerSentence::get_weight_of_subj_pred_relation(const CMorphVariant& synVariant, const int Subj, const int Pred, bool bEnclosed)
{
	const size_t MaxClauseLength = 100;
	const CSynUnit& SubjUnit = synVariant.m_SynUnits[Subj];
	const CSynUnit& PredUnit = synVariant.m_SynUnits[Pred];

	if (SubjUnit.m_Type == EClause) return 0;
	const CSynHomonym&  SubjHom = m_Words[SubjUnit.m_SentPeriod.m_iFirstWord].m_Homonyms[SubjUnit.m_iHomonymNum];
	const CSynHomonym&  PredHom = m_Words[PredUnit.m_SentPeriod.m_iFirstWord].m_Homonyms[PredUnit.m_iHomonymNum];

	// check part of speech 
	if (		!GetOpt()->GetGramTab()->is_morph_pronoun(SubjUnit.m_iPoses) 
			&&	!CanBeRelativeAntecedent(SubjHom)
			&&	!SubjHom.IsLemma("WELCHER")
		)
		return 0;

	// subject should be be host of itself
	int GroupNo = synVariant.m_vectorGroups.get_maximal_group_no(Subj);
	if	(		(GroupNo != -1)
			&&	(synVariant.m_vectorGroups.GetGroups()[GroupNo].m_MainWordNo != Subj)
		)
	return 0;


	// check coordination
	if	(		(GroupNo == -1)
			||	(synVariant.m_vectorGroups.GetGroups()[GroupNo].m_GroupType != GetOpt()->m_SimilarNPGroupType)
		)
	{
		if (!GetOpt()->GetGramTab()->GleicheSubjectPredicate(SubjUnit.m_GramCodes.c_str(), PredUnit.m_GramCodes.c_str()))
			return 0;
	}
	else
	{
		// Paul und Peter  gehen nach Hause.
		if (!(PredUnit.m_iGrammems & _QM(gPlural))) return false;
	};

	
	// counting brackets between subject and predicate
	int CountOfBrackets = 0;
	for (int i=min(Subj, Pred); i<max(Subj, Pred); i++)
	{
		const CSynUnit& U = synVariant.m_SynUnits[i];
		if (U.m_Type == EClause)
		{
			// no comma or bracket counting when there is a subclause between sibj. and pred
			CountOfBrackets = 0;
			break;
		};

		const CSynWord& Word = m_Words[U.m_SentPeriod.m_iFirstWord];
        if (Word.HasDes(OOpn) || Word.HasDes(OCls))
			CountOfBrackets++;

	};
	/// if (CountOfCommas == 1) return 0; 
	//   because of  "Der Dienst aller im Inland befindlichen Stucke, also auch der nicht zertifizierten wird geleistet."
	if (CountOfBrackets == 1) return 0;
	
	if	(		!bEnclosed
			&&	GetOpt()->GetGramTab()->is_morph_pronoun(SubjUnit.m_iPoses)
			&&	(Subj < Pred)
		)
		{
			// "das" und "es" before the predicate are the most frequent subject
			// this "return" makes  "das" a subject  in the following sentence:
			// "das ist der Plan"
			return MaxClauseLength+2;
		};

	if	(		!bEnclosed
			&&	PredHom.IsLemma("SEIN") 
			&&	(Subj < Pred)
		)
		{
			// if predicate is "SEIN" then the subject before predicate is the most preferable
			// for example "Peter ist ein Verbrecher". Though "Verbrecher" is less ambiguous, "Peter" is
			// the real subject 
			return MaxClauseLength+2;
		};


	if (	(SubjUnit.m_iGrammems & gAllCases) == _QM(gNominativ))  //  the best case
		return MaxClauseLength+1;

	if (!( SubjUnit.m_iGrammems & _QM(gNominativ) )) //  it should be checked because 
										 //  for similar nouns it is the only check
		return 0;
	

	if (Subj < Pred)
		return MaxClauseLength-Subj;
	else
		return MaxClauseLength - (Subj-Pred);
};

bool CGerSentence::CheckLastPredicatePosition(size_t ClauseLastWordNo, long RootWordNo) const
{
	//  if this root has an auxiliry verb then get the very left auxiliary verb
	for (long  NextWord = FindFirstAuxVerb(RootWordNo); NextWord != -1; NextWord = FindFirstAuxVerb(NextWord))
		RootWordNo = max(NextWord, RootWordNo);

	if (RootWordNo == ClauseLastWordNo)  return true;
	const CSynWord&  W = m_Words[RootWordNo+1];
	return (		W.HasDes(OPun) // do not use "m_TokenType == PUNCTUAT"  here, m_bPunct does not include a quotation mark
				||	( W.GetHomonymByPOS(gPRP) != -1)
				||	( W.GetHomonymByPOS(gKON) != -1)
			);
};


void CGerSentence::BuildSubjAndPredRelation(CMorphVariant& synVariant,long RootWordNo, EClauseType ClauseType)
{
	synVariant.ResetSubj();

	if (RootWordNo == -1) return;

	vector<int> PossibleAccusativArguments;
	if (		synVariant.m_SynUnits[RootWordNo].HasGrammem(gTransitiv) 
			&&	!synVariant.m_SynUnits[RootWordNo].HasGrammem(gIntransitiv) 
			&&	!synVariant.m_SynUnits[RootWordNo].HasGrammem(gImpersonal) 
		)
	{
		const CSynUnit& PredUnit = synVariant.m_SynUnits[RootWordNo];
		const CSynHomonym&  PredHom = m_Words[PredUnit.m_SentPeriod.m_iFirstWord].m_Homonyms[PredUnit.m_iHomonymNum];
		if (!PredHom.m_bPassive)
			// if the predicate is an active  transitive verb and there is only one NP, which has accustive, or a subclause
			// and this NP coincide with the only subject then do not build the subject.
			for (int WordNo=0; WordNo<synVariant.m_SynUnits.size();WordNo++)
				if (synVariant.m_SynUnits[WordNo].HasGrammem(gAkkusativ))
				{
					int GroupNo = synVariant.m_vectorGroups.get_maximal_group_no(WordNo);
					if	(		(GroupNo == -1)
							||	(synVariant.m_vectorGroups.GetGroups()[GroupNo].m_MainWordNo == WordNo)
						)
						PossibleAccusativArguments.push_back(WordNo);
				}
				else
				if (synVariant.m_SynUnits[WordNo].m_Type == EClause)
				{
					const CSynUnit& U = synVariant.m_SynUnits[WordNo];
					if	(U.m_iClauseTypeNum != -1)
					{
						int SubClauseNo = FindClauseIndexByPeriod(U.m_SentPeriod);
						assert(SubClauseNo != -1);
						const CClause& clause = m_Clauses[SubClauseNo];
						if (		(		(clause.m_vectorTypes[U.m_iClauseTypeNum].m_Type == VERBSATZ_T)
										||	(clause.m_vectorTypes[U.m_iClauseTypeNum].m_Type == INFINITIVSATZ_T)
									)
								&&	!clause.IsRelative()
							)
							PossibleAccusativArguments.push_back(WordNo);
					};
				};
	
	};

	if (ClauseType != VERBSATZ_T) return;
	{
		int AuxVerbWordNo = FindFirstAuxVerb(synVariant.m_SynUnits[RootWordNo].m_SentPeriod.m_iFirstWord);

		if (AuxVerbWordNo != -1)
		{			
			{
				int z = FindFirstAuxVerb(AuxVerbWordNo);
				if (z != -1)
					AuxVerbWordNo = z;
			}

			if (CPeriod(AuxVerbWordNo).is_part_of(CPeriod(synVariant.m_SynUnits[0].m_SentPeriod.m_iFirstWord, synVariant.m_SynUnits.back().m_SentPeriod.m_iLastWord)))
				RootWordNo = synVariant.UnitNoByWordNo(AuxVerbWordNo);
		};

	};


	// predicate should be host of itself 
	{
		int GroupNo = synVariant.m_vectorGroups.get_maximal_group_no(RootWordNo);
		if	(		(GroupNo != -1)
				&&	(synVariant.m_vectorGroups.GetGroups()[GroupNo].m_MainWordNo != RootWordNo)
			)
		return;
	};

	
	int ClauseNo = FindClauseIndexByPeriod(synVariant.GetSentPeriod());
	assert(ClauseNo != -1);

	// the predicate in a subclause should be at the last position, 
	// let alone punctuations or preposition
	bool bEnclosedClause = IsEnclosedClause(ClauseNo);

	int SubjUnitNo = 0;
	int MaxWeight = 0;
	for (int WordNo=0; WordNo<synVariant.m_SynUnits.size();WordNo++)
		if (RootWordNo != WordNo)
		{
			int Weight = get_weight_of_subj_pred_relation(synVariant, WordNo, RootWordNo, bEnclosedClause);
			if	(		(PossibleAccusativArguments.size() != 1)
					||	(PossibleAccusativArguments[0] != WordNo)
				)
			if (MaxWeight < Weight) 
			{
				MaxWeight = Weight;
				SubjUnitNo =  WordNo;

			};
		};

	

	if (MaxWeight > 0)
	{
			synVariant.m_Subjects.push_back( SubjUnitNo );
			synVariant.m_iPredk = RootWordNo;
			synVariant.m_bGoodSubject = true;

			// if the subject is a relative word and the next NP can also be a subject
			//  then add the next NP to the possible subjects
			if (synVariant.GetUnitFirstWord(SubjUnitNo) == m_Clauses[ClauseNo].m_RelativeWord.m_WordNo)
			{
				int GroupNo = synVariant.m_vectorGroups.get_maximal_group_no(SubjUnitNo);
				int next_word = synVariant.m_vectorGroups.get_next_main_word(SubjUnitNo);
				if (next_word != synVariant.m_SynUnits.size()) 
					if ( get_weight_of_subj_pred_relation(synVariant, next_word, RootWordNo, bEnclosedClause) > 0)
						synVariant.m_Subjects.push_back( next_word );
					else
						if (synVariant.m_SynUnits[next_word].HasPos(gPRONOMEN))
						{
							//  if the next word is a pronoun and it cannot be a subject then pass to the next chunk
							next_word = synVariant.m_vectorGroups.get_next_main_word(next_word);
							if (next_word != synVariant.m_SynUnits.size()) 
								if ( get_weight_of_subj_pred_relation(synVariant, next_word, RootWordNo, bEnclosedClause) > 0)
									synVariant.m_Subjects.push_back( next_word );
						};
			};
	};
	

	// change grammmems of the subject, if there is only one hypothesis
	if (synVariant.m_Subjects.size() == 1)
	{
		CSynUnit& SubjUnit = synVariant.m_SynUnits[synVariant.GetFirstSubject()];		
		QWORD Grammems = (SubjUnit.m_iGrammems & ~gAllCases) | _QM(gNominativ);
        SubjUnit.ModifyGrammems( Grammems );
	}

}



