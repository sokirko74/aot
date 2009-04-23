// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#include "StdSynan.h"
#include "RusFormatCaller.h"
#include "RusSentence.h"



//	Добавляем вес, если найдено подлежащее со след. условиями
//  1. либо оно только в именительном падеже
//  2. либо оно стоит до сказуемого
//  3. либо предложение вопросительное 
//  при этом подл. не должно быть выражено  леммой "что"

bool	CRusSentence::IsGoodSubject(const CMorphVariant& synVariant, const string& LastSentenceChar) const
{
	// попробуем такой вариант
	if (synVariant.m_Subjects.size() != 1) return false;

	if	(	   (synVariant.m_iPredk == -1) 
			|| !synVariant.HasSubj() 
		)
	return false;

	const CSynUnit& PredUnit = synVariant.m_SynUnits[synVariant.m_iPredk];
	const CSynHomonym& PredHom = m_Words[PredUnit.m_SentPeriod.m_iFirstWord].m_Homonyms[PredUnit.m_iHomonymNum];

	//  для фразы "Боже ты мой!"
	if(!( PredHom.m_iGrammems & rAllTimes ))
		if (!( PredHom.m_iPoses & (1<<ADJ_SHORT)) )
			return false;

	const CSynUnit& SubjUnit = synVariant.m_SynUnits[synVariant.GetFirstSubject()];
	const CSynWord& SubjWord = m_Words[SubjUnit.m_SentPeriod.m_iFirstWord];
	const CSynHomonym& SubjHom = SubjWord.m_Homonyms[SubjUnit.m_iHomonymNum];

	if( SubjHom.IsLemma("ЧТО") )
		return false;


	return true;

};

void CRusSentence::BuildSubjAndPredRelation(CMorphVariant& synVariant, long RootWordNo, EClauseType ClauseType)
{
	synVariant.ResetSubj();

	if( ClauseType == DASH_T )
		find_subj_and_predic_noun_with_dash(synVariant) ;
	else 
		if(		(ClauseType == PARTICIPLE_SHORT_T)
			||  (ClauseType == ADJ_SHORT_T) 
			||  (ClauseType == COMPARATIVE_T) 
			||  (ClauseType == VERB_PERS_T)
			)
		find_subj(synVariant, RootWordNo);

	synVariant.m_bGoodSubject = IsGoodSubject(synVariant, m_Words.back().m_strWord);
}



/*
функция для построения подлежащего в клаузе с тире типа 
  "Иван - дурак",
где  "Иван" - подлежащее,
 а   "дурак" - сказуемое.
*/

bool CRusSentence::find_subj_and_predic_noun_with_dash(CMorphVariant& synVariant)
{
	bool found_dash = false;
	int subj = -1, predk = -1;
	int subj_noun = -1, subj_verb = -1, predk_noun = -1, predk_verb = -1;

	for(int UnitNo = synVariant.m_vectorGroups.get_main_word(0) ; UnitNo < synVariant.m_SynUnits.size(); UnitNo = synVariant.m_vectorGroups.get_next_main_word(UnitNo))
	{
		const CSynUnit& U = synVariant.m_SynUnits[UnitNo];
		if ( U.m_Type == EClause )
			continue; 

		const CSynWord& W = m_Words[U.m_SentPeriod.m_iFirstWord];
		const CSynHomonym& H = W.m_Homonyms[U.m_iHomonymNum];

		if( W.m_bDash )
		{
			found_dash = true;
			continue;
		};

		size_t GroupNo = synVariant.m_vectorGroups.get_maximal_group_no(UnitNo);
		QWORD Grammems = H.m_iGrammems;
		if (GroupNo != -1)
			Grammems = synVariant.m_vectorGroups.GetGroups()[GroupNo].GetGrammems();


		if	(		(		Grammems & _QM(rNominativ)
						&&	H.IsSynNoun()
					)
                    ||	 H.HasPos(INFINITIVE )
			)
		{
			if(!found_dash)
				subj_noun = UnitNo;
			else 
				if(predk_noun == -1)					
					predk_noun = UnitNo;
		};
	}

	synVariant.ResetSubj();
	synVariant.m_iPredk = -1;

	if	((subj_noun != -1)  &&  (predk_noun != -1)) 
	{
		synVariant.m_Subjects.push_back (subj_noun);
		synVariant.m_iPredk = predk_noun;
	}
	else
		if ((subj_verb != -1)  &&  (predk_verb != -1))
		{
			synVariant.m_Subjects.push_back( subj_verb) ;
			synVariant.m_iPredk = predk_verb;
		}
		else
				return false;

	return true;
}

/*
 функция can_be_subject проверяет, может ли слово WordNo быть подлежащим. 
 Параметр exactitude определяет жесткость условий, которым должно 
 удовлетворять подлежащее.
 параметр bCanBeWhatAsSubj обозначает, что слово "что" моежт выполнять роль подлежащего 
 например: "я знаю, что случилось."
*/
bool CRusSentence::can_be_subject(const CMorphVariant& synVariant, int SubjWordNo, int RootWordNo, int exactitude,  bool bCanBeWhatAsSubj) const
{

	if (SubjWordNo == RootWordNo) return false;

    int gr_num = synVariant.m_vectorGroups.get_maximal_group_no(SubjWordNo);
    int i_main_word = (gr_num == -1) ? SubjWordNo : synVariant.m_vectorGroups.GetGroups()[gr_num].m_MainWordNo;
	if (i_main_word != SubjWordNo) return false;

	const CSynUnit& SubjUnit = synVariant.m_SynUnits[i_main_word];
	if (SubjUnit.m_Type == EClause)  return false;
	const CSynWord& SubjWord = m_Words[SubjUnit.m_SentPeriod.m_iFirstWord];
    if ( SubjWord.HasDes(OPun)  ) return false;

	const CSynHomonym& SubjHom = SubjWord.m_Homonyms[SubjUnit.m_iHomonymNum];
	const string& strWord = SubjWord.m_strUpperWord;

	if( gr_num != -1 )
		if( synVariant.m_vectorGroups.GetGroups()[gr_num].m_GroupType == PREP_NOUN )
			return false;
	/*
		Если между подлежащим и  сказуемым нет подч. калуз, тогда
		между подлежащим и сказуемым не может быть только одной запятой, 
		которая не вошла в группы.
		"Мария, Петр  пришли."  - здесь запятая вошла в группу
		Две могут быть
		А здесь:
		"Петр, пришел"  - только одна запятая
		
	*/
	int MinWordNo = SubjWordNo;
	int MaxWordNo = RootWordNo;
	int CountOfCommmas = 0; 
	if (MaxWordNo <  MinWordNo) swap (MaxWordNo, MinWordNo);
	for (long WordNo=MinWordNo; WordNo < MaxWordNo; WordNo++)
	{
		const CSynUnit& U = synVariant.m_SynUnits[WordNo];
		const CSynWord& W = m_Words[U.m_SentPeriod.m_iFirstWord];
		if (	(		W.m_bComma
					&&  (synVariant.m_vectorGroups.get_maximal_group_no(WordNo) == -1)
				 )
				 || (U.m_Type == EClause)
		   )
		   CountOfCommmas++;
	};
				

	if (CountOfCommmas == 1) 
		return false;


	QWORD grammems = SubjHom.m_iGrammems;
	if (gr_num != -1)
		grammems = synVariant.m_vectorGroups.GetGroups()[gr_num].GetGrammems();
		
		
	/*
	 у проверяемых граммем сказуемого можно отключить предложный падеж, поскольку предложные группы 
	 уже построены и им выше было запрещено становиться подлежащим.
	 Значит, оставшийся предложный падеж - это мусор.
	*/
	grammems &= ~_QM(rLocativ);
	if ( (grammems & rAllCases) == 0 ) 
		return false;
	

	/*
	 если уровень жесткости меньше либо равен 3, и
	 потенц. подлеж не является син. существительным,
	 тогда если оно либо не является числительным ОБА, либо входит в большую подгруппу,
	 надо выйти с ложью.
	 Короче, "красивый пришел", здесь "красивый" может стать 
	 подлежащим только при уровне жесткости 4, поскольку "красивый" не syn_noun.
	*/
	if (    (exactitude <= 3) 
	    && !SubjHom.IsSynNoun() 
	   )
	{
			return false;
	};

	//не строить подлежащее, стоящее справа через две группы от сказуемого 
	{
		int iGrCount = 0;
		for ( int tt = synVariant.m_vectorGroups.get_main_word(RootWordNo); tt < SubjWordNo; tt = synVariant.m_vectorGroups.get_next_main_word(tt))
		{
			const CSynUnit& U = synVariant.m_SynUnits[tt];
			if (U.m_Type == EClause) continue;
			const CSynWord& W = m_Words[U.m_SentPeriod.m_iFirstWord];
			const CSynHomonym& H = W.m_Homonyms[U.m_iHomonymNum];
			if ( H.IsSynNoun() ) 
					iGrCount++;
		}				
		if ( iGrCount >= 2 ) return false;
	}

	// случай a)
	// "я знаю, кто пришел", "кто" - неомонимичное союзное слово, может быть только подл.
	if (strWord == "КТО")
		return true;


	switch( exactitude )
	{
		// случай b)
		case 0:
			{
				// нет ни одного падежа, кроме номинатива
				// и есть еще граммемы лица 
				// "он", "я" ...
				if(		(		(grammems & _QM(rFirstPerson))
							||	(grammems & _QM(rSecondPerson))
							||	(grammems & _QM(rThirdPerson))
						)
					&&	!(grammems & rAllCases & ~_QM(rNominativ) )
				  )
					return true;
				else
					break;
			}
		// случай c) 
		case 1:
			{
				/*
				  нет ни одного падежа, кроме номинатива, и 
				  слово не равно "ЧТО"
				  "папа", "мама"
				*/
				if(			!(grammems & (rAllCases & ~_QM(rNominativ)))
						&&	(strWord !="ЧТО")
					)
					return true;
				else
					break;
			}
			// случай d) e) 
		case 2:
			{
				/*
				  нет ни одного падежа, кроме номинатива и аккузатива, и 
				  слово не равно "ЧТО"
				  "стол", "дуб"
				*/
				if(	    ( (grammems & rAllCases)  == ( _QM(rNominativ) | _QM(rAccusativ)) ) 
					&&	(strWord != "ЧТО") 
				   )
					return true;
				else
					break;
			}
			// случай f)   g)
		case 3:
			{
				/*
				 если есть номинатив
				*/
				if(grammems & _QM(rNominativ)) 
				{
					/*
					 если запрещено подлежащему быть "что", а SubjWordNo -
					 есть "что", тогда надо выйти 
					*/
					if  (    !bCanBeWhatAsSubj
						  && (strWord == "ЧТО") 
						)
						break;
					else
						return true;
				}
				else
					break;
			}
			// случай h)   i)
		case 4:
			{
				// если есть номинатив
				if(	grammems & _QM(rNominativ)) 
					if( gr_num != -1 )
					{
						int GroupType = synVariant.m_vectorGroups.GetGroups()[gr_num].m_GroupType;
						// если слово входит в группу, тогда только указаные группы 
						// могут быть подлежащим, например  
						if(		( GroupType == MODIF_ADJ		)
							// "очень красивый"
							||	( GroupType == NUMERAL_ADVERB	) 
							// "несколько человек"
							||	( GroupType == NUMERAL_NOUN	) 
							// "два человека"
							||	( GroupType == NOUN_NUMERAL_APPROX))
							// "человека два"
							return true;
					}
					else
					// если слово не входит в группу, тогда только указаные части речи
					// могут быть подлежащим, например  
                    if(		 SubjHom.HasPos(NUMERAL_P )
						||	 CanNumeralBeNoun(SubjHom.m_strLemma.c_str() ) 
						||	 SubjHom.HasPos( ADJ_FULL	) 
					  )
					return true;

				break;
			}
	}
	return false;
}

/*
  если между подлежащим и сказуемым не сработала  GleicheSubjectPredicate,
  что происходит во фразе "несколько человек пришло", где  "человек" 
  не согласуется с "пришло", тогда включается эта процедура, и уж если она
  возвращает ложь, значит согласования нет.
*/
bool CRusSentence::GleicheSubjPredForNumeralAndSimilar(const CMorphVariant& synVariant, int predk, int iGroup) const
{
	if (iGroup == -1) return false;
	const CGroup& group = synVariant.m_vectorGroups.GetGroups()[iGroup];

	const CSynUnit& PredUnit = synVariant.m_SynUnits[predk];
	const CSynHomonym& PredHom = m_Words[PredUnit.m_SentPeriod.m_iFirstWord].m_Homonyms[PredUnit.m_iHomonymNum];

	switch( group.m_GroupType)
	{
		case NOUN_NUMERAL_APPROX:
		case NUMERAL_NOUN:
		case NUMERAL_ADVERB:

		{
			/*
			 для этих типов групп номинатив должен быть у группы, а не у главного слова
			*/
			if( !(group.GetGrammems() & _QM(rNominativ)) )
				return false;
			// "несколько мальчиков пришло"
			if(	PredHom.HasGrammem(rSingular) &&
				PredHom.HasGrammem(rNeutrum)  &&
				PredHom.HasGrammem(rPastTense)  )
				return true;

			// "несколько мальчиков пришли"
			if(	PredHom.HasGrammem(rPlural) &&
				PredHom.HasGrammem(rPastTense))
				return true;

			// "несколько мальчиков приходит"
			// "несколько мальчиков будет приходить"
			if(	   PredHom.HasGrammem(rSingular) 
				&& PredHom.HasGrammem(rThirdPerson)  
				&& (    PredHom.HasGrammem(rPresentTense)
				    ||  PredHom.HasGrammem(rFutureTense)
				   )
			  )
				return true;

			// "несколько мальчиков приходят"
			// "несколько мальчиков будут приходить"
			if(	   PredHom.HasGrammem(rPlural) 
				&& PredHom.HasGrammem(rThirdPerson) 
				&& (    PredHom.HasGrammem(rPresentTense)
				    ||  PredHom.HasGrammem(rFutureTense)
				   )
			   )
				return true;

			break;
		}
		case SIMILAR_NOUN_GROUPS:
		{
			if( PredHom.HasGrammem(rPlural) )	
				return true;
			break;
		}
	}

	return false;
}

/*
 согласование для числительнsь "ОБА", "двое" ..
*/
bool CRusSentence::gleiche_subj_pred_for_numerals_as_nouns(const CMorphVariant& synVariant, int predk, int i_subj) const
{
	const CSynUnit& PredUnit = synVariant.m_SynUnits[predk];
	const CSynHomonym& PredHom = m_Words[PredUnit.m_SentPeriod.m_iFirstWord].m_Homonyms[PredUnit.m_iHomonymNum];

	int main_word = synVariant.m_vectorGroups.get_main_word(i_subj);
	const CSynUnit& SubjUnit = synVariant.m_SynUnits[main_word];
	const CSynHomonym& SubjHom = m_Words[SubjUnit.m_SentPeriod.m_iFirstWord].m_Homonyms[SubjUnit.m_iHomonymNum];

	if( CanNumeralBeNoun(SubjHom.m_strLemma.c_str()) )
		if( SubjHom.HasGrammem(rNominativ) && 
			PredHom.HasGrammem(rPlural) && 
			!PredHom.HasGrammem(rFirstPerson) &&
			!PredHom.HasGrammem(rSecondPerson)) 
			return true;

	return false;
}


/*
 проверяет согласование, когда сказуемое  - личная форма глагола
 SubjGroupFirstWordNo - первое слово субъектной группы
 SubjWordNo - главное слово субъектной группы (результат)
*/
bool CRusSentence::check_verb_subj_coordination(const CMorphVariant& synVariant, int predk, int SubjGroupFirstWordNo, int& SubjWordNo) const
{
	const CSynUnit& PredUnit = synVariant.m_SynUnits[predk];
	const CSynHomonym& PredHom = m_Words[PredUnit.m_SentPeriod.m_iFirstWord].m_Homonyms[PredUnit.m_iHomonymNum];

    int gr_num = synVariant.m_vectorGroups.get_maximal_group_no(SubjGroupFirstWordNo);
    int main_word = (gr_num == -1) ? SubjGroupFirstWordNo : synVariant.m_vectorGroups.GetGroups()[gr_num].m_MainWordNo;
	const CSynUnit& SubjUnit = synVariant.m_SynUnits[main_word];
	const CSynHomonym& SubjHom = m_Words[SubjUnit.m_SentPeriod.m_iFirstWord].m_Homonyms[SubjUnit.m_iHomonymNum];


	if(    GetOpt()->GetGramTab()->GleicheSubjectPredicate(SubjHom.m_GramCodes.c_str(), PredUnit.m_GramCodes.c_str() )	
		|| gleiche_subj_pred_for_numerals_as_nouns(synVariant, predk, main_word ) 
	  )
	{
		SubjWordNo = main_word;
		return true;
	}
	else
	{
		if( gr_num != -1 )
			if(	!(		(PredHom.m_iGrammems & _QM(rImperative) )
					||	(PredHom.m_iGrammems  & ( _QM(rFirstPerson) | _QM(rSecondPerson)))
				 )
			  )
			{
					if( GleicheSubjPredForNumeralAndSimilar(synVariant,  predk, gr_num) )
					{
						SubjWordNo = main_word;				
						return true;
					}
					//nim : "все четыре мальчика ушли"
					int MinimalGroupNo = synVariant.m_vectorGroups.get_minimal_group(main_word);

					if( GleicheSubjPredForNumeralAndSimilar(synVariant, predk,  MinimalGroupNo) )
					{
						SubjWordNo = main_word;				
						return true;

					}								
			}
	}
	return false;
};

/*
 проверяет согласование, когда сказуемое  - краткое прилагательное или сравн. форма 
 SubjGroupFirstWordNo - первое слово субъектной группы
 SubjWordNo - главное слово субъектной группы (результат)
*/
bool CRusSentence::check_adj_subj_coordination(const CMorphVariant& synVariant, int predk, int SubjGroupFirstWordNo, int& SubjWordNo) const
{
    int gr_num = synVariant.m_vectorGroups.get_maximal_group_no(SubjGroupFirstWordNo);
    int main_word = (gr_num == -1) ? SubjGroupFirstWordNo : synVariant.m_vectorGroups.GetGroups()[gr_num].m_MainWordNo;
	const CSynUnit& SubjUnit = synVariant.m_SynUnits[main_word];
	const CSynHomonym& SubjHom = m_Words[SubjUnit.m_SentPeriod.m_iFirstWord].m_Homonyms[SubjUnit.m_iHomonymNum];

	const CSynUnit& PredUnit = synVariant.m_SynUnits[predk];
	const CSynHomonym& PredHom = m_Words[PredUnit.m_SentPeriod.m_iFirstWord].m_Homonyms[PredUnit.m_iHomonymNum];

	/*
	  если прилагательное стоит в сравнительной форме,тогда согласования никакого
	  нет. Например:
	   "Я красивее тебя."
	   Однако надо проверить, что подлежащее стоит до сказуемого
	   Иначе слово "надежда" становится подлежащим во фраез  
	   "У меня нет больше надежды." 
	*/
	if (main_word < predk)
	 if (PredHom.m_iGrammems & _QM(rComparative))
		{
			SubjWordNo = main_word;
			return true;
		};

	if	(		(SubjHom.m_iGrammems & _QM(rFirstPerson))
			||	(SubjHom.m_iGrammems & _QM(rSecondPerson))
		)
	{
		if( SubjHom.m_iGrammems & PredHom.m_iGrammems & rAllNumbers)
		{
			SubjWordNo = main_word;
			return true;
		} 
	}else
		if(		GetOpt()->GetGramTab()->GleicheGenderNumber(SubjHom.m_GramCodes.c_str(), PredHom.m_GramCodes.c_str()) 
			|| gleiche_subj_pred_for_numerals_as_nouns(synVariant, predk, main_word ) 
		  )
		{
			SubjWordNo = main_word;
			return true;
		}
		else
		{
			if( gr_num != -1 )
			{
				int GroupType = synVariant.m_vectorGroups.GetGroups()[gr_num].m_GroupType;
				if( GroupType== SIMILAR_NOUN_GROUPS ||
					GroupType == NUMERAL_ADVERB ||
					GroupType == NOUN_ADJ ||
					GroupType == NUMERAL_NOUN)
					if(	PredHom.m_iGrammems & _QM(rPlural) )
					{							
						SubjWordNo = main_word;
						return true;;
					}
			};
		}

	return false;
};
/*
 находит подлежащее для личной формы глагола
*/

bool CRusSentence::find_subj(CMorphVariant& synVariant, int predk) 
{
	if( predk == -1)
		return false;
	{
		int AuxVerbWordNo = FindFirstAuxVerb(synVariant.m_SynUnits[predk].m_SentPeriod.m_iFirstWord);
		if (AuxVerbWordNo != -1)
			predk = synVariant.UnitNoByWordNo(AuxVerbWordNo);

	};

	const CSynUnit& PredUnit = synVariant.m_SynUnits[predk];
	if (PredUnit.m_Type == EClause)  return false;
	const CSynWord& PredWord = m_Words[PredUnit.m_SentPeriod.m_iFirstWord];
	const CSynHomonym& PredHom = PredWord.m_Homonyms[PredUnit.m_iHomonymNum];

	if ( PredUnit.m_Type == EClause ) return false; 

	int gr_num = synVariant.m_vectorGroups.get_maximal_group_no(predk);
	
	
	/*
	  bCanBeWhatAsSubj = "может ли "что" быть подлежащим"
	  Здесь проблема  заключаетсмя в том, что "что" может быть прямым дополнением
	  например, "я знаю, что он сделал", поэтому надо ограничить случае, когда
	  "что" разрешено быть подлежащим, а все остальное оставить на семантику. 
	*/
	bool bCanBeWhatAsSubj =		
								// "я знаю, что пршишло ко мне" 
								(PredHom.HasGrammem(rNonTransitive))
								// "я знаю, что сделало тебя дураком" 

							||	(		(gr_num != -1)
									&&	(synVariant.m_vectorGroups.GetGroups()[gr_num].m_GroupType == DIR_OBJ)
								);

	/*
	 Добавил Сокирко:
	 сказуемыми являются слова "казаться", "следовать" в 3л/ср  и перед ним есть ИГ
	 с дательным падежом и без именительного, тогда не строим группу подлежащего
	 Например:
	 "Вам кажется, что надо уйти " + 
	 "Пете  кажется, что он прав" +		
	 "Леди кажется  дурой " - (у слова "леди"  все падежи)
	 "Леди следует в Париж " - (у слова "леди"  все падежи)
	 Это нужно, чтобы отличить преликативное употребление от личной формы.
	*/
	if (   PredHom.IsLemma("СЛЕДОВАТЬ") 
		|| PredHom.IsLemma("КАЗАТЬСЯ") 
	   )
	   if (    (PredHom.m_iGrammems  & (_QM(rThirdPerson) | _QM(rSingular) ))//"кажется"
		    || (PredHom.m_iGrammems  & _QM(rNeutrum)) //"казалось"
		  )
		   {
			   for (int k=0; k<predk; k++)
			   {

					const CSynUnit& U = synVariant.m_SynUnits[k];
					if (U.m_Type == EClause) continue;
					const CSynHomonym& H = m_Words[U.m_SentPeriod.m_iFirstWord].m_Homonyms[U.m_iHomonymNum];
				   	QWORD grammems = H.m_iGrammems;
					int GroupNo = synVariant.m_vectorGroups.get_minimal_group(k);
					if (GroupNo != -1)
						grammems = synVariant.m_vectorGroups.GetGroups()[GroupNo].GetGrammems();

					if	(		  (grammems & _QM(rDativ)) // "вам"
							&&  !(grammems & _QM(rNominativ))  // но не "леди"!
						)
						return false;
			   };
		   };
	// добавление Сокирко закончилось

	int SubjUnitNo = -1;
	for(size_t exactitude = 0; exactitude < 5;  exactitude++)
	{
		int i = synVariant.m_vectorGroups.get_main_word(0) ;
		for(; i < synVariant.m_SynUnits.size() ; i = synVariant.m_vectorGroups.get_next_main_word(i))
			if	(	can_be_subject(synVariant, i,predk, exactitude,bCanBeWhatAsSubj) )	
			{
				bool bCoordination;

                if (	 PredHom.HasPos(ADJ_SHORT)
                     ||  PredHom.HasPos(PARTICIPLE_SHORT)
					 ||	( PredHom.m_iGrammems & _QM(rComparative))
					)
					bCoordination = check_adj_subj_coordination(synVariant, predk, i, SubjUnitNo);
				else
					bCoordination = check_verb_subj_coordination(synVariant, predk, i, SubjUnitNo);

				if (bCoordination)
					break;
			}


		if (i < synVariant.m_SynUnits.size()) break;
	}

	/*
	Менять граммемы подлежащего и группы, в которую оно входит только после того,
	как все клаузы уже построены (после операций объединения и вложения клауз), т.е.
	когда отработают все правила фрагментации.
	пример: "Стол, что за нами пристально следит, преследовал меня весь день"
	при первоначальной фрагментации подлежащим становился "день", а после объединения клауз "стол",
	поскольку "стол" находится слева, а не справа. 
	Если поменять граммемы сейчас, тогда потеряется  вн. падеж  у "день".    
	Теперь чистка граммем происходит в ф-ции CSentence::ChangeSubjAndItsGroupGrammems()
	*/
	synVariant.ResetSubj();
	if (SubjUnitNo != -1)
	{
		synVariant.m_Subjects.push_back( SubjUnitNo );
		synVariant.m_iPredk = predk;
		return true;
	};
	return false;
}

