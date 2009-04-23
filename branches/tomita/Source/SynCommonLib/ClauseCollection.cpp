// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#include "stdafx.h"
#include "Clause.h"



void CClauseCollection::Clear()
{
	m_Clauses.clear();
	m_ClausesRelations.clear();
};

void CClauseCollection::Renumerate()
{
	for (size_t  i = 0;  i < m_Clauses.size() ; i++)
		m_Clauses[i].m_SelfClauseNo = i;
};

void CClauseCollection::SortClauses()
{
	sort(m_Clauses.begin() , m_Clauses.end(), GladkijPeriodLess );
	Renumerate();
}

CClause& CClauseCollection::AddClause(CClause& pr_clause)
{
	int i = 0;
	for( ; i < m_Clauses.size() ; i++)
	{
		if( !GladkijPeriodLess(m_Clauses[i], pr_clause))
		{
			m_Clauses.insert(m_Clauses.begin() + i, pr_clause);
			break;
		}
	}
	if( i == m_Clauses.size() )
		m_Clauses.push_back(pr_clause);
		
	SortClauses();
	
	for( i = 0 ; i < m_Clauses.size() ; i++)
		if ( m_Clauses[i] == pr_clause )
			break;

	assert (i < m_Clauses.size());

	return m_Clauses[i];
	
};
void CClauseCollection::DeleteClause(int iClauseNum)
{
	m_Clauses.erase(m_Clauses.begin() + iClauseNum);
	Renumerate();
}
	


void CClauseCollection::ChangeRightBorderPr(int i, int iNewBorder)
{
	
	CClause& pPrClause = m_Clauses[i];
	pPrClause.m_iLastWord = iNewBorder;
	/*
	  пройдемся по вершинам фрагмента, если какая-та вершина 
	  теперь находится за пределами фрагмента, тогда ее нужно удалить
	*/
	for (int j = 0; j < pPrClause.m_vectorTypes.size(); j++)
		if (pPrClause.m_vectorTypes[j].m_Root.m_WordNo > iNewBorder)
		{
			pPrClause.DeleteClauseType(j);
			j--;
		};

}

int CClauseCollection::FindPreviousClauseIndex(int ClauseNo) const
{
	const CClause& pClause = m_Clauses[ClauseNo];	
	vector<CClause>::const_iterator  pClause1;	
	do
	{
		ClauseNo--;
		if (ClauseNo == -1) return ClauseNo;
		pClause1 = m_Clauses.begin() + ClauseNo;		
	}
	while( pClause.m_iFirstWord <= pClause1->m_iFirstWord );	
	
	return ClauseNo;
}





int CClauseCollection::GetMininalParentByClauseNo(int ClauseNo) const 
{
	for( int i = ClauseNo + 1; i < m_Clauses.size(); i++ )
		if(m_Clauses[ClauseNo].is_part_of(m_Clauses[i]))
			return i;

	return -1;
}

int CClauseCollection::GetMinClauseByWordNo(int WordNo) const 
{
	CPeriod P(WordNo);
	for( int i = 0; i < m_Clauses.size(); i++ )
		if(P.is_part_of(m_Clauses[i]))
			return i;

	return -1;
}

bool CClauseCollection::IsEnclosedClause(int iClauseNum) const 
{
	const CClause& pClause = m_Clauses[iClauseNum];
	for( int i = iClauseNum+1 ; i< m_Clauses.size(); i++ )
	{
		const CClause& pClause1 = m_Clauses[i];
		if(pClause1.m_iFirstWord <= pClause.m_iFirstWord)
			return true;
	}
	return false;
}



void CClauseCollection::GetClausesByLastWord(vector<int>& Parents, const CPeriod* pLeftPeriod) const
{
	Parents.clear();

	for (int k = 0; k < m_Clauses.size(); k++)
	{
		const CClause& PrimCl = m_Clauses[k]; 

		if (   pLeftPeriod->m_iLastWord == PrimCl.m_iLastWord 
			&& pLeftPeriod->is_part_of(PrimCl)
		  )
			Parents.push_back(k);
	}
}

void CClauseCollection::GetClausesByFirstWord(vector<int>& Parents, const CPeriod* pRightPeriod) const
{
	Parents.clear();

	for (int k = 0; k < m_Clauses.size(); k++)
	{
		const CClause& PrimCl = m_Clauses[k]; 

		if (   pRightPeriod->m_iFirstWord == PrimCl.m_iFirstWord 
			&& pRightPeriod->is_part_of(PrimCl)
		  )
			Parents.push_back(k);
	}
}


int CClauseCollection::FindClauseIndexByPeriod(const CPeriod& period) const
{
	for(int i = 0 ; i < m_Clauses.size() ; i++)
	{
		if( m_Clauses[i] == period )
			return i;
	}

	return -1;
}




int CClauseCollection::GetMaxClauseByFirstWord(int iFWrd) const
{
	for (int i = m_Clauses.size()-1; i >= 0; i--)
		if (m_Clauses[i].m_iFirstWord == iFWrd)
			return i;

	return -1;	
}

int CClauseCollection::GetMinClauseByFirstWord(int iFWrd) const
{
	for (int i = 0; i < m_Clauses.size(); i++)
		if (m_Clauses[i].m_iFirstWord == iFWrd)
			return i;

	return -1;
}

int CClauseCollection::GetMinClauseByLastWord(int LastWordNo) const
{
	for (int i = 0; i < m_Clauses.size(); i++)
		if (m_Clauses[i].m_iLastWord == LastWordNo)
			return i;

	return -1;
}




int CClauseCollection::GetMaxClauseByLastWord(int iLWrd) const 
{
	for (int i = m_Clauses.size()-1; i >= 0; i--)
		if (m_Clauses[i].m_iLastWord == iLWrd)
			return i;

	return -1;			
}

int CClauseCollection::GetMaxChildByLastWord(CPeriod ParentClause, int LastWordNo) const
{
	if (LastWordNo >  ParentClause.m_iLastWord) return -1;

	for (int i = m_Clauses.size()-1; i >= 0; i--)
		if (      m_Clauses[i].is_inside_of( ParentClause )
			  && (m_Clauses[i].m_iLastWord == LastWordNo)

			)
			return i;

	return -1;		
}


/*
	if WordNo+1 is the beginning of a subclause, the function  returns 
	the word after this subclause. Otherwise it returns  WordNo+1.
*/
int CClauseCollection::PassSubClauses(CPeriod ParentClause, int WordNo) const
{
	int iEncloseCl = GetMaxChildByFirstWord(ParentClause, WordNo+1);
	if	(iEncloseCl != -1 ) 
		{
			assert (m_Clauses[iEncloseCl].is_part_of(ParentClause));
			return m_Clauses[iEncloseCl].m_iLastWord+1;
		}
	else
		return WordNo+1;
}

int CClauseCollection::GetMaxChildByFirstWord(CPeriod ParentClause, int FirstWordNo) const
{
	if (FirstWordNo <  ParentClause.m_iFirstWord) return -1;

	for (int i = m_Clauses.size()-1; i >= 0; i--)
		if (  	  m_Clauses[i].m_iFirstWord == FirstWordNo 
			  &&  m_Clauses[i].is_inside_of( ParentClause )
			  &&  !m_Clauses[i].m_bIgnoreWhileBuildingSynVariants
			)
			return i;

	return -1;		
}


const CClause* CClauseCollection::FindClauseByPeriod(const CPeriod&  period) const
{
	int ii = FindClauseIndexByPeriod(period);
	if( ii == -1)
		return NULL;
	else
		return &m_Clauses[ii];
}
CClause* CClauseCollection::FindClauseByPeriod(const CPeriod&  period)
{
	int ii = FindClauseIndexByPeriod(period);
	if( ii == -1)
		return NULL;
	else
		return &m_Clauses[ii];
}
