/////////////////////////////////////////////////////////////////////////////
// Интерпретация связей
// FillVectorOfGramCortegeAndType()
// GetGramCortegesAndTypeFromRel()
// GetGramCortegeType()
// FillSemRelToGramCortegeMap()
// CreateCortege()
// AddSemRelToGramCortege()

#include "StdAfx.h"

/////////////////////////////////////////////////////////////////////////////

void CEngSemStructure::FillVectorOfGramCortegeAndType(vector<SGramCortegesAndType>& GramCortegesAndTypeV,vector<TCortege>&  GramCorteges, DictTypeEnum type)
{
	for( int i=0; i<GramCorteges.size(); i++ )
	{
		EGramCortegeType GramType = GetGramCortegeType(GramCorteges[i],type);
		if( GramType == Nothing )
			continue;
		int j=0;
		for( ; j<GramCortegesAndTypeV.size(); j++ )
		{
			if( GramCortegesAndTypeV[j].m_Type == GramType )
			{
				SWeightToCortege WeightToCortege;
				WeightToCortege.m_GramCortege = GramCorteges[i];
				GramCortegesAndTypeV[j].m_WeightGramCorteges.push_back(WeightToCortege);
				break;
			}
		}
		if( j >= GramCortegesAndTypeV.size() )
		{
			SGramCortegesAndType GramCortegesAndType;
			GramCortegesAndType.m_Type = GetGramCortegeType(GramCorteges[i],type);

			if( GramCortegesAndType.m_Type == Nothing )
				continue;

			SWeightToCortege WeightToCortege;
			WeightToCortege.m_GramCortege = GramCorteges[i];

			GramCortegesAndType.m_WeightGramCorteges.push_back(WeightToCortege);

			GramCortegesAndTypeV.push_back(GramCortegesAndType);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

void CEngSemStructure::GetGramCortegesAndTypeFromRel(vector<SGramCortegesAndType>& GramCortegesAndTypeV, CEngSemRelation& semRel)
{
	int iEngNode;
	if( semRel.m_bReverseRel )
		iEngNode = semRel.m_TargetNodeNo;
	else
		iEngNode = semRel.m_SourceNodeNo;

	vector<TCortege> GramCorteges;
	if( semRel.m_bInterpreted && !semRel.m_Pattern.IsEmpty() )
	{
		GramCorteges = semRel.m_Pattern.m_GramCorteges;
		FillVectorOfGramCortegeAndType(GramCortegesAndTypeV,GramCorteges,m_Nodes[iEngNode].GetType());
	}
	else
	{
		SemRelToGramCortegeMap_t::iterator iter;
		iter = m_SemRelToGramCortegeMap.find(semRel.m_Valency.m_RelationStr);
		if( iter != m_SemRelToGramCortegeMap.end() )
			GramCortegesAndTypeV = (*iter).second;
	}
}


/////////////////////////////////////////////////////////////////////////////

void CEngSemStructure::TranslateOneActant(int iRel,int iEngNode)
{
	CEngSemRelation& semRel = m_Relations[iRel];
	if( semRel.m_bAuxFlag )
		return;
//
	if( iEngNode != semRel.m_SourceNodeNo )
		return;

	if( m_Nodes[semRel.m_TargetNodeNo].m_NodeType == Copul )
		return;
//
	int iRusActant = m_Nodes[semRel.m_TargetNodeNo].RusNode;
	if( iRusActant ==  -1)
		return;
	if( RusStr.GetNode(iRusActant).IsTimeRossNode() )
		return;

	int iLexFunRel = FindLexFunctBySemRel(iRel);
	if( iLexFunRel != -1 ) 
	{
		if( m_LexFuncts[iLexFunRel].m_Prep.m_UnitNo != ErrUnitNo )
		{
			semRel.m_SynReal.m_Preps.clear();
			semRel.m_SynReal.m_Preps.push_back(m_LexFuncts[iLexFunRel].m_Prep);
			semRel.m_bAuxFlag = true;
			return;
		}
	}
	
	const CSemNode& rusActant = RusStr.GetNode(iRusActant);
	bool bTranslated = false;
	bool bTriedLexFunWordPattern = false;

	for( ;; )
	{

/*		if( IsSubj(semRel) )
		{
			if( IsSubjPattern(semRel) )
				semRel.m_SynReal.m_Cortege = semRel.m_Pattern.m_GramCorteges[0];

			semRel.m_GramType = Subj;
			break;
		}
*/			
		if( (bTranslated=Rule_TranslateRelWithPrepField(iRusActant,iRel,iEngNode)) )
			break;

		if	(		(semRel.m_Valency.m_RelationStr == "LOK") 
				&&	(RusStr.GetNode(iRusActant).GetType() == LocRoss) 
			)
		{
			if(		(semRel.m_RusRel!=-1) 
				&&	RusStr.GetRelation(semRel.m_RusRel)->m_Valency.m_IsAddition 
				&&	(RusStr.GetRelation(semRel.m_RusRel)->m_Valency.m_UnitNo != ErrUnitNo) 
			  )
				break;
		}

		vector<SGramCortegesAndType> GramCortegesAndTypeV;	
		GetGramCortegesAndTypeFromRel(GramCortegesAndTypeV,semRel);

		for( int i=0; i<GramCortegesAndTypeV.size(); i++ )
		{

			EGramCortegeType type = GramCortegesAndTypeV[i].m_Type;
			
			switch( type)
			{
				case Subj:
					bTranslated = Rule_TranslateSubj(iRusActant,iRel,GramCortegesAndTypeV[i].GetCorteges(),iEngNode);
					if (bTranslated)
						semRel.m_GramType = Subj;
				break;

				case PossNp:
					bTranslated = Rule_TranslatePoss(iRusActant,iRel,GramCortegesAndTypeV[i].GetCorteges(),iEngNode);
					if (bTranslated)
						semRel.m_GramType = PossNp;
				break;

				case NP_attr:
					bTranslated = true;
					semRel.m_Position = "<";
					semRel.m_PosType = FromAlgorithmPosType;
					semRel.m_GramType = NP_attr;
				break;

				case PrepNp: 
					bTranslated = Rule_TranslatePrepNounGroup(iRusActant,iRel,GramCortegesAndTypeV[i].GetCorteges(),iEngNode);

					if (bTranslated)
						semRel.m_GramType = PrepNp;

				break;

				case InfinitiveOrGerundGram:
					bTranslated = Rule_TranslateInfinitive(iRusActant,iRel,GramCortegesAndTypeV[i].GetCorteges(),iEngNode);
					if( bTranslated )
						semRel.m_GramType = InfinitiveOrGerundGram;
				break;

				case Obj:
					bTranslated = Rule_TranslateObj(iRusActant,iRel,GramCortegesAndTypeV[i].GetCorteges(),iEngNode);
					if (bTranslated)
						semRel.m_GramType = Obj;
				break;
			}

			if( bTranslated )
				break;
		}

		if( !bTranslated && semRel.m_bInterpreted && !bTriedLexFunWordPattern )
		{
			if( TryToAddNewSemPatternFromLexFunWord(iRel) )
			{
				GramCortegesAndTypeV.clear();
				bTriedLexFunWordPattern = true;
				continue;
			}
		}

		break;
	}

	if( bTranslated )
	{
		semRel.m_bAuxFlag = true;
	}
}

/////////////////////////////////////////////////////////////////////////////

EGramCortegeType CEngSemStructure::GetGramCortegeType(string& gram_str)
{
	if( gram_str == "subj")
		return Subj;

	if( gram_str == "obj" )
		return Obj;

	if(    ( gram_str.find("+NP") != string::npos ) 
		|| (gram_str == "PREP_PHR") 
	  )
		return PrepNp;

	if( (gram_str.find("gerund") != string::npos) || (gram_str.find("+inf") != string::npos ) )
		return InfinitiveOrGerundGram;

	if( (gram_str == "NP_poss") || (gram_str == "PN_poss" ) || (gram_str == "of+NP") )
		return PossNp;

	if( gram_str == "that+sent" || gram_str == "that+subj_clause")
		return SubClause;


	if( gram_str == "NP_attr")
		return NP_attr;

	if( gram_str == "-")
		return Nothing;

	return UnknownGram;
}

/////////////////////////////////////////////////////////////////////////////

EGramCortegeType CEngSemStructure::GetGramCortegeType(TCortege& cortege, DictTypeEnum type)
{
	if( cortege.m_DomItemNos[0] == -1 )
		return UnknownGram;
	string gram_str = GetItemStr(cortege.m_DomItemNos[0], type);
	EGramCortegeType CortegeType = GetGramCortegeType(gram_str);
	if( CortegeType == UnknownGram)
	{
		if( cortege.m_DomItemNos[1] != -1 )
		{
			gram_str = GetItemStr(cortege.m_DomItemNos[1], type);
			CortegeType = GetGramCortegeType(gram_str);
		}
	}

	return CortegeType;
}

/////////////////////////////////////////////////////////////////////////////

void CEngSemStructure::FillSemRelToGramCortegeMap()
{
AddSemRelToGramCortege("MULTI", CreateCortege("of+NP", "D_1"));
	SemRelToGramCortegeMap_t::iterator it;
	//отсортируем GF по частоте.
	for( it = m_SemRelToGramCortegeMap.begin(); it != m_SemRelToGramCortegeMap.end(); it++ )
		for( int i=0; i<(*it).second.size(); i++ )
			sort((*it).second[i].m_WeightGramCorteges.begin(),(*it).second[i].m_WeightGramCorteges.end());
}

/////////////////////////////////////////////////////////////////////////////

TCortege CEngSemStructure::CreateCortege(string strGX, string strDomen)
{
	TCortege cortege;
	cortege.m_DomItemNos[0] = GetRoss(Aoss)->GetItemNoByItemStr(strGX.c_str(),GetRoss(Aoss)->GetDomenNoByDomStr(strDomen.c_str()));
	cortege.m_DomItemNos[1] = -1;
	return cortege;
}

/////////////////////////////////////////////////////////////////////////////


void CEngSemStructure::AddSemRelToGramCortege(string semRel, TCortege cortege)
{
	if( cortege.m_DomItemNos[0] == -1 )
		return;

	SGramCortegesAndType	GramCortegesAndType;
	vector<SGramCortegesAndType> GramCortegesAndTypeVector;
	SemRelToGramCortegeMap_t::iterator it;

	if( (it=m_SemRelToGramCortegeMap.find(semRel)) != m_SemRelToGramCortegeMap.end() )
	{
		int i=0;
		for( ; i<(*it).second.size(); i++ )
		{
			if( (*it).second[i].m_Type == GetGramCortegeType(cortege,Aoss) )
			{
				int p = 0 ;
				for(; p < (*it).second[i].m_WeightGramCorteges.size() ; p++ )
				{
					if( (*it).second[i].m_WeightGramCorteges[p].m_GramCortege.HasEqualItems(cortege, 3 ) )
					{
						(*it).second[i].m_WeightGramCorteges[p].m_Weight++;
						break;
					}
				}
				if( p == (*it).second[i].m_WeightGramCorteges.size() )
				{
					SWeightToCortege WeightToCortege;
					WeightToCortege.m_GramCortege = cortege;
					WeightToCortege.m_Weight = 1;
					(*it).second[i].m_WeightGramCorteges.push_back(WeightToCortege);
				}
				break;
			}
		}
		
		if( i >= (*it).second.size() )
		{
			SGramCortegesAndType GramCortegesAndType;
			GramCortegesAndType.m_Type = GetGramCortegeType(cortege, Aoss);
			SWeightToCortege WeightToCortege;
			WeightToCortege.m_GramCortege = cortege;
			WeightToCortege.m_Weight = 1;
			GramCortegesAndType.m_WeightGramCorteges.push_back(WeightToCortege);
			(*it).second.push_back(GramCortegesAndType);
		}
	}
	else
	{
		GramCortegesAndType.m_Type = GetGramCortegeType(cortege,Aoss);
		SWeightToCortege WeightToCortege;
		WeightToCortege.m_Weight = 1;
		WeightToCortege.m_GramCortege = cortege;
		GramCortegesAndType.m_WeightGramCorteges.push_back(WeightToCortege);
		GramCortegesAndTypeVector.push_back(GramCortegesAndType);
		m_SemRelToGramCortegeMap[semRel] = GramCortegesAndTypeVector;
	}
}

/////////////////////////////////////////////////////////////////////////////
