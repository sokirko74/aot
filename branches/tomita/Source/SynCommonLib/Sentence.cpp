// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko


#include "stdafx.h"
#include "Sentence.h"
#include "assert.h"
#undef NDEBUG

// language specific 

extern CFormatCaller* NewFormatCallerGerman(const CSyntaxOpt* Opt);
extern CFormatCaller* NewFormatCallerEnglish(const CSyntaxOpt* Opt);
extern CFormatCaller* NewFormatCallerRussian(const CSyntaxOpt* Opt);





CSentence::CSentence(const CSyntaxOpt* pSyntaxOptions) : m_GroupsUnion(pSyntaxOptions)
{
	m_pSyntaxOptions = pSyntaxOptions;
	m_bShouldUseTwoPotentialRule = true;
	m_bShowAllGroups  = false;
}

void CSentence::Reset()
{
	m_Words.clear();
	CClauseCollection::Clear();
}

string	CSentence::GetSentenceBeginStr() const
{
	string ss;
	int count =   m_Words.size();
	if (count  >10) count = 10;
	for (int i = 0; i < count; i++)
		ss = ss + " " + m_Words[i].m_strWord;
	ss = " (\""+ ss +"...\")";
	return ss;

};

void	CSentence::OutputErrorString(string s) const
{
	m_pSyntaxOptions->OutputErrorString(s+GetSentenceBeginStr());
};

void	CSentence::TraceClauses() const
{
#ifdef _DEBUG
	for (int i = 0; i<m_Clauses.size();i++)
	{
		rml_TRACE("%s ", m_Clauses[i].GetTraceStr().c_str());
	};
	rml_TRACE("\n");
#endif
};


CFormatCaller* CSentence::GetNewFormatCaller() const
{

	if (GetOpt()->m_Language==morphGerman )
		return NewFormatCallerGerman(GetOpt());
	else
		if (GetOpt()->m_Language==morphEnglish )
			return NewFormatCallerEnglish(GetOpt());
	else
		return NewFormatCallerRussian(GetOpt());
};


void	CSentence::SetIgnoreWhileBuildingSynVariantsToAllClauses (bool Value)
{
	for (int i = 0; i<m_Clauses.size();i++)
		m_Clauses[i].m_bIgnoreWhileBuildingSynVariants = Value;
};


//	удаляет все омонимы предсказанных слов, кроме первого, если  в предложении больше 10 неузнанных слов.

void CSentence::DeleteHomonymsIfTooManyPredictedWords() 
{
	int CountOfPredicted = 0;
	for (int i = 0; i<m_Words.size(); i++)
		if (m_Words[i].m_bPredicted)
			CountOfPredicted++;

	if (CountOfPredicted >= 10)
	{
		for (int i = 0; i<m_Words.size(); i++)
		if (m_Words[i].m_bPredicted)
		{
			m_Words[i].m_Homonyms.erase(m_Words[i].m_Homonyms.begin()+1, m_Words[i].m_Homonyms.end());
			m_Words[i].BuildTerminalSymbolsByWord();
		};
	};

}


bool CSentence::IsValid() const
{
	long i=0;

	for (; i< m_Clauses.size(); i++)
	{
		if (m_Clauses[i].m_SelfClauseNo != i) 
		{
			assert(false);
			return false;
		};

		for (long j=i+1; j < m_Clauses.size(); j++)
		{
			if (m_Clauses[i].has_intersection(m_Clauses[j]))
			{
				assert(false);
				return false;
			};
		};

		for (int k=0; k<m_Clauses[i].m_vectorTypes.size(); k++)
		{
			if	(!m_Clauses[i].m_vectorTypes[k].m_Root.IsValid(m_Clauses[i], this) )
			{
				assert(false);
				return false;
			};
		};

		if	( !m_Clauses[i].m_RelativeWord.IsValid(m_Clauses[i], this) )
		{
			bool debug = m_Clauses[i].m_RelativeWord.IsValid(m_Clauses[i], this);
			assert(false);
			return false;
		};

	};

	// the sentence should have at least one sentence 
	
	


	// клаузы должны  покрывать все слова 




	if (!m_Clauses.empty() && m_Clauses.back().m_iLastWord+1 != m_Words.size()) 
	{
		assert(false);
		return false;
	};


	// проверяем синварианты
	for (i=0; i< m_Clauses.size(); i++)
	{
		//if (m_Clauses[i].m_SynVariants.empty())
		//{
		//	rml_TRACE(Format("No morphological variants  for Clause No %i", i).c_str());
		//	assert(false);
		//	return false;
		//};
		
		int SynVarNo =0;
		for (CSVI j = m_Clauses[i].m_SynVariants.begin(); j != m_Clauses[i].m_SynVariants.end(); j++)
		{
			const CMorphVariant& C = *j;
			if (j->m_ClauseTypeNo  != -1)

			{
				if (j->m_ClauseTypeNo >= m_Clauses[i].m_vectorTypes.size()) 
				{
					rml_TRACE("Synvariant N % in clause %s contains bad pointer to clause type\n", SynVarNo, m_Clauses[i].GetTraceStr().c_str());
					assert(false);
					return false;
				};

			};

			int WordNo = m_Clauses[i].m_iFirstWord;
			for (int k = 0; k < j->m_SynUnits.size(); k++)
			{
				const CSynUnit& H = j->m_SynUnits[k];
				if (H.m_Type == EClause)
				{
					int ClauseNo = FindClauseIndexByPeriod(H.m_SentPeriod);

					if (ClauseNo == -1) 
					{
						assert(false);
						return false;
					}
					if (H.m_iClauseTypeNum != -1)
					{
						if (H.m_iClauseTypeNum >= m_Clauses[ClauseNo].m_vectorTypes.size()) 
						{
							rml_TRACE("Unit N %  in clause %s contains bad pointer to clause type\n", k, m_Clauses[i].GetTraceStr().c_str());
							assert(false);
							return false;
						};
					};
					if (C.GetFirstSubject() == k)
					{
							rml_TRACE("Bad pointer to subject in Clause No %i (it cannot point to a subclause)\n", i);
							assert(false);
							return false;
					};

					WordNo  = H.m_SentPeriod.m_iLastWord + 1;
				}	
				else
				{
					if (H.m_iHomonymNum > m_Words[WordNo].m_Homonyms.size()) 
					{
						rml_TRACE("Unit N %  in clause %s contains bad pointer to homonyms\n", k, m_Clauses[i].GetTraceStr().c_str());
						assert(false);
						return false;
					};
					WordNo++;

				};
			};
			
		}
	};

	
	for (int WordNo = 0; WordNo < m_Words.size(); WordNo++)
		for (int i = 0; i < m_Words[WordNo].m_MainVerbs.size(); i++)
				if	(m_Words[WordNo].m_MainVerbs[i] >= m_Words.size())
				{
					rml_TRACE("Bad link to the main verb (%i) from word %s\n", m_Words[WordNo].m_MainVerbs[i], m_Words[WordNo].m_strWord.c_str());
					assert(false);
					return false;
				};

    for (int WordNo = 0; WordNo < m_Words.size(); WordNo++)
    {
        if (m_Words[WordNo].HasOborot1())
        {
            int WordNo1 = WordNo;
            for (; WordNo1 < m_Words.size(); WordNo1++)
                if (m_Words[WordNo1].HasOborot2())
                    break;
            assert (WordNo1 < m_Words.size());
        }
    }

	return true;
};




bool CSentence::IsBetweenGraphematicalPairDescriptors(int WordNo) const
{
	const int MaxGraphematicalPairDistance = 7;
	
	if (		m_Words[WordNo].IsFirstOfGraPair()
			||	m_Words[WordNo].IsSecondOfGraPair()
		)
	return true;


	int i;
	for (i=WordNo; i < m_Words.size(); i++)
	{
		if (i > WordNo+MaxGraphematicalPairDistance) break;
		if (m_Words[i].IsFirstOfGraPair()) return false;
		if (m_Words[i].IsSecondOfGraPair()) return true;
	};

	for (i=WordNo; i >=0; i--)
	{
		if (i+MaxGraphematicalPairDistance < WordNo) break;
		if (m_Words[i].IsSecondOfGraPair()) return false;
		if (m_Words[i].IsFirstOfGraPair()) return true;
	};
	return false;
};




//	функция DeleteOborotThatContains удаляет пометы оборота, который  включает слово WordNo


void	CSentence::DeleteOborotThatContains(int WordNo)
{
	if (!m_Words[WordNo].IsInOborot()) return;
	
	//  идем вперед до конца оборота
	bool bOb1 = m_Words[WordNo].HasOborot1();
	for (int i = WordNo; i < m_Words.size(); i++)
	{
		bool bOb2 = m_Words[i].HasOborot2();

		m_Words[i].DeleteOborotMarks();

		if(bOb2) break;
	};

	if (!bOb1)
	{
		//  идем назад до начала оборота
		assert (WordNo > 0);
		for (int i = WordNo-1; i >= 0; i--)
		{
			bool bOb1 = m_Words[i].HasOborot1();

			m_Words[i].DeleteOborotMarks();

			if(bOb1) break;

		};

	};

};


bool CSentence::ProcessGrammarModels()
{
	m_vectorTermins.insert(m_vectorTermins.begin(), m_GrammarItems.begin(), m_GrammarItems.end() );
	return true;
};


/*
	This function builds vector  vector<CSimpleGroup>;:m_SimpleGroups;
	Each group consists of two pointers to a word and  an name.
*/
bool BuildAuxSimpleGroups(CSentence& S)
{
	
	for (size_t WordNo=0; WordNo<S.m_Words.size(); WordNo++)
	{
		
		const char* g_descr  = S.m_Words[WordNo].m_UnparsedGraphemDescriptorsStr.c_str();
		CGroups Groups(S.GetOpt());


		for (size_t i=0; i<strlen(g_descr); i++)
			if (!strncmp(g_descr+i,"#SimGrm",7))
			{
				char name[100];
				CGroup G;
				// Here we read groups coordinates.
				// G.first and G.last are in coordinates of the largest group,
				// so for the largest group G.first==0  and G.last is the lenght of the largest group.
				// G.m_MainWordNo is in coordinates of the group G, that's why we should add G.m_iFirstWord to
				// G.m_MainWordNo in order to get it in the  coordinates of the largest group.
				if (sscanf (g_descr+i+8, "%s %i %i %i", name, &G.m_iFirstWord,&G.m_iLastWord, &G.m_MainWordNo) != 4) 
				{
					ErrorMessage (Format("General format error between Simple grammar Module and Syntax! (%s)", g_descr+i+8));
					return  false;
				};
				G.m_MainGroup = G.m_MainWordNo;
				G.m_GroupType = S.GetOpt()->GetGroupTypebyName(name);
				if (G.m_GroupType == -1) 
				{
					ErrorMessage (Format("Simple grammar sends a group name which is not registered! (%s)", name));
					return false;
				};
				// no groups with the same boundaries are allowed
				if (Groups.find_group(G) == -1)
					Groups.create_group(G);
				
			};


		if (!Groups.GetGroups().empty())
		{
			// initializing CGroup::m_MainGroup,  previously CGroup::m_MainGroup was initiliazed by 
			// CGroup::m_MainWordNo,  no we should find the largest child which contains  m_MainWordNo
			for (int k=0; k  < Groups.GetGroups().size(); k++)
			{
				CGroup& G = *Groups.GetGroupPtr(k);
				assert (G.m_MainGroup.is_part_of(G));
				assert (G.m_MainGroup.size() == 1);
				assert (G.m_MainGroup.m_iFirstWord == G.m_MainWordNo);
				for (int j=k-1; j>=0; j--)
					if (		G.m_MainGroup.is_part_of(Groups.GetGroups()[j])
							&&	Groups.GetGroups()[j].is_part_of (G)
						)
					{
						G.m_MainGroup = Groups.GetGroups()[j];
						break;
					};
			};

			//  creating model
			S.m_GrammarModels.push_back(Groups);

			// finding the maximal group
			CGroup MaxGroup = Groups.GetGroups()[0];
			for (size_t i=0; i<Groups.GetGroups().size(); i++)
				if (Groups.GetGroups()[i].size() >  MaxGroup.size())
					MaxGroup = Groups.GetGroups()[i];
			
			
			//  creating a collocation
			SFoundTermin Termin(&S.m_GrammarModels.back(), NoneThes, -1, WordNo+MaxGroup.m_iFirstWord, WordNo+MaxGroup.m_iLastWord);
			
			S.m_GrammarItems.push_back(Termin);

		};
	};

	return true;

};

void CSentence::AddSynWord(CSynWord& Word) 
{
	Word.BuildTerminalSymbolsByWord();
    Word.UpdateConjInfo();
    sort(Word.m_Homonyms.begin(), Word.m_Homonyms.end());

	m_Words.push_back(Word); 			
}

bool ReadSentence(CSentence& S, const CPlmLineCollection* piPLMLinePtr, size_t& LineNo)
{
	S.Reset();
	
    int OborotNo = -1;

	for(; LineNo < piPLMLinePtr->m_Items.size(); LineNo++ )
	{
		const char* strPlmLine = piPLMLinePtr->m_Items[LineNo].c_str();

		if( !CheckIfHomonymPlmLine(strPlmLine))
		{
            // if the previous  word was  the last in the sentence, then exit
            if( !S.m_Words.empty() && S.m_Words.back().HasDes(OSentEnd)  )
				break;

            CSynWord Word(&S);
		
			//  reading the first homonym and word's  properties 
            if (!Word.ProcessPlmLineForTheFirstHomonym(strPlmLine, S.GetOpt()->m_Language, OborotNo))
				return false;
			Word.m_bHasSpaceBefore = 	S.m_Words.empty() 
									||	S.m_Words.back().m_GraphematicalUnitOffset+S.m_Words.back().m_strWord.length() < Word.m_GraphematicalUnitOffset
									||	S.m_Words.back().m_bSpace;
            S.AddSynWord(Word); 			
			
		}
		else
		{	
            if (!S.m_Words.empty()) // первая строка иногда содержит всякую информацию про документ
                if (!S.m_Words.back().AddNextHomonym(strPlmLine))
				    return false;
		}

	}

	if (!BuildAuxSimpleGroups (S)) return false;

	return true;


};


/*
This function recieves a list of plm lines(piPLMLinePtr) and pointer to a line (LineNo)
It finds the next sentence delimeter, checking that a sentence should have at least one word
The function returns false if a parse  error occurs while reading. If no error, the function 
changes LineNo, which   should point to the end position of the next sentence.
*/
bool CSentence::ReadNextFromPlmLines(const CPlmLineCollection* piPLMLinePtr, size_t& LineNo)
{
	try
	{
		//  a cycle till a non-empty sentence is found or  the end is reached
		while(m_Words.empty() && (LineNo < piPLMLinePtr->m_Items.size()) )
		{
				// if an error occurs then exit with failure 
				if (!ReadSentence(*this, piPLMLinePtr, LineNo)) return false;
		}
		
		// language  specific processings 
		ReadNextFromPlmLinesLanguageSpecific();


		return true;
	}
	catch(...)
	{
		Reset();
		return false;
	}
}






void CSentence::FindGraPairs()
{

	SGraPair graPair;

	for(int i = 0 ; i < m_Words.size() ; i++ )
	{
		if( m_Words[i].IsFirstOfGraPair(Date) )			
			m_vectorGraPairs.push_back(SGraPair(i,-1,Date));

		if( m_Words[i].IsFirstOfGraPair(WebAddr) )			
			m_vectorGraPairs.push_back(SGraPair(i,-1,WebAddr));


		if( m_Words[i].IsFirstOfGraPair(Keyb) )			
			m_vectorGraPairs.push_back(SGraPair(i,-1,Keyb));

		if( m_Words[i].IsFirstOfGraPair(GermanDividedCompound) )			
			m_vectorGraPairs.push_back(SGraPair(i,-1,GermanDividedCompound));

		if( m_Words[i].IsFirstOfGraPair(Oborot) )			
			m_vectorGraPairs.push_back(SGraPair(i,-1,Oborot, m_Words[i].GetOborotNo()));
		

		for(int j = 0 ; j < m_vectorGraPairs.size() ; j++)
			if( (m_vectorGraPairs[j].m_iW2 == -1) && m_Words[i].IsSecondOfGraPair(m_vectorGraPairs[j].m_type) )
				m_vectorGraPairs[j].m_iW2 = i;		
	}

	for(int j = m_vectorGraPairs.size() - 1  ; j >= 0  ; j--)
		if( (m_vectorGraPairs[j] .m_iW1 == -1) || (m_vectorGraPairs[j] .m_iW2 == -1))

				m_vectorGraPairs.erase(m_vectorGraPairs.begin() + j);
}






void CSentence::RecalculateRelationsCoordinates()
{
	try
	{
		for(int i = 0; i < GetClausesCount() ; i++ )	
		{
			GetClause(i).RecalculateRelationsCoordinates(i);
		};
		

		for(int i = 0; i < m_ClausesRelations.size() ; i++ )	
		{
			CRelation& rel = m_ClausesRelations[i];
			rel.m_SourceClause.m_iClauseNo = FindClauseIndexByPeriod(rel.m_SourceClause.m_ClausePeriod);
			rel.m_TargetClause.m_iClauseNo = FindClauseIndexByPeriod(rel.m_TargetClause.m_ClausePeriod);
		}
	}
	catch(...)
	{
		
		OutputErrorString("RecalculateRelationsCoordinates failed!!! ");
	}
}






bool CSentence::KillHomonymsInAllSentence()
{
	assert (IsValid());
	bool bResult = false;
	try
	{
		int ii = GetClausesCount();
		for(int i = 0 ; i < ii ; i++ )
		{
			CClause& C =  GetClause(i);
			if (m_pSyntaxOptions->m_KillHomonymsMode == CoverageKillHomonyms)
			{
			  if (C.KillHomonymsCoverage())
				  bResult = true;
			}
		};
			

		return bResult;
	}
	catch(...)
	{
		return false;

	}

}




bool CSentence::DeleteMarkedHomonymsWithClauses(int iWord)
{
	for(int i = 0 ; i < m_Clauses.size() ; i++)
	{
		if(		(m_Clauses[i].m_iFirstWord <= iWord)
			&&	(m_Clauses[i].m_iLastWord  >= iWord) )
		{
            size_t SaveHomonymsCount = GetWords()[i].m_Homonyms.size();
			m_Clauses[i].DeleteMarkedHomonyms(iWord);
			return SaveHomonymsCount < GetWords()[i].m_Homonyms.size();
		}
	}
    return false;
}

void CSentence::DontDeleteExpr2ifExpr1Exists(const CPeriod& P)
{
    // if homonym with EXPR1 exists then there should be homonym with EXPR2
    // если в омоним начинает  многословный оборот, надо найти конец оборота и не удалять его 
    for(int i = P.m_iFirstWord ; i <= P.m_iLastWord ; i++)
        for (size_t j=0; j < m_Words[i].GetHomonymsCount(); j++)
            if (     m_Words[i].GetHomonym(j)->IsOb1() 
                &&  !m_Words[i].GetHomonym(j)->IsOb2()
                &&  !m_Words[i].GetHomonym(j)->m_bDelete)
            {
                for(int k = i+1 ; k <= P.m_iLastWord ; k++)
                {
                    bool bFoundEndOborot = false;
                    for (size_t l=0; l < m_Words[k].GetHomonymsCount(); l++)
                        if (m_Words[k].GetHomonym(l)->IsOb2())
                        {
                            m_Words[k].GetHomonym(l)->m_bDelete = false;
                            bFoundEndOborot = true;
                        }
                    if (bFoundEndOborot) break;
                }
            }
}

bool CSentence::DeleteMarkedHomonymsWithClauses(const CPeriod& P)
{
    DontDeleteExpr2ifExpr1Exists(P);
    bool bResult = false;
	for(int i = P.m_iFirstWord ; i <= P.m_iLastWord; i++)
		bResult = bResult || DeleteMarkedHomonymsWithClauses(i);
    return bResult;
}







//initializing slot CSynHomonym::m_SimplePrepNos for each prepositions
//If graphematics has found the same oborot then we clear oborot marks.
//So we delete oborot interpretation for each simple preposition.
void CSentence::AscribeSimplePrepositionInterpretations(BYTE PREP_POS)
{
	for (int i =0; i < m_Words.size(); i++)
		for(int k = 0 ; k < m_Words[i].GetHomonymsCount() ; k++ )
		{
			CSynHomonym& H = m_Words[i].GetSynHomonym(k);

			if	(H.HasPos(PREP_POS))
			{
				H.m_SimplePrepNos = GetOpt()->m_pOborDic->FindAllArticlesForSimplePrep(H.m_strLemma);
				if (H.IsOb1() && H.IsOb2())
					m_Words[i].DeleteOborotMarks();
			};
		};
};


void CSentence::AssignClauseNoToWords()
{
	for (long WordNo = 0; WordNo <  m_Words.size(); WordNo++)

		m_Words[WordNo].m_iClauseNo = -1;

	for(int i = 0 ; i < GetClausesCount() ; i++ )
	{
		const CClause& clause = GetClause(i);
		for(int k = clause.m_iFirstWord ; k <= clause.m_iLastWord ; k++ )		
			if( m_Words[k].m_iClauseNo == -1 )
				m_Words[k].m_iClauseNo = i;		
	}
}






void	CSentence::CalculatePrimitiveClausesCount()
{
	map<int, int> map_cl;

	if( m_vectorPrClauseNo.size() > 0 )
		return;
	for(int i = 0 ; i < m_Words.size() ; i++ )	
	{
		pair< map<int, int>::iterator, bool> ret;
		ret = map_cl.insert(map<int, int>::value_type(m_Words[i].m_iClauseNo,0));
		if( ret.second )
			m_vectorPrClauseNo.push_back(m_Words[i].m_iClauseNo);
	}	
}
long  CSentence::GetPrimitiveClauseNoByClauseNo(long lClauseNo) const 
{
	for(int i = 0 ; i < m_vectorPrClauseNo.size() ; i++ )
		if( m_vectorPrClauseNo[i] == lClauseNo )
		{
			return i;
		}

	return -1;
}


bool  CSentence::GetHomonymByClauseVariantInterface(long iWord, long iVar, long iClause, CSynHomonym& H) const  
{
	H = CSynHomonym(this);

	CSVI pSynVar = m_Clauses[iClause].m_SynVariants.begin();

	for (int i = 0; i < iVar; i++)
		pSynVar++;

	return GetHomonymByClauseVariant(iWord, pSynVar, H);
}

bool CSentence::GetHomonymByClauseVariant(long iWord, CSVI pSynVar, CSynHomonym& H)  const
{
	if( (iWord < 0) || (iWord >= m_Words.size()) )
			return false;

	const CSynWord& word = m_Words[iWord];

	int iUnit = pSynVar->UnitNoByWordNo(iWord);
	if( iUnit == -1 )
		return false;

	if ( pSynVar->GetUnitType(iUnit) != EWord)
		return false;

	return GetHomonymByClauseVariantWithModifiedProperties(iUnit, word, *pSynVar, H);
}






//	если  число слов в предложении больше 400 или больше половины слов в предложении не русские слова,
//	тогда  синтаксис входит в "панический"  режим.


bool CSentence::IsPanicSentence()
{
	if (m_Words.size() < 100) return false;

	if (m_Words.size() > 400)
		return true;

	int CountOfRussianLexems = 0;
	for (int i = 0; i < m_Words.size(); i++)
        if( m_Words[i].HasDes(ORLE) )
			CountOfRussianLexems++;

	return CountOfRussianLexems*2 < m_Words.size();

};



void CSentence::RecalculateIndicesAfterInsertingWord(int InsertWordNo)
{
	
	for (int i = 0; i < GetClausesCount(); i++)
		GetClause(i).RecalculateIndicesAfterInsertingWord(InsertWordNo);


	for (int i = 0; i < m_vectorTermins.size(); i++)
	{
		if (m_vectorTermins[i].m_iFirstWord >= InsertWordNo)
			m_vectorTermins[i].m_iFirstWord++;
		if (m_vectorTermins[i].m_iLastWord >= InsertWordNo)
			m_vectorTermins[i].m_iLastWord++;
	}

	for (int i = 0; i < m_Words.size(); i++)
	{
		CSynWord& W = m_Words[i];
		for (int j = 0; j < W.m_MainVerbs.size(); j++)
			if (W.m_MainVerbs[j] >= InsertWordNo)
				W.m_MainVerbs[j]++;

		if (W.m_TrennbarePraefixWordNo >= InsertWordNo)
			W.m_TrennbarePraefixWordNo++;
	};
}

bool CSentence::CheckSubclausePredicatePositionForAllRoots(const CClause& C)
{
	if (GetOpt()->m_Language == morphRussian) return true;

	for (size_t i=0; i< C.m_vectorTypes.size(); i++)
		if (!C.m_vectorTypes[i].m_Root.IsEmpty())
			if (CheckLastPredicatePosition(C.m_iLastWord, C.m_vectorTypes[i].m_Root.m_WordNo))
				return true;

	return false;
};

bool CSentence::EncloseClauseAsWord(int iWhat, int iWhere)
{
	CClause* pPrimWhat = &GetClause(iWhat);
	assert(pPrimWhat);
	if (!CheckSubclausePredicatePositionForAllRoots(*pPrimWhat))
		return false;
	

	CClause* pPrimWhere = &GetClause(iWhere);	
	assert(pPrimWhere);

	rml_TRACE ("enclose %s into %s\n", pPrimWhat->GetTraceStr().c_str(), pPrimWhere->GetTraceStr().c_str());


	CPeriod PeriodPrimWhere = *pPrimWhere;
	
	if( GladkijPeriodLess(*pPrimWhere,*pPrimWhat) )
	{
		PeriodPrimWhere.m_iLastWord = pPrimWhat->m_iLastWord;
		vector<int> Parents;
		GetClausesByLastWord (Parents, pPrimWhere);
		for (int i = 0; i < Parents.size(); i++)
			m_Clauses[Parents[i]].m_iLastWord = pPrimWhat->m_iLastWord;
	}
	else
	{
		PeriodPrimWhere.m_iFirstWord = pPrimWhat->m_iFirstWord;
		vector<int> Parents;
		GetClausesByFirstWord (Parents, pPrimWhere);
		for (int i = 0; i < Parents.size(); i++)
			m_Clauses[Parents[i]].m_iFirstWord = pPrimWhat->m_iFirstWord;
	};

	
	
	
	
	SortClauses();
	TraceClauses();


	// поскольку pPrimWhere могла быть вложена и сейчас она меняет свои границы, нужно перестроить 
	// варианты все надклауз, которые стоят выше  pPrimWhere. 
	// Само собой  разумеется, нужно пересторить варианты  самой  PrimWhere. Если  PrimWhere не была вложена, 
	//  тогда changed_clauses будет содержать только PrimWhere.
	// В принципе, возможен случай, когда pPrimWhat вложена, но это означает, что у PrimWhere и pPrimWhat одинаковый 
	// непосредственный  хоязин
	
	for (int i =0; i< m_Clauses.size(); i++)
		if (PeriodPrimWhere.is_part_of(m_Clauses[i]) )
		{
			if (GetOpt()->m_Language == morphGerman)
				BuildGLRGroupsInClause(GetClause(i));
			else
				m_Clauses[i].BuildSynVariants();
		};

	return true;
}


  //CreateEnclosedNotStrongClause creates a new enclosed clause in MainClauseNo.
  //After this function CSentence::m_Clauses will be one clause longer.
  //the start start of the new clause will be  iFWrd;
  //the last word  of the new clause will be  iLWrd;
  //the clause type will be ClType;

bool CSentence::CheckTerminIntersection(int iFWrd, int iLWrd) const
{
    CPeriod Clause(iFWrd, iLWrd);
    for (size_t i=0; i < m_vectorTermins.size(); i++)
    {
        const SFoundTermin& T = m_vectorTermins[i];
        if (T.has_intersection(Clause) || Clause.is_inside_of(T))
            return true;
    }
    return false;
}

bool CSentence::CreateEnclosedNotStrongClause(int MainClauseNo, int iFWrd, int iLWrd, SClauseType ClType)
{
	if (CheckTerminIntersection(iFWrd, iLWrd)) 
    {
        return false;
    }
    assert (!ClType.m_Root.IsEmpty());
	assert (ClType.m_Root.m_HomonymNo != -1);

	const CSynHomonym& H = m_Words[ClType.m_Root.m_WordNo].m_Homonyms[ClType.m_Root.m_HomonymNo];
	assert (!GetOpt()->GetGramTab()->IsStrongClauseRoot(H.m_iPoses));

	CClause* pMainClause  = &GetClause(MainClauseNo);
	CClause pNewClause(pMainClause->m_pSent, iFWrd, iLWrd);

	
	// deletes all strong homonyms in new subclause if it is possible, if not
	// then exit with "false"
	for (int i = pMainClause->m_vectorTypes.size()-1; i >= 0; i--)
	{
		//  do not use here  const CWordAndHomonym&, the value can be deleted!
		const CWordAndHomonym W = pMainClause->m_vectorTypes[i].m_Root;
		

		if	(		!W.IsEmpty()
				&&	GetOpt()->GetGramTab()->IsStrongClauseRoot( m_Words[W.m_WordNo].GetSynHomonym(W.m_HomonymNo).m_iPoses ) 
				&&	CPeriod(W.m_WordNo).is_part_of(pNewClause)
				
			)
		{

			assert(!(W == ClType.m_Root));
			// we cannot delete the last homonym of the word 
			if (m_Words[W.m_WordNo].m_Homonyms.size() == 1) return false;
			pMainClause->DeleteHomonym(W.m_WordNo, W.m_HomonymNo);

			// update index in ClType
			if (ClType.m_Root.m_WordNo == W.m_WordNo)
				if (ClType.m_Root.m_HomonymNo > W.m_HomonymNo)
					ClType.m_Root.m_HomonymNo--;
		};



	};
	

	assert(pMainClause);
	CPeriod SaveMainPeriod(*pMainClause);
	CFormatCaller* pFmtCall = GetNewFormatCaller();
	pFmtCall->AddAllRules();
	

	// creating subclause and building groups for it
	pNewClause.m_vectorTypes.push_back(ClType);
	rml_TRACE ("create clause %s\n", pNewClause.GetTraceStr().c_str());
	CClause& TheAddedClause  = AddClause(pNewClause); 
	
	if (GetOpt()->m_Language == morphGerman)
		BuildGLRGroupsInClause(TheAddedClause);
	else
		TheAddedClause.BuildGroups(*pFmtCall, false);		
		

	
	// rebuilding groups in the main clause
	pMainClause = FindClauseByPeriod(SaveMainPeriod);
	assert(pMainClause != NULL);
	pMainClause->m_SynVariants.clear();
	pFmtCall->Reset();
	
	if (GetOpt()->m_Language == morphGerman)
		BuildGLRGroupsInClause(*pMainClause);
	else
		pMainClause->BuildGroups(*pFmtCall, true);

	delete  pFmtCall;

	return true;
}



bool CSentence::RunClauseRule(const CClauseRule* it, int iClauseNum)
{


	FirstGrRoule  rule = it->m_Rule;

	bool bResult = (this->*rule)(iClauseNum);

	if (bResult)
		rml_TRACE("Rule %s has changed the structure\n", it->m_Description.c_str());

	return bResult;
}



struct CPeriodAndRuleNo {
	CPeriod	m_Period;
	int m_RuleNo;
	bool operator < (const CPeriodAndRuleNo X) const
	{
		if (m_RuleNo != X.m_RuleNo)
			return m_RuleNo < X.m_RuleNo;

		return StandardPeriodLess(m_Period,X.m_Period);
	};
	bool operator == (const CPeriodAndRuleNo X) const
	{
		return		(m_RuleNo == X.m_RuleNo)
				&&	m_Period ==X.m_Period;
	};
};

void CSentence::OneRunOfClauseRules(const vector<CClauseRule>& ListOfRules)
{
	set<CPeriodAndRuleNo> AlreadyRunRuleForThisClause;
    IsValid();
	
	TraceClauses();
	bool bChanged = false;
	do 
	{
		bChanged = false;
	
		for(int RuleNo = 0 ; RuleNo < ListOfRules.size(); RuleNo++)

			for( int ClauseNo = GetClausesCount() - 1 ; ClauseNo>= 0 ; ClauseNo--)
			{
				char ErrorMessageStr[100];
				sprintf (ErrorMessageStr,"Fragmentation rule failed (ClauseNo = %i; RuleNo= %s)",ClauseNo, ListOfRules[RuleNo].m_Description.c_str());
				try {
					const CClause& pClause = GetClause(ClauseNo);
					bool bThisRuleChangedTheStructure = false;
					CPeriodAndRuleNo P;
					P.m_Period = m_Clauses[ClauseNo];
					P.m_RuleNo = RuleNo;
					if (AlreadyRunRuleForThisClause.find(P) == AlreadyRunRuleForThisClause.end())
					{
						bThisRuleChangedTheStructure = RunClauseRule(&ListOfRules[RuleNo], ClauseNo);

						if (!IsValid())
						{
							OutputErrorString(ErrorMessageStr);
							return;
						};
						AlreadyRunRuleForThisClause.insert(P);
						//  th rule can decrease  the number of clauses 
						if (ClauseNo >= GetClausesCount())
							ClauseNo = GetClausesCount() - 1;

					}
					else
					{
						// the rule  was already applied
						int debug = 1;
					};
					if (bThisRuleChangedTheStructure)
					{
						bChanged  = true;
						TraceClauses();
					};
				}
				catch (...)
				{
					OutputErrorString( ErrorMessageStr );
					return;
				};
			}
	}
	while (bChanged);

	

}




 // функция копирует 
	//типы из правой клаузы;
 //   знаки препинания  из правой клаузы;
	//соч. союзы  из левой клаузы;
	//подч. союзы   из обеих клауз;

void CopyParametersFromRight (CClause& newClause, const CClause* pClause1, const CClause* pClause2)
{
	newClause.m_vectorTypes = pClause2->m_vectorTypes;
	newClause.m_iPunctSignsCount = pClause2->m_iPunctSignsCount;
	if (pClause1->IsRelative())
	{
		newClause.m_RelativeWord = pClause1->m_RelativeWord;
		newClause.m_AntecedentWordNo =  pClause1->m_AntecedentWordNo;

	};

	newClause.m_vectorConjs = pClause1->m_vectorConjs;


	for(int i = 0 ; i < pClause2->m_vectorConjs.size() ; i++ )
	{
		if( pClause2->m_vectorConjs[i].m_FromWhere == FROM_SUB_CONJ )
			newClause.m_vectorConjs.push_back(pClause2->m_vectorConjs[i]);

		if(pClause2->m_vectorConjs[i].m_FromWhere == FROM_OBOR_DIC )
		{
			int index = pClause2->m_vectorConjs[i].m_index;
			if( pClause2->GetOpt()->m_pOborDic->m_Entries[index].m_ConjType == sub_conj )
				newClause.m_vectorConjs.push_back(pClause2->m_vectorConjs[i]);
		}
	}
	
}

static void CopyCharacreristics(CClause& C, const CClause* pFromClause)
{
	C.m_vectorConjs = pFromClause->m_vectorConjs;
	C.m_iPunctSignsCount = pFromClause->m_iPunctSignsCount;
	C.m_vectorTypes = pFromClause->m_vectorTypes;
	C.m_RelativeWord = pFromClause->m_RelativeWord;
	C.m_AntecedentWordNo = pFromClause->m_AntecedentWordNo;
	C.m_pSent = pFromClause->m_pSent; 
}



//После работы функций UniteClauses, CreateEnclosedNotStrongClause,  EncloseClauseAsWord
//все ссылки на клаузы становятся невалидными

CClause& CSentence::UniteClauses(int iLeftClause, int iRightClause, ParametersSourceEnum whose_chars_to_copy)
{
	CClause* pOldClauseL = &GetClause(iLeftClause);
	CClause* pOldClauseR = &GetClause(iRightClause);

	assert (pOldClauseL->m_iLastWord+1 == pOldClauseR->m_iFirstWord);

	rml_TRACE ("unite %s with %s\n", pOldClauseL->GetTraceStr().c_str(), pOldClauseR->GetTraceStr().c_str());

	CClause newClause (pOldClauseR->m_pSent, pOldClauseL->m_iFirstWord, pOldClauseR->m_iLastWord);

	if( whose_chars_to_copy == LeftClauseParams)
		CopyCharacreristics(newClause, pOldClauseL);
	else
		CopyParametersFromRight(newClause,pOldClauseL,pOldClauseR);		

	vector<int> LeftParents;
	GetClausesByLastWord (LeftParents, pOldClauseL);	

	vector<int> RightParents;
	GetClausesByFirstWord (RightParents, pOldClauseR);	

	

	//это очень хитрый  assert. Нормально мы сливаем две невложенные клаузы, если обе клаузы не вложены, тогда
	//должно выполняться assert ((LeftParents.size() == 1)  &&  (RightParents.size() == 1));
	//однако мы рарешаем сливаться  вложенным клаузам, но только тогда, когда
	// 1. Либо они непосредственно вложены в одну клаузу.
	// 2. Либо одна из клауз  не является  вложенной. 
	// Выражение assert ((LeftParents.size() == 1)  ||  (RightParents.size() == 1)); 
	// как раз обеспечивает проверку  вышеуказанных условий, 
	// поскольку одна правая клауза  идет сразу за левой.
	assert ((LeftParents.size() == 1)  ||  (RightParents.size() == 1));

	vector<CPeriod> changed_clauses;

	changed_clauses.push_back(newClause);
	

	if (LeftParents.size() > 1)
	{
		assert (m_Clauses[LeftParents[0]] == *pOldClauseL);
		for (int i = 1; i< LeftParents.size(); i++)
		{
			m_Clauses[LeftParents[i]].m_iLastWord = newClause.m_iLastWord;
			changed_clauses.push_back(m_Clauses[LeftParents[i]]);
		};

		// adding clause which is the first common superclause of  pOldClauseL and pOldClauseR
		int FirstHostNo = GetMininalParentByClauseNo(iRightClause);
		if (FirstHostNo != -1)
			changed_clauses.push_back(m_Clauses[FirstHostNo]);

	}
	else
	if (RightParents.size() > 1)
	{
		assert (m_Clauses[RightParents[0]] == *pOldClauseR);
		for (int i = 1; i< RightParents.size(); i++)
		{
			m_Clauses[RightParents[i]].m_iFirstWord = newClause.m_iFirstWord;
			changed_clauses.push_back(m_Clauses[RightParents[i]]);
		};

		// adding clause which is the first common superclause of  pOldClauseL and pOldClauseR
		int FirstHostNo = GetMininalParentByClauseNo(iLeftClause);
		if (FirstHostNo != -1)
			changed_clauses.push_back(m_Clauses[FirstHostNo]);
	}
	else
	{
		// adding clause which is the first common superclause of  pOldClauseL and pOldClauseR
		int FirstHostNo = GetMininalParentByClauseNo(iRightClause); //  can be iLeftClause
		if (FirstHostNo != -1)
			changed_clauses.push_back(m_Clauses[FirstHostNo]);
	};
	
	DeleteClause(iRightClause);	
	DeleteClause(iLeftClause);
	SortClauses();

	CClause& retClause = AddClause(newClause);	
	for (int i = 0 ; i < changed_clauses.size(); i++)
	{
		int c = FindClauseIndexByPeriod(changed_clauses[i]);
		assert (c != -1);
		if (GetOpt()->m_Language == morphGerman)
			BuildGLRGroupsInClause(m_Clauses[c]);
		else
			m_Clauses[c].BuildSynVariants();


	};

	TraceClauses();
	return retClause;

}


// проверяет стоит ли на месте iWrd вложенная клауза
bool CSentence::IsWordClause(const CClause& rClause, int iWrd) const
{	
	if (rClause.m_SynVariants.empty()) return false;

	int iUnit = rClause.UnitNoByWordNo(iWrd);
	
	return rClause.m_SynVariants.begin()->m_SynUnits[iUnit].m_Type == EClause;
}



void CSentence::MarkWordAsDeleted(int iDelWrd)
{
	m_Words[iDelWrd].m_bDeleted = true;
	m_Words[iDelWrd].BuildTerminalSymbolsByWord();
}


bool CSentence::IsInNonAtomicOborPairs(int iW) const
{
	for(int i = 0 ; i < m_vectorGraPairs.size() ; i++ )
		if( m_vectorGraPairs[i].m_type == Oborot )
			if (m_vectorGraPairs[i].m_iW1 !=  m_vectorGraPairs[i].m_iW2)
				if( (m_vectorGraPairs[i].m_iW1 <= iW) && (m_vectorGraPairs[i].m_iW2 >= iW) )
					return true;
	return false;
}


int	CSentence::GetCoordConjNo(const char* WordUpper) const
{
	const vector<CCoordConjType>& C = GetOpt()->m_pOborDic->GetCoordConjs();
	vector<CCoordConjType>::const_iterator it = find(C.begin(),C.end(), WordUpper);
    if (it == C.end()) 
        return -1;
    else
	    return it - C.begin();
};










int CSentence::FindWordWithOneHomonym(int iFirstWord, int iLastWord, BYTE pos) const
{
	
	for(int i = iFirstWord ; i <= iLastWord ; i++)
	{
		if(m_Words[i].GetHomonymsCount() == 1)
            if( m_Words[i].GetHomonym(0)->HasPos(pos))
				return i;
	}
	return -1;
}


bool GetHomonymByClauseVariantWithModifiedProperties (int iUnit, const CSynWord& word, const CMorphVariant& syn_var, CSynHomonym& H)
{
	long lHomNum;
	lHomNum = syn_var.GetHomNum(iUnit);

	H = word.GetSynHomonym(lHomNum);

	// setting grammems 
	H.CopyAncodePattern( syn_var.m_SynUnits[iUnit] );
	H.m_SimplePrepNos =  syn_var.GetSimplePrepNos(iUnit);
	return true;
};


bool FindAntecedentForRelativePronoun(CSentence& C, int iClauseNum, int MainClauseNo)
{
	CClause* pClause = &C.GetClause(iClauseNum);

	CSynWord& pConj = C.GetWords()[pClause->m_RelativeWord.m_WordNo];
	if (MainClauseNo == -1) return false;
	CClause& MainClause = C.GetClause(MainClauseNo);

	// going to the left
	for(int WordNo = MainClause.m_iLastWord ; WordNo >= MainClause.m_iFirstWord ; WordNo-- )
	{
		CSynWord& pWord = C.GetWords()[WordNo];

		const CSynHomonym& RelativHomonym = pConj.m_Homonyms[pClause->m_RelativeWord.m_HomonymNo];
		

		for(CSVI pSynVar = MainClause.m_SynVariants.begin() ; pSynVar != MainClause.m_SynVariants.end(); pSynVar++ )
		{	
			int UnitNo = MainClause.UnitNoByWordNo(WordNo);
			if (UnitNo == -1) continue;
			int HomonymNo = pSynVar->m_SynUnits[UnitNo].m_iHomonymNum;

			// we are in subclause 
			if (HomonymNo == -1) continue;

			bool bSimilarGroup = false;
			{
				const CGroup* P = MainClause.GetLastHost(WordNo, pSynVar);
				if (P)
				{
					if ( P->m_GroupType == C.GetOpt()->m_SimilarNPGroupType )
						bSimilarGroup = true;
				};
			};


			if( C.CanBeRelativeAntecedent(pWord.m_Homonyms[HomonymNo]) )
                if (!pWord.HasOborot1() && !pWord.HasOborot2())
				if(     C.GetOpt()->GetGramTab()->GleicheGenderNumber( RelativHomonym.m_GramCodes.c_str(), pWord.m_Homonyms[HomonymNo].m_GramCodes.c_str()) 
					|| (		bSimilarGroup
						 &&		(RelativHomonym.m_iGrammems  & C.GetOpt()->m_PluralMask)
						)
				  ) 
				{
					pWord.SetAllOtherHomsDel(HomonymNo);
					C.DeleteMarkedHomonymsWithClauses(WordNo);

					pConj.SetAllOtherHomsDel(pClause->m_RelativeWord.m_HomonymNo);
					C.DeleteMarkedHomonymsWithClauses(pClause->m_RelativeWord.m_WordNo);
					pClause->m_RelativeWord.m_HomonymNo = 0;

					// transfering  number from the antecedent to the relative pronoun
					//  if there is no ambiguity.
					{
						QWORD Grammems = 0;
						//  take grammems  from the word
						for (size_t i=0; i < pWord.m_Homonyms.size(); i++)
							Grammems |= pWord.m_Homonyms[i].m_iGrammems;

						//  if it is inside a group, then take grammems  form the group
						for (int i=pSynVar->m_vectorGroups.GetGroups().size()-1; i >=0; i--)
						{
							const CGroup& G =  pSynVar->m_vectorGroups.GetGroups()[i];
							if (G.m_MainWordNo == WordNo)
							{
								Grammems = G.GetGrammems();
								break;
							};
						};

						if	(		(C.GetOpt()->m_PluralMask & Grammems) 
								&& !(C.GetOpt()->m_SingularMask & Grammems) 
							)
							 //  only plural
							 Grammems = C.GetOpt()->m_PluralMask;
						else
							if	(		(C.GetOpt()->m_SingularMask & Grammems) 
									&& !(C.GetOpt()->m_PluralMask & Grammems) 
								)
								//  only singular
								Grammems = C.GetOpt()->m_SingularMask;
							else
								Grammems = 0;

						if (Grammems)
						{
							CSynWord& RelWord = C.m_Words[pClause->m_RelativeWord.m_WordNo];
							for (size_t i=0; i < RelWord.m_Homonyms.size(); i++)
                                RelWord.m_Homonyms[i].ModifyGrammems( Grammems  );
						};


					};

					pClause->m_AntecedentWordNo = WordNo;
					
					

					return C.EncloseClauseAsWord(iClauseNum, MainClauseNo );
				}
		}
	}	
	return false;
};


bool CSentence::RuleForRelativeClause(int iClauseNum)
{
	CClause* pClause = &GetClause(iClauseNum);

	if ( IsEnclosedClause (iClauseNum) )  return false;

	//  If it is not a relative clause, then this procedure was called 
	//  at the main clause,  we should deal with such cases, since 
	//  the relative clause could  be already precessed and failed, then 
	//  the main clause has changed,and then  RuleForRelativeClause is called one more time
	//  for the main clause.
	//  for example: Paul und Peter, die immer dabei sind, gehen nach Hause.
	if( !pClause->IsRelative() )
	{
		iClauseNum = GetMaxClauseByFirstWord(pClause->m_iLastWord + 1);
		if (iClauseNum == -1) return false;
		pClause = &GetClause(iClauseNum);

		if( !pClause->IsRelative() )
			return false;
	};
	//  prohibit relative clauses, whihc has only one type: infinitive
	// "Um dem Ziele nдherzukommen, die Angehцrigen zu verschmelzen," 
	if (		(pClause->m_vectorTypes.size() == 1)
			&&	(pClause->m_vectorTypes[0].m_Type == GetOpt()->m_InfintiveClauseType)
		)
		return false;

	//  prohibit empty relative clauses at the end 
	// "Die RAF. hat, abgesehen von dem Zusammenwirken bei diesen Angriffen, der Armee besondere Dienste geleistet." 
	if (		pClause->m_vectorTypes.empty()
			&&	(pClause->m_iLastWord+1 == m_Words.size())
		)
		return false;

	//  prohibit enclosing if the clauses contain two parts of one analytical form
	for (size_t WordNo=pClause->m_iFirstWord; WordNo <= pClause->m_iLastWord; WordNo++)
	{
		int wrd = FindFirstAuxVerb(WordNo);
		if (wrd != -1)
			if (wrd < pClause->m_iFirstWord)
				return false;
	};


	// if clause contains only pronoun and comma, then exit
	if (pClause->size() <= 2) return false;
	if (pClause->m_iFirstWord == 0) return false;

	int MainClauseNo = GetMinClauseByLastWord(pClause->m_iFirstWord - 1);
	
	if (FindAntecedentForRelativePronoun(*this, iClauseNum, MainClauseNo))
		return true;

	//  cooordination of relative clauses
	// "Dienste, die wichtig sind, die aber nicht unbedingt vom Staat geleistet werden mьssen,"
	if (		(m_Clauses[MainClauseNo].IsRelative())
			&&	IsEnclosedClause(MainClauseNo)
		)
	{
		MainClauseNo = GetMininalParentByClauseNo(MainClauseNo);
		assert (MainClauseNo != -1);
		return FindAntecedentForRelativePronoun(*this, iClauseNum, MainClauseNo);
		
	};

	return false;
}


bool CSentence::RuleForUnitingEmptyClauseWithStrongLeftBorder(int iClauseNum)
{
	const CClause& pClause1 = GetClause(iClauseNum);
	if ( !pClause1.m_vectorTypes.empty() ) return false;

	int iWord = pClause1.m_iFirstWord;
	if (!pClause1.HasLeftStarter() )
		return false;

	int iNxt = GetMaxClauseByFirstWord(pClause1.m_iLastWord+1);
	if (iNxt == -1) return false; 


	const CClause& pClause2 = GetClause(iNxt);		
	
	if ( pClause2.HasLeftStarter() ) return false;

	if ( pClause2.HasOnlyOneSimConjFromOborDic() ) return false;


	if (GetOpt()->m_Language == morphRussian)
	{
		bool bDash  =		(		(GetWords()[pClause2.m_iFirstWord].m_strWord == "-")
							||	(
										m_Words[pClause2.m_iFirstWord].m_bComma
									&& pClause2.m_iLastWord>pClause2.m_iFirstWord
									&& ( GetWords()[pClause2.m_iFirstWord+1].m_strWord == "-")
								)
						)
					&&	pClause2.m_vectorTypes.empty();
		//  for dash should be a special rule
		if (bDash) return false;
	};


	int iEncClL = GetMaxChildByLastWord(pClause1, pClause1.m_iLastWord);
	int iEncClR = GetMaxChildByFirstWord(pClause2,pClause2.m_iFirstWord);
	

	if (iEncClL != -1 || iEncClR != -1)
	{
		CClause& newClause = UniteClauses( iClauseNum, iNxt, RightClauseParams);
		return true;

	}

	return false;
}


bool CSentence::IsStrictLeftClauseBorder(CPeriod P) const 
{
	int iWord = P.m_iFirstWord;
	// the first word of the sentence
	if( iWord == 0)
		return true;

	// the last word of the sentence
	if(iWord == GetWords().size() - 1)
		return true;

	// a semicolon is a strict delimiter
	if( m_Words[iWord].m_strWord[0] ==  ';' )
		return true;

	// a semicolon in the second position is a strict delimiter
	if(iWord  < GetWords().size() - 1)
		if( m_Words[iWord+1].m_strWord[0] ==  ';' )
			return true;

	// an open bracket is a strict delimiter
	// if there is no close brackets inside the Period
    if( m_Words[iWord].HasDes(OOpn) )
	{
		// go until the last word
		size_t i=iWord+1;
		for (;  i < P.m_iLastWord; i++)
		{

            if( m_Words[iWord].HasDes(OCls) )
				break;
		};
		if (i != P.m_iLastWord)
			return true;
	};


		//multiple punctuation marks are a stict delimiter (Russian direct speech):
		//"... сказала она, - Петя."
    if( m_Words[iWord].HasDes(OPun) )
	  if (iWord  < m_Words.size() - 1)
          if( m_Words[iWord+1].HasDes(OPun) )
			return true;


	return false;
}


	//клонирование  оборотов сделано для того, чтобы  возникла омонимия между  оборотом и не оборотом. 
	//Например, на отрезке "то есть" найден оборот(союз). На этом же отрезке моежт быть построено подлежащее 
	//и сказуемое. В итоге,  мы должны получить два равноправных варианта: с оборотом и c  подлежащим и сказуемым.

void CSentence::CloneHomonymsForOborots()
{
	//  идем по всем графематическим вилкам, 
	for( int i = 0 ; i < m_Words.size() ; i++ )	
    {
        CSynWord& FirstWord = m_Words[i];
        if (!FirstWord.HasOborot1()) continue;
        
        if (!FirstWord.HasOborot2())
		{
      		// многословные обороты
            for(int j = i ; j < m_Words.size() ; j++ )							
            {
				m_Words[j].CloneHomonymForOborot();			
                if (m_Words[j].HasOborot2()) break;
            }
		}
		else
		{
			// однословные обороты
			for(int k = 0 ; k < FirstWord.GetHomonymsCount() ; k++ )
			{
				CSynHomonym& H = FirstWord.GetSynHomonym(k);
				if (H.m_OborotNo != -1)
                {
				    const COborotForSyntax& O = GetOpt()->m_pOborDic->m_Entries[H.m_OborotNo];
					    //Проверяем соответствие найденных  графематикой   оборотов и морфологии.
					    //Для омонимов, которые не являются союзом, предлогом, вводным словом или наречием ("GF      = 1  НАР")
					    //интерпретация  в словаре оборотов удаляется. Так, например, для омонима "ПОСОЛ" слова "после" 
					    //оборотная интерпретация будет удалена, хотя во входном файле EXPR_NO стоял на всех омонимах. 
				    if( !(H.m_iPoses & O.m_Poses) )
					    H.DeleteOborotMarks();
                }
			}
		}	
    }
};



int CSentence::FindAuxVerbByLemma(int MainWordNo, const string& Lemma) const
{
	for (size_t WordNo =0;  WordNo < m_Words.size(); WordNo++)
		for (size_t i =0;  i < m_Words[WordNo].m_MainVerbs.size(); i++)
		if (m_Words[WordNo].m_MainVerbs[i] == MainWordNo)
			if ( m_Words[WordNo].FindLemma(Lemma) )
				return WordNo;

	return -1;
};

int CSentence::FindFirstAuxVerb(int MainWordNo) const
{
	for (size_t WordNo =0;  WordNo < m_Words.size(); WordNo++)
		for (size_t i =0;  i < m_Words[WordNo].m_MainVerbs.size(); i++)
		if (m_Words[WordNo].m_MainVerbs[i] == MainWordNo)
				return WordNo;

	return -1;
};
size_t			CSentence::GetPrimitiveClausesCount() const
{
	return m_vectorPrClauseNo.size();
}

const CClause*	CSentence::GetPrimitiveClause(int ClauseNo) const
{
	return &GetClause(m_vectorPrClauseNo[ClauseNo]);
}



