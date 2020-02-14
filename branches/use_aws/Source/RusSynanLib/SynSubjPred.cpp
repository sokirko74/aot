// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#include "StdSynan.h"
#include "RusFormatCaller.h"
#include "RusSentence.h"



//	��������� ���, ���� ������� ���������� �� ����. ���������
//  1. ���� ��� ������ � ������������ ������
//  2. ���� ��� ����� �� ����������
//  3. ���� ����������� �������������� 
//  ��� ���� ����. �� ������ ���� ��������  ������ "���"

bool	CRusSentence::IsGoodSubject(const CMorphVariant& synVariant, const string& LastSentenceChar) const
{
	// ��������� ����� �������
	if (synVariant.m_Subjects.size() != 1) return false;

	if	(	   (synVariant.m_iPredk == -1) 
			|| !synVariant.HasSubj() 
		)
	return false;

	const CSynUnit& PredUnit = synVariant.m_SynUnits[synVariant.m_iPredk];
	const CSynHomonym& PredHom = m_Words[PredUnit.m_SentPeriod.m_iFirstWord].m_Homonyms[PredUnit.m_iHomonymNum];

	//  ��� ����� "���� �� ���!"
	if(!( PredHom.m_iGrammems & rAllTimes ))
		if (!( PredHom.m_iPoses & (1<<ADJ_SHORT)) )
			return false;

	const CSynUnit& SubjUnit = synVariant.m_SynUnits[synVariant.GetFirstSubject()];
	const CSynWord& SubjWord = m_Words[SubjUnit.m_SentPeriod.m_iFirstWord];
	const CSynHomonym& SubjHom = SubjWord.m_Homonyms[SubjUnit.m_iHomonymNum];

	if( SubjHom.IsLemma("���") )
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
������� ��� ���������� ����������� � ������ � ���� ���� 
  "���� - �����",
���  "����" - ����������,
 �   "�����" - ���������.
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
 ������� can_be_subject ���������, ����� �� ����� WordNo ���� ����������. 
 �������� exactitude ���������� ��������� �������, ������� ������ 
 ������������� ����������.
 �������� bCanBeWhatAsSubj ����������, ��� ����� "���" ����� ��������� ���� ����������� 
 ��������: "� ����, ��� ���������."
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
		���� ����� ���������� �  ��������� ��� ����. �����, �����
		����� ���������� � ��������� �� ����� ���� ������ ����� �������, 
		������� �� ����� � ������.
		"�����, ����  ������."  - ����� ������� ����� � ������
		��� ����� ����
		� �����:
		"����, ������"  - ������ ���� �������
		
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
	 � ����������� ������� ���������� ����� ��������� ���������� �����, ��������� ���������� ������ 
	 ��� ��������� � �� ���� ���� ��������� ����������� ����������.
	 ������, ���������� ���������� ����� - ��� �����.
	*/
	grammems &= ~_QM(rLocativ);
	if ( (grammems & rAllCases) == 0 ) 
		return false;
	

	/*
	 ���� ������� ��������� ������ ���� ����� 3, �
	 ������. ������ �� �������� ���. ���������������,
	 ����� ���� ��� ���� �� �������� ������������ ���, ���� ������ � ������� ���������,
	 ���� ����� � �����.
	 ������, "�������� ������", ����� "��������" ����� ����� 
	 ���������� ������ ��� ������ ��������� 4, ��������� "��������" �� syn_noun.
	*/
	if (    (exactitude <= 3) 
	    && !SubjHom.IsSynNoun() 
	   )
	{
			return false;
	};

	bool hasprep = false;
	//�� ������� ����������, ������� ������ ����� ��� ������ �� ���������� 
	{
		int iGrCount = 0;
		for ( int tt = synVariant.m_vectorGroups.get_main_word(RootWordNo); tt < SubjWordNo; tt = synVariant.m_vectorGroups.get_next_main_word(tt))
		{
			const CSynUnit& U = synVariant.m_SynUnits[tt];
			if( tt == SubjWordNo - 1 && (U.HasPos(PREP)))
				hasprep = true;
			if (U.m_Type == EClause) continue;
			const CSynWord& W = m_Words[U.m_SentPeriod.m_iFirstWord];
			const CSynHomonym& H = W.m_Homonyms[U.m_iHomonymNum];
			if ( H.IsSynNoun() ) 
					iGrCount++;
		}				
		if ( iGrCount >= 2 ) return false;
	}

	// ������ a)
	// "� ����, ��� ������", "���" - ������������� ������� �����, ����� ���� ������ ����.
	if (strWord == "���")
		return true;


	switch( exactitude )
	{
		// ������ b)
		case 0:
			{
				// ��� �� ������ ������, ����� ����������
				// � ���� ��� �������� ���� 
				// "��", "�" ...
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
		// ������ c) 
		case 1:
			{
				/*
				  ��� �� ������ ������, ����� ����������, � 
				  ����� �� ����� "���"
				  "����", "����"
				*/
				if(			!(grammems & (rAllCases & ~_QM(rNominativ)))
						&&	(strWord !="���")
					)
					return true;
				else
					break;
			}
			// ������ d) e) 
		case 2:
			{
				/*
				  ��� �� ������ ������, ����� ���������� � ����������, � 
				  ����� �� ����� "���"
				  "����", "���"
				*/
				if(	    ( (grammems & rAllCases)  == ( _QM(rNominativ) | _QM(rAccusativ)) ) 
					&&	(strWord != "���") 
				   )
					return true;
				else
					break;
			}
			// ������ f)   g)
		case 3:
			{
				/*
				 ���� ���� ���������
				*/
				if(grammems & _QM(rNominativ)) 
				{
					/*
					 ���� ��������� ����������� ���� "���", � SubjWordNo -
					 ���� "���", ����� ���� ����� 
					*/
					if  (    !bCanBeWhatAsSubj
						  && (strWord == "���") 
						)
						break;
					else
						return true;
				}
				else
					break;
			}
			// ������ h)   i)
		case 4:
			{
				// ���� ���� ���������
				if(	grammems & _QM(rNominativ)) 
					if( gr_num != -1 )
					{
						int GroupType = synVariant.m_vectorGroups.GetGroups()[gr_num].m_GroupType;
						// ���� ����� ������ � ������, ����� ������ �������� ������ 
						// ����� ���� ����������, ��������  
						if(		( GroupType == MODIF_ADJ		)
							// "����� ��������"
							||	( GroupType == NUMERAL_ADVERB	) 
							// "��������� �������"
							||	( GroupType == NUMERAL_NOUN	) 
							// "��� ��������"
							||	( GroupType == NOUN_NUMERAL_APPROX))
							// "�������� ���"
							return true;
					}
					else
					// ���� ����� �� ������ � ������, ����� ������ �������� ����� ����
					// ����� ���� ����������, ��������  
                    if(		 (SubjHom.HasPos(NUMERAL_P ) 
						||	 CanNumeralBeNoun(SubjHom.m_strLemma.c_str() ) 
						||	 SubjHom.HasPos( ADJ_FULL	) ) && !hasprep //������� � 1901 �� 1921 ����
					  )
					return true;

				break;
			}
	}
	return false;
}

/*
  ���� ����� ���������� � ��������� �� ���������  GleicheSubjectPredicate,
  ��� ���������� �� ����� "��������� ������� ������", ���  "�������" 
  �� ����������� � "������", ����� ���������� ��� ���������, � �� ���� ���
  ���������� ����, ������ ������������ ���.
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
			 ��� ���� ����� ����� ��������� ������ ���� � ������, � �� � �������� �����
			*/
			if( !(group.GetGrammems() & _QM(rNominativ)) )
				return false;
			// "��������� ��������� ������"
			if(	PredHom.HasGrammem(rSingular) &&
				PredHom.HasGrammem(rNeutrum)  &&
				PredHom.HasGrammem(rPastTense)  )
				return true;

			// "��������� ��������� ������"
			if(	PredHom.HasGrammem(rPlural) &&
				PredHom.HasGrammem(rPastTense))
				return true;

			// "��������� ��������� ��������"
			// "��������� ��������� ����� ���������"
			if(	   PredHom.HasGrammem(rSingular) 
				&& PredHom.HasGrammem(rThirdPerson)  
				&& (    PredHom.HasGrammem(rPresentTense)
				    ||  PredHom.HasGrammem(rFutureTense)
				   )
			  )
				return true;

			// "��������� ��������� ��������"
			// "��������� ��������� ����� ���������"
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
 ������������ ��� ����������s� "���", "����" ..
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
 ��������� ������������, ����� ���������  - ������ ����� �������
 SubjGroupFirstWordNo - ������ ����� ���������� ������
 SubjWordNo - ������� ����� ���������� ������ (���������)
*/
bool CRusSentence::check_verb_subj_coordination(const CMorphVariant& synVariant, int predk, int SubjGroupFirstWordNo, int& SubjWordNo) const
{
	const CSynUnit& PredUnit = synVariant.m_SynUnits[predk];
	const CSynHomonym& PredHom = m_Words[PredUnit.m_SentPeriod.m_iFirstWord].m_Homonyms[PredUnit.m_iHomonymNum];

    int gr_num = synVariant.m_vectorGroups.get_maximal_group_no(SubjGroupFirstWordNo);
    int main_word = (gr_num == -1) ? SubjGroupFirstWordNo : synVariant.m_vectorGroups.GetGroups()[gr_num].m_MainWordNo;
	const CSynUnit& SubjUnit = synVariant.m_SynUnits[main_word];
	const CSynHomonym& SubjHom = m_Words[SubjUnit.m_SentPeriod.m_iFirstWord].m_Homonyms[SubjUnit.m_iHomonymNum];


	if(    GetOpt()->GetGramTab()->GleicheSubjectPredicate(SubjHom.GetGramCodes().c_str(), PredUnit.GetGramCodes().c_str() )	
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
					//nim : "��� ������ �������� ����"
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
 ��������� ������������, ����� ���������  - ������� �������������� ��� �����. ����� 
 SubjGroupFirstWordNo - ������ ����� ���������� ������
 SubjWordNo - ������� ����� ���������� ������ (���������)
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
	  ���� �������������� ����� � ������������� �����,����� ������������ ��������
	  ���. ��������:
	   "� �������� ����."
	   ������ ���� ���������, ��� ���������� ����� �� ����������
	   ����� ����� "�������" ���������� ���������� �� �����  
	   "� ���� ��� ������ �������." 
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
		if(		GetOpt()->GetGramTab()->GleicheGenderNumber(SubjHom.GetGramCodes().c_str(), PredHom.GetGramCodes().c_str()) 
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
 ������� ���������� ��� ������ ����� �������
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
	  bCanBeWhatAsSubj = "����� �� "���" ���� ����������"
	  ����� ��������  ������������ � ���, ��� "���" ����� ���� ������ �����������
	  ��������, "� ����, ��� �� ������", ������� ���� ���������� ������, �����
	  "���" ��������� ���� ����������, � ��� ��������� �������� �� ���������. 
	*/
	bool bCanBeWhatAsSubj =		
								// "� ����, ��� ������� �� ���" 
								(PredHom.HasGrammem(rNonTransitive))
								// "� ����, ��� ������� ���� �������" 

							||	(		(gr_num != -1)
									&&	(synVariant.m_vectorGroups.GetGroups()[gr_num].m_GroupType == DIR_OBJ)
								);

	/*
	 ������� �������:
	 ���������� �������� ����� "��������", "���������" � 3�/��  � ����� ��� ���� ��
	 � ��������� ������� � ��� �������������, ����� �� ������ ������ �����������
	 ��������:
	 "��� �������, ��� ���� ���� " + 
	 "����  �������, ��� �� ����" +		
	 "���� �������  ����� " - (� ����� "����"  ��� ������)
	 "���� ������� � ����� " - (� ����� "����"  ��� ������)
	 ��� �����, ����� �������� ������������� ������������ �� ������ �����.
	*/
	if (   PredHom.IsLemma("���������") 
		|| PredHom.IsLemma("��������") 
	   )
	   if (    (PredHom.m_iGrammems  & (_QM(rThirdPerson) | _QM(rSingular) ))//"�������"
		    || (PredHom.m_iGrammems  & _QM(rNeutrum)) //"��������"
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

					if	(		  (grammems & _QM(rDativ)) // "���"
							&&  !(grammems & _QM(rNominativ))  // �� �� "����"!
						)
						return false;
			   };
		   };
	// ���������� ������� �����������

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
	������ �������� ����������� � ������, � ������� ��� ������ ������ ����� ����,
	��� ��� ������ ��� ��������� (����� �������� ����������� � �������� �����), �.�.
	����� ���������� ��� ������� ������������.
	������: "����, ��� �� ���� ���������� ������, ����������� ���� ���� ����"
	��� �������������� ������������ ���������� ���������� "����", � ����� ����������� ����� "����",
	��������� "����" ��������� �����, � �� ������. 
	���� �������� �������� ������, ����� ����������  ��. �����  � "����".    
	������ ������ ������� ���������� � �-��� CSentence::ChangeSubjAndItsGroupGrammems()
	*/
	synVariant.ResetSubj();
	synVariant.m_iPredk = predk;
	if (SubjUnitNo != -1)
	{
		synVariant.m_Subjects.push_back( SubjUnitNo );
		return true;
	};
	return false;
}

