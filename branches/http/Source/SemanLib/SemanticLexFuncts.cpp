#include "stdafx.h"
#include "SemanticRusStructure.h"
#include "LexFuncts.h"


bool			IsOper1 (const string& LexFunct) 
{
		return		 (LexFunct == "Oper1")
			      || (LexFunct == "Real1")
				  || (LexFunct == "AntiReal1")
				  || (LexFunct == "IncepOper1")
				  || (LexFunct == "FinOper1")
				  || (LexFunct == "Caus1Func0")
				  || (LexFunct == "Liqu1Func0")
				  ;
};
bool			IsOper2 (const string& LexFunct) 
{
		return			(LexFunct == "Oper2") 
					||	(LexFunct == "Real2")
					|| (LexFunct == "AntiReal2")
					||  (LexFunct == "IncepOper2")
					||  (LexFunct == "Caus2Func0")
					||  (LexFunct == "Liqu2Func0")
					;
};
bool			IsOper3 (const string& LexFunct) 
{
		return			(LexFunct == "Oper3") 
					||	(LexFunct == "Real3")
					||  (LexFunct == "AntiReal3")
					||  (LexFunct == "IncepOper3")
					||  (LexFunct == "Caus3Func0")
					||  (LexFunct == "Liqu3Func0")
					;

};

bool			IsOper (const string& LexFunct) 
{
      return		  IsOper1(LexFunct)
		    	  ||  IsOper2(LexFunct)
				  ||  IsOper3(LexFunct)
  				  /*
				    CausFunc0 не совсем  правильно сюда включать, поскольку
					подлежащим для этой функции будет  слово, которое не является 
					участником  ситуации (каузатор). Но эта функция похожа 
					на Operi тем, что ситуация является прямым дополнением.
				  */
				  || (LexFunct == "CausFunc0")
				  ;
};

bool			IsFunc0 (const string& LexFunct)
{
	return	      (LexFunct == "Func0") 
			   || (LexFunct == "PermFunc0")
			   || (LexFunct == "IncepFunc0")
			   || (LexFunct == "FinFunc0");
};

bool			IsFunc1 (const string& LexFunct)
{
     return	       (LexFunct == "Func1")
				|| (LexFunct == "PermFunc1")
				|| (LexFunct == "IncepFunc1")
				|| (LexFunct == "FinFunc1");
;
};

bool			IsFunc2 (const string& LexFunct)
{
     return	    (LexFunct == "Func2");
};

bool			IsFunc3 (const string& LexFunct)
{
     return	    (LexFunct == "Func3");
};

	
bool			IsFunc (const string& LexFunct) 
{
     return	    IsFunc3(LexFunct) ||
				IsFunc2(LexFunct) ||
				IsFunc1(LexFunct) ||
				IsFunc0(LexFunct) 
				;
};

bool IsOper(const string& lexFun, int& OperNum) 
{
	if( IsOper1(lexFun) )
	{
		OperNum = 1;
		return true;
	}

	if( IsOper2(lexFun) )
	{
		OperNum = 2;
		return true;
	}
	if( IsOper3(lexFun) )
	{
		OperNum = 2;
		return true;
	}
	return false;
}

bool IsFunc(const string& lexFun, int& FuncNum) 
{

	bool bRet = false; 
	if( IsFunc0(lexFun) )
		FuncNum = 0;
	else	if( IsFunc1(lexFun) )
			FuncNum = 1;
	else	if( IsFunc2(lexFun) )
			FuncNum = 2;
	else	if( IsFunc3(lexFun) )
			FuncNum = 3;

	return true;
}


bool			IsParameterOfVerb (const string& LexFunct) 
{
	  return	    IsOper(LexFunct) 
				|| IsFunc(LexFunct);
};

bool			IsParameterOfAdjOrAdv (const string& LexFunct) 
{
      return	    (LexFunct == "Magn")
			      ||(LexFunct == "AntiMagn")
				  ||(LexFunct == "Ver")
				  ||(LexFunct == "AntiVer")
		       	  ||(LexFunct == "Bon")
			      ||(LexFunct == "AntiBon");
};


size_t CRusSemStructure::CountLexFunctsInOneClause(long ClauseNo) 
{
	size_t Result = 0;


    for (long i =0; i < m_LexFuncts.size(); i++)
     if (   IsInClause(m_LexFuncts[i].m_SourceNodeNo, ClauseNo)  )
		  Result ++;

   return Result;
};



void AddByLemma(const vector<SLexFunIndexes>& base, string Lemma, vector<CDictReference>& result) 
{
	SLexFunIndexes LexFunIndexes;
	LexFunIndexes.m_LexFunValue.m_UnitStr = Lemma;
	EngRusMakeLower(LexFunIndexes.m_LexFunValue.m_UnitStr);
	for (
		vector<SLexFunIndexes>::const_iterator  it = lower_bound(base.begin(), base.end(), LexFunIndexes, LexFunValueComp);
		 (		(it != base.end() )
			&&	( (*it).EqByLexFunValue(LexFunIndexes.m_LexFunValue)  )
		 );
		 it++)
		result.push_back(it->m_LexFunArg);
};

/*
 эти функции были написаны, чтобы просчитать 
 семантическое сходство первой и последней клауз в следующем предложении
 "Его ударил тот мужчина, который был в красной куртке, а полицейский их разнял"
*/
void CRusSemStructure::FindConnectedSituations(long NodeNo) 
{
	m_Nodes[NodeNo].m_ConnectedSits.clear();
	if ( m_Nodes[NodeNo].m_MainWordNo == -1) return;
	const CRusSemWord& W =   m_Nodes[NodeNo].m_Words[m_Nodes[NodeNo].m_MainWordNo];

	AddByLemma(m_pData->m_LexFunIndexesRusByValue, W.m_Lemma, m_Nodes[NodeNo].m_ConnectedSits);

	for (long i=0; i < W.m_WordEquals.size(); i++)
	  AddByLemma(m_pData->m_LexFunIndexesRusByValue, W.m_WordEquals[i], m_Nodes[NodeNo].m_ConnectedSits);
}

/*
 для всех существительных находит глаголы, для которых эти существительные являются S0
 например, у "постановки" должна быть ссылка на глагол "ставить"
 поскольку S0(ставить) = "постановка"
 Это нужно, чтобы провести LF S0.Oper от слова "вопрос" к слову  "постановка" во 
 фразе  "постановка вопроса"
*/

void CRusSemStructure::FindVerbsOfS0(long NodeNo) 
{
	m_Nodes[NodeNo].m_VerbsOfS0.clear();
	if (!m_Nodes[NodeNo].IsPrimitive()) return;
	SLexFunIndexes LexFunIndexes;
	LexFunIndexes.m_LexFunValue.m_UnitStr = m_Nodes[NodeNo].m_Words[0].m_Lemma;
	EngRusMakeLower(LexFunIndexes.m_LexFunValue.m_UnitStr);
	const vector<SLexFunIndexes>& base = m_pData->m_LexFunIndexesRusByValue;
	for (
		vector<SLexFunIndexes>::const_iterator  it = lower_bound(base.begin(), base.end(), LexFunIndexes, LexFunValueComp);
		 (		(it != base.end() )
			&&	( (*it).EqByLexFunValue(LexFunIndexes.m_LexFunValue)  )
		 );
		 it++)
		 if (it->m_LexFun == "S0")
		 {
		    m_Nodes[NodeNo].m_VerbsOfS0.push_back(it->m_LexFunArg.m_UnitStr);
			EngRusMakeUpper (m_Nodes[NodeNo].m_VerbsOfS0[m_Nodes[NodeNo].m_VerbsOfS0.size() - 1]);
		 };

};


void  CRusSemStructure::BuildVerbLexFunctParameterForTheNodes (long  SitNodeNo, long ParamNodeNo)
{

	const CRusSemNode& SitNode = m_Nodes[SitNodeNo];
	const CRusSemNode& ParamNode = m_Nodes[ParamNodeNo];
	vector<CLexicalFunctionField>::const_iterator It = find(SitNode.m_LexFunctFields.begin(), SitNode.m_LexFunctFields.end(), ParamNode.m_Words[ParamNode.m_MainWordNo].m_Lemma);
	if (It == SitNode.m_LexFunctFields.end())
	{
		// поищем среди эквивалентов
		const CRusSemWord& W = 	ParamNode.m_Words[ParamNode.m_MainWordNo];
		for (long j=0; j < W.m_WordEquals.size(); j++)
		{
			It = find(SitNode.m_LexFunctFields.begin(), SitNode.m_LexFunctFields.end(), W.m_WordEquals[j]);
			if (It != SitNode.m_LexFunctFields.end()) break;
		}
	};
	/*
	параметр может  быть словосочетанием 
	Например: IncepOper1 (ответственность) = взять_на_себя
	*/
	if (It == SitNode.m_LexFunctFields.end())
		if (ParamNode.GetType() == CollocRoss)
		{
			// берем TITLЕ	  
			string UnitStr = GetRoss(CollocRoss)->GetEntryStr(ParamNode.GetInterp()->m_UnitNo);
			EngRusMakeUpper(UnitStr);
			It = find(SitNode.m_LexFunctFields.begin(), SitNode.m_LexFunctFields.end(), UnitStr);
		};


	/*
	строим LF SO.Oper типа "постановка вопроса"
	*/
	bool bS0 = false;
	if (It == SitNode.m_LexFunctFields.end())
	{
		// поищем среди V0
		for (long j=0; j < ParamNode.m_VerbsOfS0.size(); j++)
		{
			It = find(SitNode.m_LexFunctFields.begin(), SitNode.m_LexFunctFields.end(), ParamNode.m_VerbsOfS0[j]);
			if (It != SitNode.m_LexFunctFields.end()) 
			{
				bS0 = true;
				break;
			}
		}
	};



	if (It == SitNode.m_LexFunctFields.end()) return;
	if (   !IsParameterOfVerb(It->m_LexFunct) 
		&& (It->m_LexFunct != "S0.Oper1")
		)
		return;

	// если не совпало по номеру значения
	if (It->m_MeanNum != -1)
		if (ParamNode.GetInterp() != 0)
			if ( GetRoss(ParamNode.GetInterp()->m_DictType)->GetUnitMeanNum(ParamNode.GetInterp()->m_UnitNo) != It->m_MeanNum)
				return;  

	/*
	Для Func надо проверить, что слово-ситуация имеет номинатив
	*/
	if (!bS0)
		if (IsFunc(It->m_LexFunct))
			if ( !SitNode.HasOneGrammem (rNominativ) )
				return;

	/*
	для  Оперов загружаем вторую валентность, и проверям, что
	слово-ситуация подпадает под этот шаблон.
	Проверку не нужно делать, если мы имеем дело с S0.Oper
	Проверку не нужно делать, если мы строим межклаузную LF
	например: "реформы, проведенные в прошлом году"
	*/
	if (!bS0)
		if (ParamNode.m_ClauseNo == SitNode.m_ClauseNo)
			if (IsOper(It->m_LexFunct))
			{
				long j=0;
				for (;j < ParamNode.m_NotProhibitedValSets.size(); j++)
				{
					CSemPattern P;
					if (ParamNode.m_NotProhibitedValSets[j].size() < 2) continue;
					GetActantPattern (ParamNodeNo, ParamNode.m_NotProhibitedValSets[j][1],  P);
					string  SyntacticRelation;
					CSynRealization SyntacticRealization;
					if (IsPattern (P, SitNodeNo, SyntacticRelation, SyntacticRealization) )
						break;
				};
				if (j == ParamNode.m_NotProhibitedValSets.size()) 
				{
					if (    (It->m_Prep.m_UnitNo == ErrUnitNo)
						|| !SitNode.HasThePrep(It->m_Prep.m_UnitNo)
						)
						return;
				};
			};


	//ведем стрелку от слова ситуации в присвязочный глагол
	string LexFunctName = It->m_LexFunct;
	if (bS0) LexFunctName = "S0."+LexFunctName;
	m_LexFuncts.push_back (CLexFunctRel(SitNodeNo, ParamNodeNo, LexFunctName));
};

void CRusSemStructure::BuildVerbLexFunctParameter(long ClauseNo) 
{
   // создаем узлы на лексических функциях из двух узлов 
   for (long i =0; i < m_Nodes.size(); i++)
     if (      IsInClause(i, ClauseNo) 
		 	&& m_Nodes[i].IsWordContainer()
 	    )
	{
		if (m_Nodes[i].m_LexFunctFields.size() == 0) continue;
		if (m_Nodes[i].m_MainWordNo == -1) continue;
		if (!m_Nodes[i].m_Words[m_Nodes[i].m_MainWordNo].HasPOS (NOUN)) continue;
		

		for (size_t k=0; k < m_Nodes.size(); k++)
		if (    IsInClause(k, ClauseNo)
			 && m_Nodes[k].IsWordContainer()
			 && (m_Nodes[k].m_MainWordNo!= -1)
			 && (i != k) 
		   )
		   BuildVerbLexFunctParameterForTheNodes (i, k);
	 };


};



void CRusSemStructure::BuildAdjOrAdvLexFunctParamNodes(long ClauseNo) 
{
    for (long NodeNo =0; NodeNo< m_Nodes.size(); NodeNo++)
    if (       IsInClause(NodeNo, ClauseNo) 
		 	&& m_Nodes[NodeNo].IsWordContainer()
			&& (m_Nodes[NodeNo].m_MainWordNo != -1)
	   )
	{
		 vector<long> Nodes;
		 vector<long> Rels;
		 GetOutcomingSynRelations(NodeNo, Rels);
		 // формируем множество узлов, которые могут быть лексическими функциями типа Magn  
		 for (long i=0; i < Rels.size(); i++)
			 if  (   (m_SynRelations[Rels[i]].m_SynRelName == "ПРИЛ_СУЩ")
				  || (m_SynRelations[Rels[i]].m_SynRelName == "НАРЕЧ_ГЛАГОЛ")
				 )
			 {
				 vector<long> Rels1;
				 GetOutcomingSynRelations(m_SynRelations[Rels[i]].m_TargetNodeNo, Rels1);
				 bool b = false;;
				 for (long l=0; l < Rels1.size(); l++)
                   if  (   (m_SynRelations[Rels1[l]].m_SynRelName == "ОДНОР_ПРИЛ")
					     ||(m_SynRelations[Rels1[l]].m_SynRelName == "ОДНОР_НАР")
					   )
				   {
					   Nodes.push_back(m_SynRelations[Rels1[l]].m_TargetNodeNo);
					   b = true;
				   };
                 if (b == false)
					 Nodes.push_back(m_SynRelations[Rels[i]].m_TargetNodeNo);
			 };

         
         for (long i=0; i < Nodes.size();i++)
         if (m_Nodes[Nodes[i]].IsPrimitive())
		 {
			 vector<CLexicalFunctionField>::const_iterator It = find(m_Nodes[NodeNo].m_LexFunctFields.begin(), m_Nodes[NodeNo].m_LexFunctFields.end(), m_Nodes[Nodes[i]].m_Words[0].m_Lemma);
		   if (It == m_Nodes[NodeNo].m_LexFunctFields.end()) continue;
		   //ведем стрелку от слова ситуации в LF
            m_LexFuncts.push_back (CLexFunctRel(NodeNo, Nodes[i], It->m_LexFunct));
		 };

	};

};


