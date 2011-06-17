// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko


#include "StdSynan.h"
#include "RusSentence.h"
#include "RusFormatCaller.h"

#undef NDEBUG
#include "RusWord.h"

CSentence* NewSentenceRussian (const CSyntaxOpt* pSyntaxOptions)
{
	return new CRusSentence(pSyntaxOptions);
};

CRusSentence::CRusSentence(const CSyntaxOpt* pSyntaxOptions)  : CSentence(pSyntaxOptions)
{
	InitClauseVectorRules();

	m_KOTORYI_INDEX = GetOpt()->m_pOborDic->FindSubConj("КОТОРЫЙ"); 
	m_CHEI_INDEX	 = GetOpt()->m_pOborDic->FindSubConj("ЧЕЙ"); 

}

CRusSentence::~CRusSentence()
{
	m_vectorPrimitiveRules.clear();
};

void CRusSentence::ReadNextFromPlmLinesLanguageSpecific()
{
	TryBuildVerbLemmaWithKa();
};



void CRusSentence::ChooseClauseType(const  vector<SClauseType>& vectorTypes, CMorphVariant& V)
{
	int empty_type = -1; 

	for(int i = 0 ; i < vectorTypes.size() ; i++ )
	{
		if( !vectorTypes[i].m_Root.IsEmpty() )
		{
			int node = vectorTypes[i].m_Root.m_WordNo;
			int hom = vectorTypes[i].m_Root.m_HomonymNo;

			int iUnit = V.UnitNoByWordNo(node);
			assert (iUnit != -1);
			if( V.m_SynUnits[iUnit].m_iHomonymNum == hom )
			{
				V.m_ClauseTypeNo = i;
				return;
			}				
		}
		else
			empty_type = i;

	}
	V.m_ClauseTypeNo = empty_type;

}

bool CRusSentence::RunSyntaxInClauses(ESynRulesSet)
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


void TryBuildVerbLemmaWithKa(CSynWord& W)
{
   if ( W.m_strWord.find("-ка") == string::npos) return;

   for (int i = 0; i < W.m_Homonyms.size(); i++)
   {
     if (W.m_Homonyms[i].HasPos(VERB))
	  if ((W.m_Homonyms[i].HasGrammem(rPlural) &&
		   W.m_Homonyms[i].HasGrammem(rPastTense)) ||
		  ((W.m_Homonyms[i].HasGrammem(rFirstPerson) || 
		    W.m_Homonyms[i].HasGrammem(rSecondPerson)) &&  
		  W.m_Homonyms[i].HasGrammem(rImperative)))
	  {
         CSynHomonym GoodHom = W.m_Homonyms[i];
		 W.m_Homonyms.clear();
         W.m_Homonyms.push_back(GoodHom);
		 return;
	  }
   }   
}

void CRusSentence::TryBuildVerbLemmaWithKa()
{
  for (int i = 0; i < m_Words.size(); i++)
	  ::TryBuildVerbLemmaWithKa(m_Words[i]);
}

void CRusSentence::TryToAddComparativeTypeToClause()
{
	int i,j,k;
	for (i = 0; i < GetClausesCount(); i++)
	{
		if ( GetClause(i).HasUnambiguousStrongRootWithoutWeakHomonyms()) continue;
		
		for (j = GetClause(i).m_iFirstWord; j <= GetClause(i).m_iLastWord; j++)
			for(k = 0; k < m_Words[j].GetHomonymsCount(); k++)
				if (  (   (m_Words[j].GetSynHomonym(k).HasPos(ADJ_FULL)) 
					   || (m_Words[j].GetSynHomonym(k).HasPos(NUMERAL)) // больше, меньше
					  )
					&& 
					 (m_Words[j].GetSynHomonym(k).HasGrammem(rComparative)) 
				   )
				{
					SClauseType NewTyp(COMPARATIVE_T, j, k);	
					GetClause(i).m_vectorTypes.push_back(NewTyp);
				}			
				
	}
}


void CRusSentence::DeleteHomOneToThousand()
{
	for (int i = 0; i < m_Words.size(); i++)
	{
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

void CRusSentence::AddWeightForSynVariantsInClauses()
{
	for (int i = 0; i < GetClausesCount(); i++)
	{
		CClause&  pClause = GetClause(i);
		
		pClause.AddVariantWeightWithHomOneToFifty();
	}
}


void CreateHomonymFor_NECHEGO(CSynHomonym& H, long plPardigmID, string psAncode, long plOldGrammems)
{
	string s;
	H.m_lPradigmID = plPardigmID;
	
	H.m_lFreqHom = 1;
	s = "НЕЧЕГО";
	H.SetLemma(s);
	H.m_iCmpnLen = 6;
	H.m_LemSign = '+';
	H.m_CommonGramCode = "??";
	const CAgramtab* Agramtab = H.GetOpt()->GetGramTab();
	H.m_iGrammems = Agramtab->GetAllGrammems( psAncode.c_str() );
	H.m_iPoses = (1 << PRONOUN_PREDK);
	

	H.m_iGrammems &= ~_QM(rGenitiv);
	int lGramm = H.m_iGrammems;
	vector<int> v_gramm;

	if ( (plOldGrammems & _QM(rGenitiv)) != 0 )
	{
		H.m_iGrammems |= _QM(rGenitiv);
		lGramm |= _QM(rGenitiv);
		v_gramm.push_back(lGramm);
		lGramm &= ~_QM(rGenitiv);
	}

	if ( (plOldGrammems & _QM(rDativ)) != 0 )
	{
		H.m_iGrammems |= _QM(rDativ);
		lGramm |= _QM(rDativ);
		v_gramm.push_back(lGramm);
		lGramm &= ~_QM(rDativ);
	}

	if ( (plOldGrammems & _QM(rAccusativ)) != 0 )
	{
		H.m_iGrammems |= _QM(rAccusativ);
		lGramm |= _QM(rAccusativ);
		v_gramm.push_back(lGramm);
		lGramm &= ~_QM(rAccusativ);
	}

	if ( (plOldGrammems & _QM(rInstrumentalis)) != 0 )
	{
		H.m_iGrammems |= _QM(rInstrumentalis);
		lGramm |= _QM(rInstrumentalis);
		v_gramm.push_back(lGramm);
		lGramm &= ~_QM(rInstrumentalis);
	}

	if ( (plOldGrammems & _QM(rLocativ)) != 0 )
	{
		H.m_iGrammems |= _QM( rLocativ);
		lGramm |= _QM(rLocativ);
		v_gramm.push_back(lGramm);
		lGramm &= ~_QM(rLocativ);
	}
	
	for ( int i = 0; i < v_gramm.size(); i++ )
	{
		string strPos = H.GetPartOfSpeechStr();
		string strHelpGr;
		strHelpGr = Agramtab->GrammemsToStr(v_gramm[i]);

		string n_code;
		BYTE iPos;
		QWORD iGrm;
		strPos += " ";
		strPos += strHelpGr;
		bool Result = Agramtab->ProcessPOSAndGrammems(strPos.c_str(), iPos, iGrm); 
		assert (Result);
 		Result = Agramtab->GetGramCodeByGrammemsAndPartofSpeechIfCan(iPos, iGrm, n_code);
		assert (Result);

		H.m_GramCodes += n_code; 
	}
	
	
}


void CRusSentence::DisruptPronounPredik()
{
	for (size_t i = 0; i+2 < m_Words.size(); i++)
    {
        if (m_Words[i+0].HasOborot1() != m_Words[i+0].HasOborot2()) continue;
        if (m_Words[i+1].HasOborot1() != m_Words[i+1].HasOborot2()) continue;
        if (m_Words[i+2].HasOborot1() != m_Words[i+2].HasOborot2()) continue;

        int ihom  = m_Words[i+2].GetHomonymByPOS(PRONOUN);
		if (        m_Words[i].FindLemma("НЕ") 
                &&  m_Words[i+1].GetHomonymByPOS(PREP) != -1
                &&  m_Words[i+2].FindLemma("ЧТО")
                &&  ihom != -1
            )
        {
	        QWORD lOldGrammems = m_Words[i+2].m_Homonyms[ihom].m_iGrammems;
	        m_Words[i+2].m_Homonyms.clear();
	        CSynHomonym Hom_nechego (this);
	        CreateHomonymFor_NECHEGO(Hom_nechego, GetOpt()->m_lPradigmID_NECHEGO, GetOpt()->m_Gramcode_NECHEGO, lOldGrammems);
	        m_Words[i+2].m_Homonyms.push_back(Hom_nechego);
	        MarkWordAsDeleted( i );
        }
	    
    }

		
}


const long  CommonNounPrefixesCount = 2;
const string CommonNounPrefixes[CommonNounPrefixesCount] = {"ВИЦЕ-", "ЭКС-"};


void CreateHomonymFor_EksVice(CSynHomonym& H,long plPardigmID, string psAncode, string sLem, string TypeAncode)
{
	const CAgramtab* Agramtab = H.GetOpt()->GetGramTab();
	H.m_lPradigmID = plPardigmID;
	H.m_GramCodes = psAncode;
	H.m_lFreqHom = 1;
	H.SetLemma(sLem);
	H.m_iCmpnLen = sLem.length();
	H.m_LemSign = '+';
	H.m_CommonGramCode = TypeAncode; 
	H.m_iGrammems = Agramtab->GetAllGrammems( psAncode.c_str() );
	H.m_iPoses = (1 << Agramtab->GetPartOfSpeech( psAncode.c_str() ));
	
}

void CRusSentence::CutPrefixEksAndVize()
{

	for ( int ii = 0; ii < m_Words.size(); ii++ )
	{
		string Word = m_Words[ii].m_strUpperWord;
		vector<int> Prefixes;
		for (long i=0; i < CommonNounPrefixesCount; i++)
		 if (CommonNounPrefixes[i] == Word.substr(0, CommonNounPrefixes[i].length()))
		 {
			 Prefixes.push_back(i);
			 Word.erase(0, CommonNounPrefixes[i].length());
			 i = -1;
			 if (Word.empty()) break;
		 };
		if (Prefixes.empty()) continue;


	
		vector<CFormInfo> Paradigms;
		GetOpt()->GetLemmatizer()->CreateParadigmCollection(false, Word, false, false, Paradigms);

		if ( Paradigms.size() < 1 ) continue;

		vector<CSynHomonym> vec_Homonyms;
		for (long k=0; k < Paradigms.size(); k++)
		{
			long lParadigm = Paradigms[k].GetParadigmId();
			string TypeAncode = Paradigms[k].GetCommonAncode();
			string sGramatCode = Paradigms[k].GetSrcAncode();
			string sLemma = Paradigms[k].GetWordForm(0); 
            if ( !binary_search(GetOpt()->m_pProfessions->m_vectorDatItems.begin(), GetOpt()->m_pProfessions->m_vectorDatItems.end(), sLemma) )
				continue;
			CSynHomonym NewHom (this);
			CreateHomonymFor_EksVice(NewHom, lParadigm, sGramatCode, sLemma, TypeAncode);
			vec_Homonyms.push_back(NewHom);
		}

		if ( vec_Homonyms.size() > 0 )
		{
			for (long i=0; i <  Prefixes.size(); i++)
			{
                m_Words[ii].m_UnparsedGraphemDescriptorsStr +=  " " + CommonNounPrefixes[Prefixes[i]];
			};

			m_Words[ii].m_Homonyms.clear();
			for ( int j = 0; j < vec_Homonyms.size(); j++ )
				m_Words[ii].m_Homonyms.push_back(vec_Homonyms[j]);
		}

	}
}

//менять граммемы подлежащего и группы, в которую он входит только после того,
//как все клаузы уже построены (после операций объединения и вложения клауз), т.е.
//когда отработают все правила фрагментации.
//раньше чистка граммем происходила в ф-ции CFormatCaller::find_subj_for_verb
//пример: "Стол, что за нами пристально следит, преследовал меня весь день"
//при первоначальной фрагментации подлежащим становился "день", а после объединения клауз "стол",
//поэтому терялся вн. у "день".    
void ChangeSubjAndItsGroupGrammemsInClause(CClause& C, SVI pSynVar)
{
	if ( pSynVar->m_Subjects.size() != 1) return;


	int iSbj = pSynVar->GetUnitLastWord(pSynVar->GetFirstSubject());
	assert(iSbj != -1);
	int iPrdk = pSynVar->GetUnitLastWord(pSynVar->m_iPredk);
	assert(iPrdk != -1);

	CGroup* GrpWithSubj = const_cast<CGroup*>( C.GetLastHost(iSbj, pSynVar) );
	const CGroup* GrpWithPredk = C.GetLastHost(iPrdk, pSynVar);

	QWORD ipredk_gram = 0;
	if ( !GrpWithPredk )
		ipredk_gram = pSynVar->m_SynUnits[pSynVar->m_iPredk].m_iGrammems;
	else
		ipredk_gram = GrpWithPredk->GetGrammems();

	QWORD isubj_gram = 0;	
	if ( !GrpWithSubj )
		isubj_gram = pSynVar->m_SynUnits[pSynVar->GetFirstSubject()].m_iGrammems;
	else
		isubj_gram = GrpWithSubj->GetGrammems();

	string debug = C.GetOpt()->GetGramTab()->GrammemsToStr(isubj_gram);
	debug = C.GetOpt()->GetGramTab()->GrammemsToStr(ipredk_gram);

    QWORD new_subj_grammems = (ipredk_gram & isubj_gram & rAllNumbers) | _QM(rNominativ); 
	debug = C.GetOpt()->GetGramTab()->GrammemsToStr(new_subj_grammems);


	// setting grammems  for group and relation
	if ( GrpWithSubj )
	{
		size_t breaks = rAllCases | rAllNumbers;	
		GrpWithSubj->SetGrammems( GrpWithSubj->GetGrammems() & (new_subj_grammems | ~breaks) );
        for ( int i = 0; i < pSynVar->m_vectorGroups.m_Relations.size(); i++ )
		{
            if (GrpWithSubj->m_MainWordNo == pSynVar->m_vectorGroups.m_Relations[i].m_iFirstWord)
            {
			    pSynVar->m_vectorGroups.m_Relations[i].m_iGrammems = GrpWithSubj->GetGrammems();
			    if (GrpWithSubj->m_GroupType == NOUN_ADJ)
			    {
				    int AdjUnitNo = pSynVar->UnitNoByWordNo(pSynVar->m_vectorGroups.m_Relations[i].m_iLastWord);
				    assert (AdjUnitNo < pSynVar->m_SynUnits.size());
                    pSynVar->m_SynUnits[AdjUnitNo].ModifyGrammems( new_subj_grammems );	
			    };
            }
		}
	}

    pSynVar->m_SynUnits[pSynVar->GetFirstSubject()].ModifyGrammems(new_subj_grammems);	
	

}

void CRusSentence::ChangeSubjAndItsGroupGrammems()
{
	for ( int i = 0; i < GetClausesCount(); i++ )
	{
		CClause& PrCl = GetClause(i);
		for ( SVI pSynVar = PrCl.m_SynVariants.begin(); pSynVar != PrCl.m_SynVariants.end(); pSynVar++ )
		{
			if ( pSynVar->HasSubj() )
				ChangeSubjAndItsGroupGrammemsInClause(PrCl, pSynVar);
		}
	}
}

bool	CRusSentence::IsRelativSentencePronoun(int ClauseStartWordNo, int WordNo, int& HomonymNo) const
{
	int SubordConjNo = m_Words[WordNo].m_SubordinateConjNo;
	HomonymNo = 0;

	// "кое-чьи" не является релативным словом, а морфология  его не знает, поэтому возникает омоним "чей"
	if (m_Words[WordNo].m_strWord.find('-') != string::npos) return false;
	

	if (m_Words[WordNo].m_Homonyms.empty()) return false;
	return 	(SubordConjNo == m_KOTORYI_INDEX)
		||	(SubordConjNo == m_CHEI_INDEX);
};


EClauseType CRusSentence::GetClauseTypeByAncodePattern (const CAncodePattern& Pattern) const
{
	
    if( Pattern.HasPos(VERB ) )
        return VERB_PERS_T;
    else
    if( Pattern.HasPos(ADVERB_PARTICIPLE) )
        return ADVERB_PARTICIPLE_T;
    else
    if( Pattern.HasPos(PARTICIPLE_SHORT) )
        return PARTICIPLE_SHORT_T;
    else
    if( Pattern.HasPos(ADJ_SHORT) )
        return ADJ_SHORT_T;
    else
    if( Pattern.HasPos(PREDK) )
        return PREDK_T;
    else
    if( Pattern.HasPos(PARTICIPLE) )
        return PARTICIPLE_T;
    else
    if( Pattern.HasPos(INFINITIVE) )
        return INFINITIVE_T;
    else
    if( Pattern.HasPos(INP) )
        return INP_T;
    else
	if	( Pattern.HasPos(ADJ_FULL) && Pattern.HasGrammem(rComparative) )
		return COMPARATIVE_T;

	//возвращаем  несуществующую часть речи
	return UnknownPartOfSpeech;
};



//функция  идет по всем словам предложение, если слово вошло в графематическую вилку  ФИ1-ФИ2,
//тогда это слово не считается  союзом
//Например, "И" в "И.А.Кирилловой"  не будет союзом.

void CRusSentence::ProcessFio1Fio2()
{
	bool bInFIO = false;
	for (int i = 0; i < m_Words.size();i++)
	{
        if (m_Words[i].HasDes(OFAM1) )
			bInFIO = true;

		if (bInFIO)
			for (int k =0; k<m_Words[i].m_Homonyms.size(); k++)
				m_Words[i].m_Homonyms[k].m_CoordConjNo = -1;

        if (m_Words[i].HasDes(OFAM2))
			bInFIO = false;
	};
		


};


// если в клаузе нашлись "который" или "чeй", тогда  омонимы вершины  на причастие или на деепричастие надо удалить
void CRusSentence::DeleteSomeTypesInRelativeClauses()
{
	for (size_t ClauseNo =0; ClauseNo < m_Clauses.size(); ClauseNo++)
	{
		CClause& C = m_Clauses[ClauseNo];
		if (C.IsRelative() )
		{
			for (int i=C.m_vectorTypes.size()-1; i>=0; i--)
			if (		(C.m_vectorTypes[i].m_Type == PARTICIPLE_T)
					||	(C.m_vectorTypes[i].m_Type == ADVERB_PARTICIPLE_T)
				)
					if (m_Words[C.m_vectorTypes[i].m_Root.m_WordNo].m_Homonyms.size() > 1)
						C.DeleteHomonym(C.m_vectorTypes[i].m_Root.m_WordNo,C.m_vectorTypes[i].m_Root.m_HomonymNo);
		};
	};
};

bool	CRusSentence::IsProfession(const CSynHomonym& H) const
{
	return H.CompareWithPredefinedWords(*GetOpt()->m_pProfessions);
}

bool CRusSentence::BuildClauses()
{
    //ProcessFio1Fio2();
    //SolveAmbiguityUsingRuleForShortAdj();
    //DeleteHomonymsIfTooManyPredictedWords();
    //DeleteHomOneToThousand();
    //FindGraPairs();
    //BuildGLRGroupsInSentence();
    //if(! BuildInitialClauses() )
    //{
    //    return false;

    //};    
    //RunSyntaxInClauses(AllRules);
    //RecalculateRelationsCoordinates();	
    //AssignClauseNoToWords();	
    //AddWeightForSynVariantsInClauses();
    //return true;

    //запись в log файл отключена
    FILE* log_fp = 0;
    // log_fp = fopen("current_sentence.log", "w");

    ProcessFio1Fio2();

    SolveAmbiguityUsingRuleForShortAdj();
    DeleteHomonymsIfTooManyPredictedWords();
    m_bPanicMode = IsPanicSentence();
    if (m_bPanicMode)
    {
        rml_TRACE("!!!!!!!!!!  Entering panic mode !!!!!!!!!!!1\n");
    };
    BuildGLRGroupsInSentence();
    CRusSentence SavedSentence(GetOpt());
    SavedSentence = *this;
    m_bShouldUseTwoPotentialRule = !m_bPanicMode;
TryWithoutTwoPotentialRule:
    bool bRes = true;

    if (log_fp) 
    {
        fprintf (log_fp, "%s....",  GetSentenceBeginStr().c_str());;

    };

    assert ( GetClausesCount() == 0 );

    // соединение предикатива нечего: "Вам не о чем волноваться"
    DisruptPronounPredik();

    // удаление префиксов ВИЦЕ- и ЭКС- 
    CutPrefixEksAndVize();

    //	удаление омонимов вводных слов, если эти слова не выделены запятыми и пр.
    DetermineParenthesis();

    //	удаление омонимов, частоты которых соотносятся 1/1000
    // DeleteHomOneToThousand();

    FindGraPairs();

    CloneHomonymsForOborots();

    AscribeSimplePrepositionInterpretations(PREP);

    FindAllTermins();

    bool SecondTryOfCoverageKillHomonyms = false;
    vector<CSynWord> SaveWords = m_Words;
    vector<SFoundTermin> SaveFoundTermins = m_vectorTermins;

BuildInitialClausesLabel:

    TraceClauses();

    if(! BuildInitialClauses() )
    {
        fclose(log_fp);
        return false;
    }

    TraceClauses();

    DeleteSomeTypesInRelativeClauses();

    TraceClauses();

    if (log_fp)  fprintf (log_fp, "BuildAnalyticalVerbForms\n");
    BuildAnalyticalVerbForms();

    if (log_fp)  fprintf (log_fp, "TryToAddComparativeTypeToClause\n");
    TryToAddComparativeTypeToClause();

    /*if (log_fp)  fprintf (log_fp, "TryToRebuildDashInClause\n");
    TraceClauses();
    TryToRebuildDashInClause();
    TraceClauses();*/

    if (log_fp)  fprintf (log_fp, "RunSyntaxInClause(AllRules)\n");
    RunSyntaxInClauses(AllRules);

    // после первого вызова RunSyntaxInClause нужно удалить омонимы, которые противоречат найденным терминам
    if (log_fp)  fprintf (log_fp, "DeleteMarkedHomonymsWithClauses\n");
    DeleteMarkedHomonymsWithClauses(CPeriod(0,(int)m_Words.size()-1));

    if (log_fp)  fprintf (log_fp, "InitWordsInTermins\n");
    InitWordsInTermins();

    assert ( IsValid() );

    if (!m_bPanicMode)
    {
        if (log_fp)  fprintf (log_fp, "OneRunOfClauseRules(m_vectorPrimitiveRules)\n");
        OneRunOfClauseRules(m_vectorPrimitiveRules);
        assert ( IsValid() );

        if (log_fp)  fprintf (log_fp, "RunSyntaxInClause(RulesAfterSimClauses)\n");
        RunSyntaxInClauses(RulesAfterSimClauses);

        if (log_fp)  fprintf (log_fp, "OneRunOfClauseRules(m_vectorMainEncloseRules)\n");
        OneRunOfClauseRules(m_vectorMainEncloseRules);
        assert ( IsValid() );

        if (log_fp)  fprintf (log_fp, "RunSyntaxInClause(GroupRulesForClause)\n");
        RunSyntaxInClauses(GroupRulesForClause);	

        if (log_fp)  fprintf (log_fp, "OneRunOfClauseRules(m_vectorDisruptRules)\n");
        OneRunOfClauseRules(m_vectorDisruptRules);
        assert ( IsValid() );
    };

    if (log_fp)  fprintf (log_fp, "RunSyntaxInClause(AllRules)\n");
    RunSyntaxInClauses(AllRules);	

    if (log_fp)  fprintf (log_fp, "BuildClauseRelations()\n");
    BuildClauseRelations();
    assert ( IsValid() );

    if (log_fp)  fprintf (log_fp, "RecalculateRelationsCoordinates()\n");
    RecalculateRelationsCoordinates();	
    AssignClauseNoToWords();
    assert ( IsValid() );

    if (log_fp)  fprintf (log_fp, "AddWeightForSynVariantsInClauses()\n");
    AddWeightForSynVariantsInClauses();
    assert ( IsValid() );

    //менять граммемы подлежащего и группы, в которую оно входит, нужно  только после того,
    //как все клаузы уже построены (после операций объединения и вложения клауз), т.е.
    //когда отработают все правила фрагментации.
    //Раньше чистка граммем происходила в ф-ции CFormatCaller::find_subj_for_verb
    //пример: "Стол, что за нами пристально следит, преследовал меня весь день"
    //при первоначальной фрагментации подлежащим становился "день", а после объединения клауз "стол",
    //поэтому терялся вн. у "день".    
    ChangeSubjAndItsGroupGrammems();

    if (!m_bPanicMode)
    {
        for ( int tt = 0; tt < GetClausesCount(); tt++ )
            if ( 0 == GetClause(tt).GetSynVariantsCount() )
                if (m_bShouldUseTwoPotentialRule)
                {
                    *this = SavedSentence;
                    m_bShouldUseTwoPotentialRule = false;
                    if (log_fp)  fprintf (log_fp, "goto TryWithoutTwoPotentialRule\n");
                    goto TryWithoutTwoPotentialRule;
                }
                if (m_pSyntaxOptions->m_KillHomonymsMode == CoverageKillHomonyms)
                {

                    //	если  были удалены омонимы и программа организует еще один проход  (только один!)

                    if (log_fp)  fprintf (log_fp, "KillHomonymsInAllSentence\n");
                    if (    KillHomonymsInAllSentence()
                        && !SecondTryOfCoverageKillHomonyms
                        )
                    {
                        IsValid();
                        for ( int ClauseNo = 0; ClauseNo < GetClausesCount(); ClauseNo++ )
                        {
                            const CClause& Clause = GetClause(ClauseNo);

                            //	если остались ПУСТЫХи или клаузы  c пустым кол-вом вариантов

                            if (			Clause.m_vectorTypes.empty()  
                                //||		Clause.m_SynVariants.empty() 
                                )
                            {
                                SecondTryOfCoverageKillHomonyms = true;

                                //Мы не можем здесь оставить вектор слов(m_Words) так, как он есть,
                                //поскольку будем заново вызывать функцию BuildAnalyticalVerbForms, которая  будет строить 
                                //анал. формы,т.е. удалять и менять  сами слова.
                                //Мы так же не можем  сделать m_Words = SaveWords,
                                //поскольку тогда  восстановятся все омонимы,которые были удалены KillHomonymsInAllSentence.
                                //Поэтому мы делаем следующее.

                                vector<CSynWord> p;
                                for (int i =0; i < SaveWords.size(); i++)
                                {
                                    int j=0;
                                    for (; j< m_Words.size(); j++)
                                        if (			(SaveWords[i].m_GraphematicalUnitOffset == m_Words[j].m_GraphematicalUnitOffset)
                                            &&		(SaveWords[i].m_strWord == m_Words[j].m_strWord)
                                            )
                                            break;
                                    if (j < m_Words.size())
                                    {
                                        p.push_back(m_Words[j]);
                                        p.back().m_MainVerbs = SaveWords[i].m_MainVerbs;
                                        p.back().m_TrennbarePraefixWordNo = SaveWords[i].m_TrennbarePraefixWordNo;
                                    }
                                    else
                                        p.push_back(SaveWords[i]);

                                };
                                m_Words = p;
                                m_vectorTermins = SaveFoundTermins;
                                if (log_fp)  fprintf (log_fp, "goto  BuildInitialClausesLabel\n");
                                goto  BuildInitialClausesLabel;
                            };
                        };
                    };


                };
    };


    assert ( IsValid() );

    if (log_fp)  fprintf (log_fp, "fclose(log_fp)\n");
    if (log_fp)  fclose(log_fp);	

    return true;
}







