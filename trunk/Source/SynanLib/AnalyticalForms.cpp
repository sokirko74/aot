// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#include "StdSynan.h"
#include "RusWord.h"
#include "RusSentence.h"
#include "assert.h"
#undef NDEBUG
#include "RusFormatCaller.h"


bool HasDeclinableSynNounInInstrumentalis(const CSynWord& _W)
{
	for (int i = 0; i < _W.m_Homonyms.size(); i++)
		if ( _W.m_Homonyms[i].IsSynNoun() ) 
		 if (    _W.m_Homonyms[i].HasGrammem(rInstrumentalis) 
			 && !_W.m_Homonyms[i].HasGrammem(rNominativ) 
			)
				return true;
		
	return false; 
}


bool CheckComparativeZAPLATAForAnalyticalForm (const CRusSentence& S,long WordNo, long ClauseNo)
{
	for (long i=WordNo+1; i <= S.GetClause(ClauseNo).m_iLastWord; i++)
	  if ( HasDeclinableSynNounInInstrumentalis(S.m_Words[i]) 	 )
		  return true;

  return false;	
};

void CRusSentence::CheckGroupsForAnalyticalVerbForms(CAnalyticalFormVars &vectorAnalyticalVariants, CPeriod ClausePeriod)
{

	try
	{
		CRusFormatCaller FormatCaller(GetOpt());

		FormatCaller.AddSimpleRulesForAnalyticalVerbFormChecking();
		

		CClause prClause(this, ClausePeriod.m_iFirstWord, ClausePeriod.m_iLastWord);
		SetIgnoreWhileBuildingSynVariantsToAllClauses(true);
		prClause.BuildGroups(FormatCaller, false);
		SetIgnoreWhileBuildingSynVariantsToAllClauses(false);
		
		
		for (CSVI it = prClause.m_SynVariants.begin(); !vectorAnalyticalVariants.empty()&& (it!=prClause.m_SynVariants.end()); it++)
			for (size_t GroupNo=0; GroupNo< it->m_vectorGroups.GetGroups().size(); GroupNo++)
			{
				const CGroup& group = it->m_vectorGroups.GetGroups()[GroupNo];

				for ( int i = 0; i < vectorAnalyticalVariants.size(); i++ )
					if(			(group.m_iFirstWord <= vectorAnalyticalVariants[i].iWordNum) 
							&&	(group.m_iLastWord >= vectorAnalyticalVariants[i].iWordNum)
						)
					{
						vectorAnalyticalVariants.erase( vectorAnalyticalVariants.begin()+i );
						i--;
					}				
			}
		
	}
	catch(...)
	{
		OutputErrorString("Failed RunSyntaxInClause(CheckGroupsForAnalyticalVerbForms)");
	}
}



bool HasShortParticipleOrAdj(const CSynWord& W)
{
	for (int i = 0; i < W.m_Homonyms.size(); i++)
		if (W.m_Homonyms[i].HasPos(PARTICIPLE_SHORT) || W.m_Homonyms[i].HasPos(ADJ_SHORT))
			return true;
		
	return false; 	
}


/*
разбираем случай "они должны были быть потрачены",
когда словоформа "были" должна  будет подсоединиться к форме "должны", 
а сначала "быть" должна подсоединиться к форме "потрачены", чтобы "потрачены"
не считалось предикатом, а стало инфинитивом. Последнее действие совершается
в ниженстоящей процедуре.  Здесь ищется инфинитив "быть", за которым сразу же или через несколько
слов должно стоять краткое прилагательное, например:
"они должны были быть быстро потрачены"

*/
void BuildAnalyticalVerbFormsZaplata2(CRusSentence& S, int ClauseNo)
{

	const CClause&  PrCl = S.GetClause(ClauseNo);
	int iBe = -1;
	for (int WordNo = PrCl.m_iFirstWord; WordNo <= PrCl.m_iLastWord; WordNo++)
	{
		if (S.m_Words[WordNo].m_strUpperWord =="БЫТЬ")
		{
			iBe = WordNo;
			continue;
		};
		if (iBe != -1)
			if (HasShortParticipleOrAdj(S.m_Words[WordNo])) 
				if (S.CheckAnalyticalVerbForm(iBe, WordNo))
				{

					/*
					"быть" с краткой формой
					не может быть вершиной клаузы, например, нельзя
					сказать "*Чтобы быть красив, ты должен уйти"
					*/
					S.m_Words[iBe].m_MainVerbs.push_back( WordNo );

					break;
				}

	};
};

bool HasCompar(const CSynWord& W)
{
	for (int i = 0; i < W.m_Homonyms.size(); i++)
		if ( (W.m_Homonyms[i].HasPos(ADJ_FULL)) && ( W.m_Homonyms[i].HasGrammem(rComparative) ) )
			return true;
		
	return false; 	
}

bool HasPredik(const CSynWord& W)
{
	for (int i = 0; i < W.m_Homonyms.size(); i++)
		if (!W.m_Homonyms[i].HasGrammem(rIndeclinable)) //  "бух","спасибо" не строятся с анал. формами
			if (W.m_Homonyms[i].HasPos(PREDK) || W.m_Homonyms[i].HasPos(PRONOUN_PREDK))
				return true;
		
	return false; 	
}

bool HasInfinitive(const CSynWord& W)
{
	for (int i = 0; i < W.m_Homonyms.size(); i++)
		if (W.m_Homonyms[i].HasPos(INFINITIVE))
			return true;
		
	return false; 		
}




/*
  построение аналитических форм глагола; 
  вспомогательным гл. может быть "быть" или "стать"
*/
void CRusSentence::BuildAnalyticalVerbForms()
{
	for (int  ClauseNo = 0; ClauseNo < GetClausesCount(); ClauseNo++)
	{
		//номер вспомогательного глагола в цепочке словоформ - первая компонента ан.ф. 
		int iBe = -1;
		//вектор потенциальных предикатов на заполнение второй компоненты  
		CAnalyticalFormVars v_AnalyticalFormVars;
		BuildAnalyticalVerbFormsZaplata2(*this, ClauseNo);
		const CClause&  PrCl = GetClause(ClauseNo);
		
		for (int WordNo = PrCl.m_iFirstWord; WordNo <= PrCl.m_iLastWord; WordNo++)
		if (		m_Words[WordNo].m_MainVerbs.empty() 
				&&	(FindFirstAuxVerb(WordNo) == -1)
			)
		{
			string s_lem = m_Words[WordNo].GetSynHomonym(0).m_strLemma;
			if (m_Words[WordNo].IsInOborot()) continue;
			//ищем гл. "быть" или "стать"
			if (m_Words[WordNo].HasAnalyticalBe() && iBe == -1)
			{
				iBe = WordNo;
				continue;
			}

			if ( HasInfinitive(m_Words[WordNo]))
			{
				v_AnalyticalFormVars.push_back( SAnalyticalFormVariant(WordNo, m_Words[WordNo].GetHomonymsCount(), SAnalyticalFormVariant::Infinitive, s_lem, AllHomonymsArePredicates(m_Words[WordNo])) );
				continue;
			}

			if ( HasPredik( m_Words[WordNo]) ) 
			{
				v_AnalyticalFormVars.push_back( SAnalyticalFormVariant(WordNo, m_Words[WordNo].GetHomonymsCount(), SAnalyticalFormVariant::Predikative, s_lem, AllHomonymsArePredicates(m_Words[WordNo])) );
				continue;
			}

			if (HasShortParticipleOrAdj(m_Words[WordNo])) 
			{
				v_AnalyticalFormVars.push_back( SAnalyticalFormVariant(WordNo, m_Words[WordNo].GetHomonymsCount(), SAnalyticalFormVariant::Short_Form, s_lem, AllHomonymsArePredicates(m_Words[WordNo])) );
				continue;
			}
			//поиск сравнительной степени
			if ( HasCompar(m_Words[WordNo]) ) 		 
				v_AnalyticalFormVars.push_back( SAnalyticalFormVariant(WordNo, m_Words[WordNo].GetHomonymsCount(), SAnalyticalFormVariant::Comp_Adj, s_lem, AllHomonymsArePredicates(m_Words[WordNo])) );
		}
		
		if (iBe != -1 && v_AnalyticalFormVars.size() > 0)
		{
			{
				//правило для цепочки {"быть"(буд.), предикатив (омонимичный), инфинитив (нс)}, тогда строим ан.ф. с инфинитивом
				//"Мальчик будет горько плакать"
				int DummyHomNo;
				CIntVector dummyVector;
				if	(		(2 == v_AnalyticalFormVars.size() )
						&&	(iBe < v_AnalyticalFormVars[0].iWordNum) 
						&&  !v_AnalyticalFormVars[0].bAllHomPredik 
						&&  SAnalyticalFormVariant::Predikative == v_AnalyticalFormVars[0].ePos 
						&&  SAnalyticalFormVariant::Infinitive == v_AnalyticalFormVars[1].ePos 

						&&  IsAnalyticalVerbForm(iBe, v_AnalyticalFormVars[0].iWordNum, DummyHomNo, dummyVector)
						&&  IsAnalyticalVerbForm(iBe, v_AnalyticalFormVars[1].iWordNum, DummyHomNo, dummyVector)
					)
					{
						bool bFoundChain =  true;
						for ( int mm = PrCl.m_iFirstWord; mm < iBe; mm++ )
							for ( int tt = 0; tt < m_Words[mm].GetHomonymsCount(); tt++ )
                                if ( m_Words[mm].m_Homonyms[tt].HasGrammem(rDativ) && !m_Words[mm].HasDes(OLLE) )
									bFoundChain = false;	

						if (bFoundChain)
							// удаляем вариант омонимичного предикатива, если есть вариант будущего времени
						{
							v_AnalyticalFormVars.erase( v_AnalyticalFormVars.begin() );

						};
					}

				//  конец  подслучая "Мальчик будет горько плакать"
			};

			//проверить на возможное вхождение компоненты аналитической формы в синтаксическую группу:
			//"был о нем"
			CheckGroupsForAnalyticalVerbForms(v_AnalyticalFormVars, PrCl);

			if ( 0 == v_AnalyticalFormVars.size() ) continue;

			/*разбираем случай "он был больше учителем, чем шофером", где не надо строить
			  аналитическую форму со сравн. степенью, хотя, в принципе, ей ничего не мешает.

			  Формально:
			  Если у нас только одна гипотеза ("сравн. степени"), если после нее стоит существительное
			  в творительном падеже, тогда  анал. форму строит не надо.
			   Сокирко. 3 мая 2001
			*/
			if (v_AnalyticalFormVars.size() == 1)
				if (SAnalyticalFormVariant::Comp_Adj == v_AnalyticalFormVars[0].ePos)
					if (CheckComparativeZAPLATAForAnalyticalForm(*this, v_AnalyticalFormVars[0].iWordNum, ClauseNo))
						continue;


			//установить порядок на гипотезах (см. operator<);
			sort(v_AnalyticalFormVars.begin(), v_AnalyticalFormVars.end());

			for ( int k = 0; k < v_AnalyticalFormVars.size(); k++ )
				if (CheckAnalyticalVerbForm(iBe, v_AnalyticalFormVars[k].iWordNum))
				{
					m_Words[iBe].m_MainVerbs.push_back( v_AnalyticalFormVars[k].iWordNum );
					{
						string dump =  m_Words[iBe].m_strWord  + " " + m_Words[v_AnalyticalFormVars[k].iWordNum].m_strWord;
						rml_TRACE("analytical form \"%s\" was created\n", dump.c_str());
					};

					{ // удалить все омонимы, кроме глагола
						m_Words[iBe].SetHomonymsDel(false);

						for (size_t HomNo=0; HomNo < m_Words[iBe].m_Homonyms.size(); HomNo++)
							if (GetClauseTypeByAncodePattern(m_Words[iBe].m_Homonyms[HomNo]) == UnknownPartOfSpeech) 
								m_Words[iBe].m_Homonyms[HomNo].m_bDelete = true;

						DeleteMarkedHomonymsWithClauses(iBe);
					};

					InitClauseType(GetClause(ClauseNo));
					break;
				}

		}
	}
}

bool CRusSentence::CheckAnalyticalVerbForm(int iVWrd, int iSWrd)
{
	int VerbHomNo;
	CIntVector AnalyticHom;

	if ( !IsAnalyticalVerbForm(iVWrd, iSWrd, VerbHomNo, AnalyticHom) ) 
		return false;
	
	if (AnalyticHom.size() == 0) return false;
	int j = 0;
	for (; j < m_Words[iSWrd].GetHomonymsCount(); j++)
	{
		if ( find(AnalyticHom.begin(), AnalyticHom.end(), j) == AnalyticHom.end() )
			m_Words[iSWrd].m_Homonyms[j].m_bDelete =  true;
	}

	DeleteMarkedHomonymsWithClauses(iSWrd);
		
/*	for (j = 0; j < AnalyticHom.size(); j++)
		ChangeGramcodeAsAnalyticForm(GetOpt(), m_Words[iSWrd].m_Homonyms[j], m_Words[iVWrd].m_Homonyms[VerbHomNo]);
*/
	return true;
}

//проверяет согласование вспомогательных глаголов и предикатов в ан.ф. 
bool CRusSentence::IsAnalyticalVerbForm(int iVerbWrd, int iSWrd, int& VerbHomNo, CIntVector& AnalyticHom)
{
	AnalyticHom.clear();

	//с предикативными значениями "много" и "мало" анал. форм не строить 
	if ( ( m_Words[iSWrd].FindLemma("МНОГО") || m_Words[iSWrd].FindLemma("МАЛО") ) &&
		 -1 != m_Words[iSWrd].GetHomonymByPOS(PREDK) )	
		return false;

	for (VerbHomNo = 0; VerbHomNo < m_Words[iVerbWrd].GetHomonymsCount(); VerbHomNo++)
	{
		const CSynHomonym& VerbHom = m_Words[iVerbWrd].GetSynHomonym(VerbHomNo);

		if (		VerbHom.IsLemma("БЫТЬ")
			||	(		VerbHom.IsLemma("СТАТЬ")
					&&	(		VerbHom.HasPos(VERB) 
							||	VerbHom.HasPos(INFINITIVE)
						) 
				) 
			)
		{
			for (int j = 0; j < m_Words[iSWrd].GetHomonymsCount(); j++)
			{

				CSynHomonym ShortFormHom = m_Words[iSWrd].GetSynHomonym(j);
				if (ShortFormHom.HasPos(INFINITIVE) && ShortFormHom.HasGrammem(rNonPerfective))
					if (VerbHom.HasGrammem(rFutureTense) || VerbHom.IsLemma("СТАТЬ"))
						AnalyticHom.push_back(j);

				if (        (      ShortFormHom.HasPos(PREDK) 
                                || ShortFormHom.HasPos(PRONOUN_PREDK)
                            ) 
                        && 
					        (      VerbHom.HasGrammem(rNeutrum) 
                                ||  VerbHom.HasPos(INFINITIVE)  // "вам должно стать скучно"
                                || (        VerbHom.HasGrammem(rFutureTense) 
                                        &&  VerbHom.HasGrammem(rThirdPerson) 
                                        &&  VerbHom.HasGrammem(rSingular) 
                                   ) 
                             ) 
                    )
					AnalyticHom.push_back(j);

				if ( (ShortFormHom.HasPos(ADJ_FULL)) && ( ShortFormHom.HasGrammem(rComparative) ) )
					AnalyticHom.push_back(j);	
				
				if (ShortFormHom.HasPos(PARTICIPLE_SHORT) || ShortFormHom.HasPos(ADJ_SHORT) )
				{
					// "была", "стал", "был", "было" ...
					if	(		VerbHom.HasGrammem(rSingular) 
							&&	VerbHom.HasGrammem(rPastTense)
							&&	ShortFormHom.HasGrammem(rSingular) 
							&&	(VerbHom.m_iGrammems & ShortFormHom.m_iGrammems & rAllGenders)
						)
					{
						AnalyticHom.push_back(j);
					}
					else
					if	(		VerbHom.HasGrammem(rPlural) 
							&&	ShortFormHom.HasGrammem(rPlural) 
						)
					{
						AnalyticHom.push_back(j);
					}
					else
					if (		!VerbHom.HasGrammem(rPastTense)
							&&	VerbHom.HasGrammem(rSingular) 
							&&	ShortFormHom.HasGrammem(rSingular) 
						)
					{
						AnalyticHom.push_back(j); 	
					}
					else
					if	(	(		VerbHom.HasPos(INFINITIVE) 
								||	VerbHom.HasPos(ADVERB_PARTICIPLE)
							)
							&&	(iVerbWrd < iSWrd) // последним условием мы запрещаем строиться 
											// анал. форме в  предложении "они должны уже быть".
						)
					{
						AnalyticHom.push_back(j); 	
					}
				}
			}

			if (AnalyticHom.size() == 0) return false;

			return true;
		}
	}
	return false;
}


