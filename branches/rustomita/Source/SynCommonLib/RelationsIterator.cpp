// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#include "stdafx.h"
#include "RelationsIterator.h"

//============================================================
//==============  CSynOutputRelation =========================
//============================================================
const CSyntaxOpt* CSynOutputRelation:: GetOpt() const 
{
	return m_pSyntaxOptions;
};
bool CSynOutputRelation::Init(const CRelation& Rel,int iSourceGroup, int iTargetGroup, const CSyntaxOpt* pSyntaxOptions)
{
	m_pSyntaxOptions = pSyntaxOptions;
	m_iSourceGroup = iSourceGroup;
	m_iTargetGroup = iTargetGroup;
	m_Relation = Rel;
	return true;
}
void CSynOutputRelation::CopyFrom(const CSynOutputRelation& X)
{
	m_iSourceGroup = X.m_iSourceGroup;
	m_iTargetGroup = X.m_iTargetGroup;
	m_Relation = X.m_Relation;
	m_pSyntaxOptions = X.m_pSyntaxOptions;
};



//============================================================
//==============  CRelationsIterator =========================
//============================================================

const CSyntaxOpt* CRelationsIterator::GetOpt() const
{
	return m_pSent->GetOpt();
};


void CRelationsIterator::SetSentence(const CSentence* pSent) 
{
	m_pSent = pSent;
}

const vector<CGroup>&  CRelationsIterator::GetFirmGroups() const 
{
	return m_vectorGroups;
};

const vector<CSynOutputRelation>&	CRelationsIterator::GetRelations() const
{
	return m_vectorIRelations;
};

void CRelationsIterator::AddClauseNoAndVariantNo(long iClauseNo, long iVarNo)
{
	CSVI pSynVar = m_pSent->m_Clauses[iClauseNo].m_SynVariants.begin();
	for (int i = 0; i < iVarNo; i++)
			pSynVar++;

	m_mapClauseNo2SynVar[iClauseNo] = pSynVar;
}







bool CRelationsIterator::BuildRelations()
{
	try
	{
		// строим отношения внутри клауз
		map<int,CSVI>::iterator it ;
		for( it = m_mapClauseNo2SynVar.begin() ; it != m_mapClauseNo2SynVar.end() ; it++ )
		{
			if( !BuildRelationsInClause(it->first, it->second) )
				return false;			
		}

		// строим межклаузные отношения
		for(int i = 0 ; i < m_pSent->m_ClausesRelations.size() ; i++ )
		{
			const CRelation& rel = m_pSent->m_ClausesRelations[i];
			map<int,CSVI>::iterator itSource = m_mapClauseNo2SynVar.find(rel.m_SourceClause.m_iClauseNo);
			map<int,CSVI>::iterator itTarget = m_mapClauseNo2SynVar.find(rel.m_TargetClause.m_iClauseNo);

			if( (  itSource == m_mapClauseNo2SynVar.end() ) ||
				(  itTarget == m_mapClauseNo2SynVar.end() ) )
				continue;

			const CClause& Source = m_pSent->m_Clauses[rel.m_SourceClause.m_iClauseNo];
			const CClause& Target = m_pSent->m_Clauses[rel.m_TargetClause.m_iClauseNo];
			CRelation ClauseRel = rel;
			ClauseRel.m_iFirstWord = Source.GetMainWordOfTheClauseForThisVariant(*itSource->second, rel.m_SourceClause);
			ClauseRel.m_iLastWord = Target.GetMainWordOfTheClauseForThisVariant(*itTarget->second, rel.m_TargetClause);

			
			if	(			(ClauseRel.m_iFirstWord == -1) 
						||	(ClauseRel.m_iLastWord == -1) 
				)				
				continue;
			
			int iSourceGr = FindFirmGroupClauseRelations(itSource->first, itSource->second, ClauseRel.m_iFirstWord);
			int iTargetGr = FindFirmGroupClauseRelations(itTarget->first, itTarget->second, ClauseRel.m_iLastWord);
	
			CSynOutputRelation R;;
			R.Init(ClauseRel, iSourceGr, iTargetGr, m_pSent->m_pSyntaxOptions);
			m_vectorIRelations.push_back(R);
		}
	}
	catch(...)
	{
		return false;
	}	
	return true;
}


bool CRelationsIterator::BuildRelationsOfGroups(const CClause& clause, CSVI pSynVar)
{
    for(int i = 0 ; i < pSynVar->m_vectorGroups.m_Relations.size() ; i++ )
	{
		const CRelation& rel = pSynVar->m_vectorGroups.m_Relations[i];		

		if	(		(rel.type == GetOpt()->m_DisruptConjRelation) 
				&&	( rel.m_iFirstWord == rel.m_iLastWord - 1) 
			)
		continue;

		map<int,CSVI>::iterator itSource;
		map<int,CSVI>::iterator itTarget;


		itSource = m_mapClauseNo2SynVar.find(rel.m_SourceClause.m_iClauseNo);
		itTarget = m_mapClauseNo2SynVar.find(rel.m_SourceClause.m_iClauseNo);

		if(		(itSource == m_mapClauseNo2SynVar.end() )
			||  (itTarget == m_mapClauseNo2SynVar.end() ))
			continue;

		if	(		( (rel.m_SourceType == EClause) && (rel.m_SourceClause.m_pClauseVar != &*itSource->second))
				||	( (rel.m_TargetType == EClause) && (rel.m_TargetClause.m_pClauseVar != &*itTarget->second)) 
			)
			continue;		
		
		const CGroup* sourceGroup = NULL, *targetGroup = NULL;
		int iSourceGroup = -1, iTargetGroup = -1;

		sourceGroup = clause.GetLastFirmHost( rel.m_iFirstWord, pSynVar );
		if(  sourceGroup )
			iSourceGroup = FindFirmGroup(*sourceGroup);

		targetGroup = clause.GetLastFirmHost( rel.m_iLastWord, pSynVar );
		if( targetGroup )			
			iTargetGroup = FindFirmGroup(*targetGroup);	
		
		if( iSourceGroup == -1 )
			iSourceGroup = TryToFindDisruptConjGroup(pSynVar, rel, rel.m_iFirstWord ); 

		if( iTargetGroup == -1 )
			iTargetGroup = TryToFindDisruptConjGroup(pSynVar, rel, rel.m_iLastWord  ); 
		
		
        if (        (iSourceGroup == -1 && (rel.m_iFirstWord != rel.m_iLastWord))
                ||  (iSourceGroup != iTargetGroup)
            )
        {
		    CSynOutputRelation R;;
		    R.Init(rel, iSourceGroup, iTargetGroup, m_pSent->m_pSyntaxOptions);
		    m_vectorIRelations.push_back(R);
        }
	}

	return true;
}


int FindRelPeriod(CSVI pSynVar, const  CPeriod& rel) 
{
    for(int i = 0 ; i < pSynVar->m_vectorGroups.m_Relations.size() ; i++ )
		if( pSynVar->m_vectorGroups.m_Relations[i] == rel )
			return i;
	return -1;
}

int CRelationsIterator::TryToFindDisruptConjGroup(CSVI pSynVar, const CRelation& rel, int iWord ) 
{
	if(		(rel.type != GetOpt()->m_DisruptConjGroupType)  
		&& ( rel.type != GetOpt()->m_DisruptConjRelation )
	 )
		return -1;

	for(int i = 0 ; i < m_vectorGroups.size() ; i++ )
		if( CPeriod(iWord).is_part_of(m_vectorGroups[i])  ) //если уже была до этого построена
			return i;

	int iRBorder = iWord;
	int ii;
	while( (ii = FindRelPeriod(pSynVar, CPeriod(iRBorder, iRBorder + 1))) != -1 )
	{
        if( pSynVar->m_vectorGroups.m_Relations[ii].type != GetOpt()->m_DisruptConjRelation )
			break;
		iRBorder++;
	}
	
	if( iRBorder == iWord)
		return -1;

	CGroup conj_group;
	conj_group.m_iFirstWord = iWord;
	conj_group.m_iLastWord = iRBorder;
	conj_group.m_GroupType = GetOpt()->m_DisruptConjRelation;
	m_vectorGroups.push_back(conj_group);

	return m_vectorGroups.size() - 1;
}






bool CRelationsIterator::BuildRelationsInClause(int iClauseNum, CSVI pSynVar)
{
	try
	{
		if( (iClauseNum < 0) || ( iClauseNum >= m_pSent->GetClausesCount()))
			return false;

		const CClause& clause = m_pSent->GetClause(iClauseNum);

		for(int i = 0 ; i  < pSynVar->m_vectorGroups.GetGroups().size() ; i++ )
		{
			
			const CGroup& G = pSynVar->m_vectorGroups.GetGroups()[i];
			if( GetOpt()->is_firm_group(G.m_GroupType) )
			{
				m_vectorGroups.push_back(G);
				// оставляем только максимальную группу из всех жестких
				for (int k = m_vectorGroups.size()-2; k >=0; k--)
					if (m_vectorGroups[k].is_part_of(G))
						m_vectorGroups.erase(m_vectorGroups.begin() + k);
				continue;
			}
    	}

        if( !BuildRelationsOfGroups(clause, pSynVar) )
		    return false;

		BuildSubjRel(*pSynVar, iClauseNum);
	}
	catch(...)
	{
		return false;
	}

	return true;
}

void CRelationsIterator::BuildSubjRel(const CMorphVariant& SynVar, int iClauseNo)
{	
	if( (SynVar.m_iPredk != -1) && SynVar.HasSubj() )
	{
		CRelation Relation;
		Relation.m_SourceClause.m_iClauseNo = iClauseNo;
		Relation.m_TargetClause.m_iClauseNo = iClauseNo;
		Relation.m_TargetType = EWord;
		Relation.m_SourceType = EWord;
		Relation.type = GetOpt()->m_SubjRelation;
		Relation.m_iFirstWord = SynVar.GetSentenceCoordinates(SynVar.m_iPredk).m_iFirstWord;
		Relation.m_iLastWord = SynVar.GetSentenceCoordinates(SynVar.GetFirstSubject()).m_iFirstWord;

		CSynOutputRelation R;
		R.Init(Relation, -1, -1, m_pSent->m_pSyntaxOptions);
		m_vectorIRelations.push_back(R);
	}
}

void	CRelationsIterator::ResetAll()
{
	m_mapClauseNo2SynVar.clear();
	m_vectorIRelations.clear();
	m_vectorGroups.clear();
};

int CRelationsIterator::FindFirmGroup(const CGroup& group) const
{
	for(int i = 0 ; i < m_vectorGroups.size() ; i++ )
		if(	(m_vectorGroups[i].m_iFirstWord == group.m_iFirstWord) &&
			(m_vectorGroups[i].m_iLastWord == group.m_iLastWord)
		  )
		  return i;

	return -1;
}


int CRelationsIterator::FindFirmGroupClauseRelations(int iClauseNum, CSVI pSynVar, int iRelMember)
{
	if( (iClauseNum < 0) || ( iClauseNum >= m_pSent->GetClausesCount()))
		return -1;

	const CClause& clause = m_pSent->GetClause(iClauseNum);
	
	const CGroup* MemberGroup = NULL;

	MemberGroup = clause.GetLastFirmHost( iRelMember, pSynVar );
	if(  MemberGroup )
		return  FindFirmGroup(*MemberGroup);
		
	return -1;
}
