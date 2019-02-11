/////////////////////////////////////////////////////////////////////////////
// Заполнение слота "CEnglishEquivMap& mapRNodeToENode"
// Поиск английских эквивалентов.
// Описание лежит в "l:\documents\Алгоритм перевода русского слова в английское на трансфере.lnk"

#include "StdAfx.h"
#include "../SemanLib/LexFuncts.h"
#include "../SemanLib/HierarchyHolder.h"
/////////////////////////////////////////////////////////////////////////////
// ищет  в значениях поля  RUS словаря  type русский вход RusEquivToEngArticleNo,// все словарные статьи, которые содержат такое значение, помещаются в engEquivs

bool CEngSemStructure::FindEnglishEquivHelper(vector<CEngInterp>& engEquivs,const CEngUnitNoToRusUnit RusEquivToEngArticleNo,DictTypeEnum type)
{
	bool bFlag = false;
	bool bFound = false;
	vector<CEngUnitNoToRusUnit>& equivs = m_pData->GetRusEquivIndexes(type);
	vector<CEngUnitNoToRusUnit>::const_iterator iter = lower_bound(equivs.begin() , equivs.end(), RusEquivToEngArticleNo );
	do
	{
		bFlag = (iter != equivs.end()) && (*iter == RusEquivToEngArticleNo);

		if( bFlag )
		{
			CEngInterp EngEquiv;
			EngEquiv.m_DictType = type;
			EngEquiv.m_UnitNo = (*iter).m_EngUnitNo;
			engEquivs.push_back( EngEquiv );
		}
		iter++;
	}
	while( bFlag );

	return(engEquivs.size() > 0);
}


/////////////////////////////////////////////////////////////////////////////

void CEngSemStructure::FindEnglishEquiv(vector<CEngInterp>& engEquivs, const string& rus_lemma, BYTE MeanNum /* = 10*/, DictTypeEnum   EngDictType /* = Aoss*/)
{
	CEngUnitNoToRusUnit RusEquivToEngArticleNo;
	RusEquivToEngArticleNo.m_RusUnitStr = rus_lemma;
	RusEquivToEngArticleNo.m_RusMeanNum = MeanNum;
	// сначала ищем с указанным номером значения
	if( !FindEnglishEquivHelper(engEquivs, RusEquivToEngArticleNo, EngDictType) )
	{
		// потом, если не  нашли, то - с любым номером значения.
		if( MeanNum < 10)
		{
			RusEquivToEngArticleNo.m_RusMeanNum = 10;
			FindEnglishEquivHelper(engEquivs, RusEquivToEngArticleNo, EngDictType);
		}
	}
}



/////////////////////////////////////////////////////////////////////////////
// основная функция  нахождения эквивалентов

void CEngSemStructure::FindEnglishEquivMain(CEnglishEquivMap& mapRNodeToENode)
{
	for( int NodeNo=0; NodeNo<RusStr.GetNodesSize(); NodeNo++ )
	{
		if( !RusStr.GetNode(NodeNo).IsWordContainer() )
			continue;

		const CSemNode& RusNode = RusStr.GetNode(NodeNo);

// если это Тезаурус
		if( IsThesRoss(RusNode.GetType()) )
		{
			long ThesId = GetThesIdByRossId(RusNode.GetType());
			string rus_num = GetRoss(RusNode.GetType())->GetEntryStr(RusNode.GetUnitNo());
			long TermId = RusNode.GetInterp()->m_TerminId;

			vector<int> CurrentEnglishTermins;
			helper.GetThes(ThesId)->QueryEnglishTranslations(TermId, CurrentEnglishTermins);
			long count = CurrentEnglishTermins.size();
			if( count>0 )
			{
				long num = CurrentEnglishTermins[0];
				const CInnerTermin& TermPtr = helper.GetThes(ThesId)->m_Termins[num];
				string eng_num = Format("%i", TermPtr.m_TerminId);

				CEngInterp interp;
				interp.m_DictType = RusNode.GetType();
				interp.m_UnitNo = GetRoss(RusNode.GetType())->LocateUnit(eng_num.c_str(),1);
				interp.m_TerminId = TermPtr.m_TerminId;
				interp.m_ThesaurusId = ThesId;
				

				if( interp.m_UnitNo != ErrUnitNo )
				{
					vector<CEngInterp> mapRes;
					mapRes.push_back(interp);
					mapRNodeToENode[NodeNo] = mapRes;
				}
				else
				{
					InterpretWithPlugArticles(NodeNo,mapRNodeToENode);
					//  if InterpretWithPlugArticles was successful
					if (mapRNodeToENode.find(NodeNo) != mapRNodeToENode.end())
					{
						vector<CEngInterp>& mapRes = mapRNodeToENode[NodeNo];
						for (long i =0; i < mapRes.size(); i++)
						{ 
							mapRes[i].m_TerminId = interp.m_TerminId;
							mapRes[i].m_ThesaurusId = interp.m_ThesaurusId;
						}
					};

				}
				continue;
			}
		}


// берем англ. эквиваленты из русской статьи		
		vector< SEngEquiv > vectorEngEquivs;
		GetEngEquivsFromRusArticle(vectorEngEquivs,RusNode.GetUnitNo(),RusNode.GetType(),NodeNo);

// устанавливаем соотвествие русских словарей английским
		vector<DictTypeEnum> EngDictTypes;
		if( RusNode.GetType() == OborRoss )	
		{
			EngDictTypes.push_back(Aoss);
			EngDictTypes.push_back(EngObor);
			EngDictTypes.push_back(EngCollocRoss);
		}
		else if( RusNode.GetType() == CollocRoss )
		{
			EngDictTypes.push_back(EngCollocRoss);
			for( int k=0; k<vectorEngEquivs.size(); k++ )
			{
				if( vectorEngEquivs[k].m_StrEngWord.find("_") == string::npos )
				{
					EngDictTypes.push_back(Aoss);
					break;
				}
			}
		}
		else
		{
			EngDictTypes.push_back(Aoss);
			for( int k=0; k<vectorEngEquivs.size(); k++ )
			{
				if( vectorEngEquivs[k].m_StrEngWord.find("_") != string::npos )
				{
					EngDictTypes.push_back(EngCollocRoss);
					EngDictTypes.push_back(EngObor);
					break;
				}
			}
		}

// ищем в английиских словарях те англ. эквиваленты, к-рые указаны в поле ENG
		GetEngEquivsFromVector(mapRNodeToENode,NodeNo,vectorEngEquivs,EngDictTypes);

		CEnglishEquivMap::iterator it = mapRNodeToENode.find(NodeNo);
// в англйиских словарях удалось найти словарные статьи, указанные в поле ENG
		if( it != mapRNodeToENode.end() )
			continue;

			
// в крайнем случае интерпретируем заглушкой
		InterpretWithPlugArticles(NodeNo,mapRNodeToENode);
	}

	FindEquivForLexFunct(mapRNodeToENode);
}

/////////////////////////////////////////////////////////////////////////////

void CEngSemStructure::FindEquivForLexFunct(CEnglishEquivMap& mapRNodeToENode)
{
	for( int i = 0; i<RusStr.GetLexFuncts().size(); i++ )
	{
		const CLexFunctRel& lexFun = RusStr.GetLexFuncts()[i];
		CEnglishEquivMap::iterator SitEquivs = mapRNodeToENode.find(lexFun.m_SourceNodeNo);
		if( SitEquivs == mapRNodeToENode.end() )
			continue;

		vector<CEngInterp> newSitEquivs;
		vector<CEngInterp> newParamEquivs;
		CLexicalFunctionField LexicalFunct("","");
		CRossInterp prep;
		CLexFunctRel LexFunctRel;
		for( int k=0; k<SitEquivs->second.size(); k++ )
		{
			if( HasThisLexFunc(lexFun.m_LexFunctName,SitEquivs->second[k],LexicalFunct) )
			{
				EngRusMakeLower(LexicalFunct.m_Lemma);
				long ParamUnitNo = GetRoss(Aoss)->LocateUnit(LexicalFunct.m_Lemma.c_str(),LexicalFunct.m_MeanNum);
				if(ParamUnitNo != ErrUnitNo)
				{
					newSitEquivs.push_back(SitEquivs->second[k]);
					CEngInterp newInterp;
					newInterp.m_UnitNo = ParamUnitNo;
					newInterp.m_DictType = Aoss;
					newParamEquivs.push_back(newInterp);
					prep = LexicalFunct.m_Prep;
					break;
				}
				else
				{
					if( IsParameterOfAdjOrAdv(lexFun.m_LexFunctName) )
					{
						newSitEquivs.push_back(SitEquivs->second[k]);
						LexFunctRel.m_LexFunParamLemma = LexicalFunct.m_Lemma;
					}
				}
			}
		}

		//assert(newValEquivs.size() == newSitEquivs.size() );
		if( newSitEquivs.size() )
		{
			LexFunctRel.m_SourceNodeNo= lexFun.m_SourceNodeNo;
			LexFunctRel.m_TargetNodeNo = lexFun.m_TargetNodeNo;
			LexFunctRel.m_LexFunctName = lexFun.m_LexFunctName;
			LexFunctRel.m_Prep.m_UnitNo = prep.m_UnitNo;
			LexFunctRel.m_Prep.m_DictType = prep.m_DictType;
			m_LexFuncts.push_back(LexFunctRel);

			mapRNodeToENode[lexFun.m_SourceNodeNo] = newSitEquivs;
			if( newParamEquivs.size() )
				mapRNodeToENode[lexFun.m_TargetNodeNo] = newParamEquivs;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

void CEngSemStructure::FindEngEquivForRusArticle(CEngInterp rusUnit, vector<CEngInterp>& vectorEngEquivsFromDict,DictTypeEnum EngDictType)
{
	vector<SEngEquiv> vectorEngEquivs;		
	GetEngEquivsFromRusArticle(vectorEngEquivs, rusUnit.m_UnitNo, rusUnit.m_DictType);
	vector<DictTypeEnum> dicts;
	dicts.push_back(EngDictType);
	FindEngWords(vectorEngEquivsFromDict, vectorEngEquivs, dicts);
	if( !vectorEngEquivsFromDict.size() )
	{
		string RusWord = GetRoss(rusUnit.m_DictType)->GetEntryStr(rusUnit.m_UnitNo);
		FindEnglishEquiv(vectorEngEquivsFromDict, RusWord , GetRoss(rusUnit.m_DictType)->GetUnitMeanNum(rusUnit.m_UnitNo), EngDictType);
		IntersectEngEquivs( vectorEngEquivs, vectorEngEquivsFromDict);
	}
}

/////////////////////////////////////////////////////////////////////////////

void CEngSemStructure::IntersectEngEquivs(vector<SEngEquiv >& vectorEngEquivsFromRusArticle,vector<CEngInterp>& vectorEngEquivsFromAoss )
{
	if( vectorEngEquivsFromRusArticle.size() == 0 )
		return;

	vector<long> delEquivsNum;
	for( int i = 0 ; i < vectorEngEquivsFromAoss.size() ; i++ )
	{
		CRossInterp& UnitInterp = vectorEngEquivsFromAoss[i];
		string strEngWord = GetRoss(UnitInterp.m_DictType)->GetEntryStr(UnitInterp.m_UnitNo);
		int meanNo = GetRoss(UnitInterp.m_DictType)->GetUnitMeanNum(UnitInterp.m_UnitNo);
		int j=0;
		for( ; j<vectorEngEquivsFromRusArticle.size(); j++ )
		{
			if( strEngWord == vectorEngEquivsFromRusArticle[j].m_StrEngWord )
			{
				if( vectorEngEquivsFromRusArticle[j].m_iMeanNum != 10 )
				{
					if( vectorEngEquivsFromRusArticle[j].m_iMeanNum == meanNo )
						break;
				}
				else
					break;
			}
		}
		if( j == vectorEngEquivsFromRusArticle.size() )
			delEquivsNum.push_back(i);
	}

	if( delEquivsNum.size() != vectorEngEquivsFromAoss.size() )
		for( int i=delEquivsNum.size()-1; i>=0; i-- )
			vectorEngEquivsFromAoss.erase(vectorEngEquivsFromAoss.begin() + delEquivsNum[i]);
}

/////////////////////////////////////////////////////////////////////////////

void CEngSemStructure::GetAFieldVector(string FieldStr, DictTypeEnum type, vector<TCortege>& vectorAgx, long UnitNo) const
{
	if( type == NoneRoss)
		return;
	if (UnitNo != ErrUnitNo)
		if (!GetRoss(type)->IsEmptyArticle(UnitNo))
			for( int i=GetRoss(type)->GetUnitStartPos(UnitNo); i<=GetRoss(type)->GetUnitEndPos(UnitNo); i++ )
			{
				long FieldNo = GetRoss(type)->GetCortegeFieldNo(i);
				if(	FieldStr == GetRoss(type)->Fields[FieldNo].FieldStr )
					vectorAgx.push_back(GetCortege(GetRoss(type),i));
			}
}


/////////////////////////////////////////////////////////////////////////////
// эта функция берет все эквиваленты из поля ENG 
//  если есть ENUMBER рабртает ТОЛЬКО по нему
//  пробует EDOMAIN (пока пустая, т.к. RusStr.GetClausePO() все равно не работает)
//  пробует ELEX
//  пробует EOPERATOR
//  пробует EPREP
//  пробует ELF
//  затем смотрит на EGF и ESF

void CEngSemStructure::GetEngEquivsFromRusArticle(vector< SEngEquiv >& vectorEngEquivs,
	int RusUnitNo,DictTypeEnum DictType /* = Ross*/,int iRusNode /* = -1*/) const
{
	if( RusUnitNo == ErrUnitNo )
		return;
	if( DictType == NoneRoss )
		return;
//
	vector<TCortege> corteges;
	GetRossHolder(DictType)->GetFieldValues(string("ENG"),RusUnitNo,corteges);

	for( int i=0; i<corteges.size(); i++ )
	{
		SEngEquiv engEquiv;		
		if( corteges[i].m_DomItemNos[0] == -1 )
			continue;
		string strEngWord = (const char*)GetRoss(DictType)->GetDomItemStr(corteges[i].m_DomItemNos[0]);
//
		int pos = strEngWord.find("#delete_numeral_prefix");
		if( pos!=string::npos )
		{
			engEquiv.m_StrNumeral = strEngWord.substr(pos);
			strEngWord.erase(pos);
		}
		pos = strEngWord.find("#ed");
		if( pos!=string::npos && pos>0 )
		{
			engEquiv.m_StrNumeral = strEngWord.substr(pos);
			strEngWord.erase(pos);
		}
//
		engEquiv.m_StrEngWord = strEngWord;

	
		if( corteges[i].m_DomItemNos[1] != -1 )
		{
			string meanNum = (const char*)GetRoss(DictType)->GetDomItemStr(corteges[i].m_DomItemNos[1]);
			engEquiv.m_iMeanNum = meanNum[0] - '0';
		}

		if( corteges[i].m_DomItemNos[2] != -1 )
			engEquiv.m_StrLexFunc = GetRoss(DictType)->GetDomItemStr(corteges[i].m_DomItemNos[2]);
		else
			engEquiv.m_StrLexFunc = "";

		if( DictType == Ross)
			engEquiv.m_RusPoses = RusStr.GetPosesFromRusArticle(*GetRossHolder(DictType),RusUnitNo);
		vectorEngEquivs.push_back(engEquiv); // здесь поля ENG
	}
//
	if( iRusNode == -1 )
		return;
	const CSemNode& rusNode = RusStr.GetNode(iRusNode);
	if( rusNode.m_MainWordNo == -1 )
		return;
	const CSemWord& rusWord = rusNode.GetWord(rusNode.m_MainWordNo);

	vector<SEngEquiv> tempEngEquivs;
	
// обработка ENUMBER
	vector<TCortege> vectorAnm;
	GetAFieldVector("ENUMBER", DictType,vectorAnm,RusUnitNo);

	vector<long> vecListByAnm;
	for( int i=0; i<vectorAnm.size(); i++ )
	{
		if( vectorAnm[i].m_DomItemNos[0] == -1 )
			continue;
		long theId = vectorAnm[i].m_BracketLeafId - 1;
		vecListByAnm.push_back(theId);

		int i1 = atoi(rusWord.m_NumeralPrefix.c_str());
		string strANM = GetCortegeStr(rusNode.GetType(),vectorAnm[i]);
		int i2 = atoi(strANM.c_str());
		if( i1!=i2 )
			continue;

		if( theId < vectorEngEquivs.size() )
		{
			tempEngEquivs.push_back(vectorEngEquivs[theId]);
			vectorEngEquivs = tempEngEquivs;
			return;
		}
	} // vectorAnm.size()

// берем те эквиваленты, на которые не ссылается никакое ENUMBER
	if( vecListByAnm.size()>0 )
	{
		for( int i=0; i<vectorEngEquivs.size(); i++ )
		{
			if( find(vecListByAnm.begin(),vecListByAnm.end(),i) != vecListByAnm.end() )
				continue;
			tempEngEquivs.push_back(vectorEngEquivs[i]);
		}
	}
	if( tempEngEquivs.size()>0 )
	{
		vectorEngEquivs = tempEngEquivs;
		return;
	}

// обработка EDOMAIN
	vector<TCortege> vectorApo;
	GetAFieldVector("EDOMAIN", DictType,vectorApo,RusUnitNo);
	long ClauseNo = RusStr.GetNode(iRusNode).m_ClauseNo;
	string tema = RusStr.GetClausePO(ClauseNo);

// возьмем все связи (но дополнительные отношения брать не будем!)
	vector<long> inRelsRus;
	RusStr.GetIncomingRelations(iRusNode,inRelsRus,false);
	vector<long> outRelsRus;
	RusStr.GetOutcomingRelations(iRusNode,outRelsRus);
	vector<long> usingRels;
	for( int i=0; i<inRelsRus.size(); i++ )
	{
		const CSemRelation* R = RusStr.GetRelation(inRelsRus[i]);
		if( R->m_bReverseRel  && !R->m_bDopRelation)
			usingRels.push_back(inRelsRus[i]);
	}
	for( int i=0; i<outRelsRus.size(); i++ )
	{
		const CSemRelation* R = RusStr.GetRelation(outRelsRus[i]);
		if( !R->m_bReverseRel && !R->m_bDopRelation)
			usingRels.push_back(outRelsRus[i]);
	}

// !!! если это место раскомментировать, "она" перейдет в "she", а не в "it"
//	if( usingRels.size()==0 )
//		return;
//



// обработка ELEX
	vector<TCortege> vectorAlx;
	GetAFieldVector("ELEX", DictType,vectorAlx,RusUnitNo);

	vector<long> vecGoodByAlx;
	for( int i=0; i<vectorAlx.size(); i++ )
	{
		if( vectorAlx[i].m_DomItemNos[0] == -1 )
			continue;
		long theId = vectorAlx[i].m_BracketLeafId - 1;

		if( vectorAlx[i].m_LeafId == 0 ) //если это ELEX без индекса
		{
			// не может быть ELEX без индекса
			assert( false );
		}

		string strALX = GetCortegeStr(rusNode.GetType(),vectorAlx[i]);
		EngRusMakeUpper(strALX);

		for( int k=0; k<usingRels.size(); k++ )
		{
			if( RusStr.GetRelation(usingRels[k])->m_Valency.m_LeafId != vectorAlx[i].m_LeafId )
				continue;

			int iRusActant;
			if( RusStr.GetRelation(usingRels[k])->m_bReverseRel )
				iRusActant = RusStr.GetRelation(usingRels[k])->m_SourceNodeNo;
			else
				iRusActant = RusStr.GetRelation(usingRels[k])->m_TargetNodeNo;

			const CSemNode& rusNode = RusStr.GetNode(iRusActant);
			int iSemMainWord = rusNode.m_MainWordNo;
			if( iSemMainWord == -1 )
				continue;
			string strVAL = rusNode.GetWord(iSemMainWord).m_Lemma;
			
			// если это указание вышестоящего концепта
			if (strALX[0] == '#')
			{
				if (!_find(rusNode.m_HigherConcepts, strALX)) continue;
			}
			else
			if(	strVAL != strALX )
			{
			  // проверяем еще эквиваленты слова
			  long j=0;
			  for (; j < rusNode.GetWord(iSemMainWord).m_WordEquals.size(); j++)
			  {
			    string strAspVerb = rusNode.GetWord(iSemMainWord).m_WordEquals[j];
				if (strAspVerb == strALX) break;
			  }
			  /*
			  если и в аспектных парах нет, тогда попробуем найти
			  с помощью анафорических связей
			  */
  			  if (j == rusNode.GetWord(iSemMainWord).m_WordEquals.size())
			  {
				long nd = RusStr.GetAnaphoraTarget(iRusActant);
				if (     (nd == -1)
					||  !RusStr.GetNode(nd).IsLemma(strALX)
				   )
				continue;
			  };
			};
			

			if( theId < vectorEngEquivs.size() )
				vecGoodByAlx.push_back(theId);
		}
	} // vectorAlx.size()

	if( vecGoodByAlx.size()>0 )
	{
		for( int i=0; i<vecGoodByAlx.size(); i++ )				
			tempEngEquivs.push_back(vectorEngEquivs[vecGoodByAlx[i]]);			
		vectorEngEquivs = tempEngEquivs;
		return;
	}

	
// обработка EOPERATOR
	vector<TCortege> vectorAop;
	GetAFieldVector("EOPERATOR", DictType,vectorAop,RusUnitNo);

	vector<long> vecGoodByAop;
	for( int i=0; i<vectorAop.size(); i++ )
	{
		if( vectorAop[i].m_DomItemNos[0] == -1 )
			continue;
		long theId = vectorAop[i].m_BracketLeafId - 1;

		string strAOP = GetCortegeStr(rusNode.GetType(),vectorAop[i]);
		if( vectorAop[i].m_LeafId == 0 ) //если это EOPERATOR без индекса
		{
			if (strAOP == "БЫ")
			{
			  if ( !RusStr.GetSemClause(rusNode.m_ClauseNo).m_HasParticleBY )
				  continue;
			}
			else
			{
				if( !rusNode.HasRelOperator(strAOP) )
					continue;
			}
			if( theId < vectorEngEquivs.size() )
				vecGoodByAop.push_back(theId);
		}

		for( int k=0; k<usingRels.size(); k++ )
		{
			if( RusStr.GetRelation(usingRels[k])->m_Valency.m_LeafId != vectorAop[i].m_LeafId )
				continue;

			int iRusActant;
			if( RusStr.GetRelation(usingRels[k])->m_bReverseRel )
				iRusActant = RusStr.GetRelation(usingRels[k])->m_SourceNodeNo;
			else
				iRusActant = RusStr.GetRelation(usingRels[k])->m_TargetNodeNo;

			const CSemNode& rusNode = RusStr.GetNode(iRusActant);
			if( !RusStr.GetNode(iRusActant).HasRelOperator(strAOP) )
				continue;
			if( theId < vectorEngEquivs.size() )
				vecGoodByAop.push_back(theId);
		}
	}

	if( !vecGoodByAop.empty() )
	{
		for( int i=0; i<vecGoodByAop.size(); i++ )				
			tempEngEquivs.push_back(vectorEngEquivs[vecGoodByAop[i]]);
		vectorEngEquivs = tempEngEquivs;
		return;
	}



	// обработка EPREP
	vector<TCortege> vectorAPredlog;
	GetAFieldVector("EPREP",DictType,vectorAPredlog,RusUnitNo);

	vector<long> vecGoodByAPredlog;
	for( int i=0; i<vectorAPredlog.size(); i++ )
	{
		if( vectorAPredlog[i].m_DomItemNos[0] == -1 )
			continue;
		if( vectorAPredlog[i].m_LeafId != 0 ) continue;//EPREP должен быть без индекса
		long theId = vectorAPredlog[i].m_BracketLeafId - 1;
		if( theId >= vectorEngEquivs.size() ) continue;
		string strAOP = GetCortegeStr(rusNode.GetType(),vectorAPredlog[i]);
		WORD UnitNo = GetRoss(OborRoss)->LocateUnit(strAOP.c_str(), 1);
		if ( !rusNode.HasThePrep(UnitNo)) continue;
		vecGoodByAPredlog.push_back(theId);
	}

	if( vecGoodByAPredlog.size()>0 )
	{
		for( int i=0; i<vecGoodByAPredlog.size(); i++ )				
			tempEngEquivs.push_back(vectorEngEquivs[vecGoodByAPredlog[i]]);
		vectorEngEquivs = tempEngEquivs;
		return;
	}

	// обработка ELF
	vector<TCortege> vectorAlf;
	GetAFieldVector("ELF", DictType,vectorAlf,RusUnitNo);

	vector<long> vecGoodByALF;
	for( int i=0; i<vectorAlf.size(); i++ )
	{
		if( vectorAlf[i].m_DomItemNos[0] == -1 )
			continue;
		if( vectorAlf[i].m_LeafId != 0 ) continue;//ELF должен быть без индекса
		long theId = vectorAlf[i].m_BracketLeafId - 1;
		if( theId >= vectorEngEquivs.size() ) continue;
		string strAOP = GetCortegeStr(rusNode.GetType(),vectorAlf[i]);
		string LF;
		long RelNo = -1;
		if (strAOP.substr(0,3) == "out")
		{
			LF = strAOP.substr(3);
			Trim(LF);
			RelNo = RusStr.FindLexFunctBySituationNodeNo(iRusNode);
		}
		else
		  if (strAOP.substr(0,2) == "in")
			{
				string op = strAOP.substr(2);
				LF = Trim(op);
				RelNo = RusStr.FindLexFunctByParameterNodeNo(iRusNode);
		  }

		if (RelNo == -1) continue;
		if (RusStr.GetLexFuncts()[RelNo].m_LexFunctName != LF) continue;
		vecGoodByALF.push_back(theId);
	}

	if( vecGoodByALF.size()>0 )
	{
		for( int i=0; i<vecGoodByALF.size(); i++ )				
			tempEngEquivs.push_back(vectorEngEquivs[vecGoodByALF[i]]);
		vectorEngEquivs = tempEngEquivs;
		return;
	}




// обработка EGF
	StringVector domens;
	domens.push_back("D_GRAMMEMS");

	vector<TCortege> vectorAgx;
	GetAFieldVector("EGF", DictType,vectorAgx,RusUnitNo);

	vector<long> vecListByAgx;
	vector<long> vecGoodByAgx;
	for( int i=0; i<vectorAgx.size(); i++ )
	{
		if( vectorAgx[i].m_DomItemNos[0] == -1 )
			continue;
		long theId = vectorAgx[i].m_BracketLeafId - 1;
		vecListByAgx.push_back(theId);

		if( vectorAgx[i].m_LeafId == 0 ) //если это EGF без индекса
		{
			if( !CheckDomensForCortege(domens,vectorAgx[i],DictType) )
				continue;

			string strGrammems = GetItemStr(vectorAgx[i].m_DomItemNos[0],DictType);
			QWORD Grammems;
			DWORD Pose;

			m_pData->GetCustomGrammems(strGrammems,Grammems,Pose);

			if( Pose==0 && Grammems==-1 )
				continue;
			
			if( Pose!=0 && !rusWord.HasPOS(GetPosFromPosBit(Pose)) )
				continue;

			if( !(rusWord.GetAllGrammems() & Grammems ) )
				continue;

			if( theId < vectorEngEquivs.size() )
			{
				vecGoodByAgx.push_back(theId);
				vecListByAgx.pop_back();
			}
		}
		
		else //если это EGFi
		{
			bool bFoundAGX = false;
			string strAGX = GetCortegeStr(rusNode.GetType(),vectorAgx[i]);

			for( int k=0; k<usingRels.size(); k++ )
			{
				if( RusStr.GetRelation(usingRels[k])->m_Valency.m_IsAddition )
					continue;
				if( RusStr.GetRelation(usingRels[k])->m_Valency.m_LeafId != vectorAgx[i].m_LeafId )
					continue;

				bFoundAGX = true;

				int iRusActant;
				if( RusStr.GetRelation(usingRels[k])->m_bReverseRel )
					iRusActant = RusStr.GetRelation(usingRels[k])->m_SourceNodeNo;
				else
					iRusActant = RusStr.GetRelation(usingRels[k])->m_TargetNodeNo;

				if(    CheckDomensForCortege(domens,vectorAgx[i],DictType) 
				  )
				{
					//если это не формат, взятый из GF а просто часть речи и граммемы
					int semMainWord = RusStr.GetNode(iRusActant).m_MainWordNo;
					if( semMainWord == -1 )
						continue;

					string strGrammems = GetItemStr(vectorAgx[i].m_DomItemNos[0], DictType);
					DWORD Pose;
					QWORD Grammems;
					m_pData->GetCustomGrammems(strGrammems,Grammems,Pose);
					if( Pose==0 && Grammems==-1 )
						continue;
					if( Pose!=0 && !RusStr.GetNode(iRusActant).GetWord(semMainWord).HasPOS(GetPosFromPosBit(Pose)) )
						continue;
					if(	!(RusStr.GetNode(iRusActant).GetWord(semMainWord).GetAllGrammems() & Grammems) )
						continue;

					if( theId < vectorEngEquivs.size() )
					{
						vecGoodByAgx.push_back(theId);
						vecListByAgx.pop_back();
					}
				}
				else
				{
					string dbg_str = GetItemStr(vectorAgx[i].m_DomItemNos[0],DictType);

					//проверяем на эквивалентность с тем кортежем, по которому Леша собрал

					const CSemRelation& rusRel = *RusStr.GetRelation(usingRels[k]);
					if(    !CompareCortegeItems(GetRossHolder(DictType),vectorAgx[i],rusRel.m_SynReal.m_Cortege) 
						&& (strAGX != "+") 
					  )
						continue;
					if( theId < vectorEngEquivs.size() )
					{
						vecGoodByAgx.push_back(theId);
						vecListByAgx.pop_back();
					}
				}
			}

			if( !bFoundAGX && strAGX == "-" )
			{
				if( theId < vectorEngEquivs.size() )
				{
					vecGoodByAgx.push_back(theId);
					vecListByAgx.pop_back();
				}
			}
		}
	} // vectorAgx.size()

// обработка ESF
	vector<TCortege> vectorAcx;
	GetAFieldVector("ESF", DictType,vectorAcx,RusUnitNo);

	vector<long> vecListByAcx;
	vector<long> vecGoodByAcx;
	for( int i=0; i<vectorAcx.size(); i++ )
	{
		if( vectorAcx[i].m_DomItemNos[0] == -1 )
			continue;
		long theId = vectorAcx[i].m_BracketLeafId - 1;
		vecListByAcx.push_back(theId);

		if( vectorAcx[i].m_LeafId == 0 ) // если это АCХ без индекса
		{
			string strACX = GetCortegeStr(rusNode.GetType(),vectorAcx[i]);
			

			vector<string> SemFets;
			SemFets.push_back(strACX.c_str());
			IncludeLowerInHierarchy(&m_pData->m_HierarchySemFetDoc,SemFets);
			int k=0;
			for( ; k<SemFets.size(); k++ )
			{
				if( HasSemFetPro(RusStr.GetNode(iRusNode),SemFets[k]) )
					break;
			}
			if( k>=SemFets.size() )
				continue;

			if( theId < vectorEngEquivs.size() )
			{
				vecGoodByAcx.push_back(theId);
				vecListByAcx.pop_back();
			}
		}

		else //если это АCХi
		{
			bool bFoundACX = false;
			string strACX = GetCortegeStr(rusNode.GetType(),vectorAcx[i]);

			for( int k=0; k<usingRels.size(); k++ )
			{
				if( RusStr.GetRelation(usingRels[k])->m_Valency.m_LeafId != vectorAcx[i].m_LeafId )
					continue;

				bFoundACX = true;

				int iRusActant;
				if( RusStr.GetRelation(usingRels[k])->m_bReverseRel )
					iRusActant = RusStr.GetRelation(usingRels[k])->m_SourceNodeNo;
				else
					iRusActant = RusStr.GetRelation(usingRels[k])->m_TargetNodeNo;

				vector<string> SemFets;
				SemFets.push_back(strACX.c_str());
				IncludeLowerInHierarchy(&m_pData->m_HierarchySemFetDoc,SemFets);
				int kk=0;
				for( ; kk<SemFets.size(); kk++ )
				{
					if( HasSemFetPro(RusStr.GetNode(iRusActant),SemFets[kk]) )
						break;
				}
				if( kk>=SemFets.size() )
					continue;

				if( theId < vectorEngEquivs.size() )
				{
					vecGoodByAcx.push_back(theId);
					vecListByAcx.pop_back();
				}
			}

		}
	} // vectorAcx.size()

// формируем результат
	if( vecGoodByAgx.size()>0 && vecGoodByAcx.size()>0 )
	{
		for( int i=0; i<vecGoodByAgx.size(); i++ )
		{
			if( find(vecGoodByAcx.begin(),vecGoodByAcx.end(),vecGoodByAgx[i]) != vecGoodByAcx.end() )
				tempEngEquivs.push_back(vectorEngEquivs[vecGoodByAgx[i]]);
		}

		if( tempEngEquivs.size()>0 )
		{
			vectorEngEquivs = tempEngEquivs;
			return;
		}
	}
		
	if( vecGoodByAgx.size()>0 )
	{
		for( int i=0; i<vecGoodByAgx.size(); i++ )				
			tempEngEquivs.push_back(vectorEngEquivs[vecGoodByAgx[i]]);			
		vectorEngEquivs = tempEngEquivs;
		return;
	}

	if( vecGoodByAcx.size()>0 )
	{
		for( int i=0; i<vecGoodByAcx.size(); i++ )				
			tempEngEquivs.push_back(vectorEngEquivs[vecGoodByAcx[i]]);			
		vectorEngEquivs = tempEngEquivs;
		return;
	}

// раз мы здесь, то либо нет ни EGF ни ESF, либо они противоречивы

// берем те эквиваленты, на которые не ссылается никакая EGF
	if( vecListByAgx.size()>0 )
	{
		for( int i=0; i<vectorEngEquivs.size(); i++ )
		{
			if( find(vecListByAgx.begin(),vecListByAgx.end(),i) != vecListByAgx.end() )
				continue;
			tempEngEquivs.push_back(vectorEngEquivs[i]);
		}
	}
	if( tempEngEquivs.size()>0 )
	{
		vectorEngEquivs = tempEngEquivs;
		return;
	}

// берем те эквиваленты, на которые не ссылается никакая ESF
	if( vecListByAcx.size()>0 )
	{
		for( int i=0; i<vectorEngEquivs.size(); i++ )
		{
			if( find(vecListByAcx.begin(),vecListByAcx.end(),i) != vecListByAcx.end() )
				continue;
			tempEngEquivs.push_back(vectorEngEquivs[i]);
		}
	}
	if( tempEngEquivs.size() )
	{
		vectorEngEquivs = tempEngEquivs;
		return;
	}
}

/////////////////////////////////////////////////////////////////////////////

void CEngSemStructure::FindEngWords(vector<CEngInterp>& resEngUnits, vector< SEngEquiv >& vectorEngEquivs, vector<DictTypeEnum> EngDictTypes)
{
	for( int j=0; j<EngDictTypes.size(); j++ )
	{
		for( int i=0; i<vectorEngEquivs.size(); i++ )
		{
			long UnitNo;
			if( vectorEngEquivs[i].m_iMeanNum == 10 )
				UnitNo = GetRoss(EngDictTypes[j])->LocateUnit(vectorEngEquivs[i].m_StrEngWord.c_str(), 1);
			else
				UnitNo = GetRoss(EngDictTypes[j])->LocateUnit(vectorEngEquivs[i].m_StrEngWord.c_str(), vectorEngEquivs[i].m_iMeanNum);
			if( UnitNo != ErrUnitNo )
			{
				if( (vectorEngEquivs[i].m_RusPoses != -1) && (EngDictTypes[j] == Aoss ) && (vectorEngEquivs[i].m_iMeanNum == 10 ) )
				{
					CEngSemWord  Dummy;
					ReadMorphFromMainGF(UnitNo, EngDictTypes[j], Dummy);
					BYTE engPos = GetOnePOS(Dummy.m_Poses);
//Gri !!!!!
// это место где need переходит в have
					if( engPos == UnknownPOS )
						continue;
					if( !( (1 << engPos) & (EngPOSByRusPOS(GetOnePOS(vectorEngEquivs[i].m_RusPoses),"") )) )
						continue;
//gri
				}
				CEngInterp interp;
				interp.m_UnitNo = UnitNo;
				interp.m_DictType = EngDictTypes[j];
				resEngUnits.push_back(interp);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

void CEngSemStructure::GetEngEquivsFromVector(CEnglishEquivMap& mapRNodeToENode, int iRusNode, vector< SEngEquiv >& vectorEngEquivs, vector<DictTypeEnum> EngDictTypes)
{
	CEnglishEquivMap::iterator it;
	it = mapRNodeToENode.find(iRusNode);
	if( it !=  mapRNodeToENode.end() )
		FindEngWords(mapRNodeToENode[iRusNode], vectorEngEquivs, EngDictTypes);		
	else
	{
		vector<CEngInterp> vector_intreps;
		FindEngWords(vector_intreps,vectorEngEquivs,EngDictTypes);
		if( vector_intreps.size() ) 
			mapRNodeToENode[iRusNode] = vector_intreps;
	}
}

/////////////////////////////////////////////////////////////////////////////

bool CEngSemStructure::InterpretWithPlugArticles(long RusNodeNo,CEnglishEquivMap& mapRNodeToENode)
{
	CEngInterp EngEquiv;
	EngEquiv.m_DictType = Aoss;
	EngEquiv.m_UnitNo  = ErrUnitNo;
	if (RusStr.GetNode(RusNodeNo).m_SynGroupTypeStr == NAMES) return false;
	if (RusStr.GetNode(RusNodeNo).m_SynGroupTypeStr == KEYB) return false;

	// числительные
	if (		RusStr.GetNode(RusNodeNo).IsPrimitive() 
			&&	isdigit((unsigned char)RusStr.GetNode(RusNodeNo).GetWord(0).m_Word[0])
			&&	!RusStr.HasPOS (RusNodeNo,ADV)  // "вдвоем"
	   ) return false;

	if( RusStr.IsVerbForm(RusStr.GetNode(RusNodeNo)) )
		EngEquiv.m_UnitNo = GetRoss(Aoss)->LocateUnit ("+VERB",1);
	else if( RusStr.HasPOS(RusNodeNo,NOUN) )
		EngEquiv.m_UnitNo = GetRoss(Aoss)->LocateUnit("+NOUN",1);
	else if( RusStr.HasPOS(RusNodeNo,ADJ_FULL) || RusStr.HasPOS(RusNodeNo,ADJ_SHORT) )
		EngEquiv.m_UnitNo = GetRoss(Aoss)->LocateUnit("+ADJECTIVE",1);
	else if( RusStr.HasPOS (RusNodeNo,ADV) )
		EngEquiv.m_UnitNo = GetRoss(Aoss)->LocateUnit("+ADVERB",1);
	else if( RusStr.HasPOS(RusNodeNo,PRONOUN) )
		EngEquiv.m_UnitNo = GetRoss(Aoss)->LocateUnit("+PRONOUN",1);
	else if( RusStr.HasPOS(RusNodeNo,PRONOUN_P) )
		EngEquiv.m_UnitNo = GetRoss(Aoss)->LocateUnit("+PRON-ADJ",1);

	if( EngEquiv.m_UnitNo == ErrUnitNo )
		return false;

	vector<CEngInterp> vectorEquivs;
	vectorEquivs.push_back(EngEquiv);
	mapRNodeToENode[RusNodeNo] = vectorEquivs;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
