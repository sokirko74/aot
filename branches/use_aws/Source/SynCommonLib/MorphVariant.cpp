// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#include "stdafx.h"
#include "MorphVariant.h"
#include "Sentence.h"


CSynUnit::CSynUnit(const CAgramtab* pGramTab) : CAncodePattern(pGramTab)
{
	m_iHomonymNum = -1;
	m_Type = EWord;
	m_iClauseTypeNum = -1;
	m_SentPeriod.m_iFirstWord = -1;
	m_SentPeriod.m_iLastWord = -1;
	m_FoundPrepDependCases = 0;
	m_bReflexive = false;
}



CMorphVariant::CMorphVariant(const CAgramtab* pGramTab)
{
    m_pGramTab = pGramTab;
	Reset(); 
};


void CMorphVariant::Reset()
{
	m_vectorGroups.Clear();
	m_SynUnits.clear();
	m_Subjects.clear();
	m_iPredk = -1;
	m_iWeight = 0;
	m_ClauseTypeNo = -1;
	m_bGoodSubject = false;
};	

bool CMorphVariant::HasSubj() const
{
	return !m_Subjects.empty();
};

bool CMorphVariant::HasPredk() const
{
	return m_iPredk != -1;
}

void CMorphVariant::PushHomonymNum(int i)
{ 
	CSynUnit Unit(m_pGramTab);
	Unit.m_iHomonymNum = i;
	m_SynUnits.push_back(Unit); 
};

void CMorphVariant::PopHomonymNum()
{ 
	assert (!m_SynUnits.empty());
	m_SynUnits.erase(m_SynUnits.begin() + m_SynUnits.size() - 1); 
};

void CMorphVariant::ResetSubj()
{
	m_Subjects.clear();
	m_bGoodSubject = false;
};	

int CMorphVariant::GetFirstSubject() const
{
	if ( m_Subjects.empty() ) return -1;
	return m_Subjects[0];
};	

size_t CMorphVariant::GetUnitsCount() const 
{
	return m_SynUnits.size();
}

EUnitType CMorphVariant::GetUnitType(int iUnit) const
{
	return m_SynUnits[iUnit].m_Type;
}

const vector<int>& CMorphVariant::GetSimplePrepNos(int i) const
{
	return m_SynUnits[i].m_SimplePrepNos;
}

int CMorphVariant::GetHomNum(int iMemb) const
{		
	return m_SynUnits[iMemb].m_iHomonymNum;
}

/*
 сравнение CMorphVariant-ов по номерам омонимов и по номерам вершин вложенных клауз
*/
bool CMorphVariant::EqualTo(const CMorphVariant& variant) const
{
	if( variant.m_SynUnits.size() != m_SynUnits.size() )
		return false;

	for(int i = 0 ; i < m_SynUnits.size() ; i++ )
		if (m_SynUnits[i].m_Type == EWord)
		{
			if(m_SynUnits[i].m_iHomonymNum != variant.m_SynUnits[i].m_iHomonymNum)
				return false;
		}
		else
		{
			if (m_SynUnits[i].m_Type == EClause)
				if(m_SynUnits[i].m_iClauseTypeNum != variant.m_SynUnits[i].m_iClauseTypeNum)
					return false;
		};
			
	return true;
}


int	CMorphVariant::UnitNoByWordNo(int iWord) const
{
	if (iWord == -1) return -1;
	CPeriod P(iWord);

	for (int UnitNo = 0; UnitNo < m_SynUnits.size(); UnitNo++)
		if	( P.is_part_of(m_SynUnits[UnitNo].m_SentPeriod)	)
			return UnitNo;

	assert (false);

	return -1;
}




CPeriod CMorphVariant::GetSentenceCoordinates(const CPeriod& PeriodInUnits) const 
{
	int w1 = GetUnitFirstWord(PeriodInUnits.m_iFirstWord);
	int w2 = GetUnitLastWord(PeriodInUnits.m_iLastWord);
	return CPeriod (w1, w2);
}

CPeriod CMorphVariant::GetSentPeriod() const
{
	assert ( !m_SynUnits.empty() );
	return CPeriod(m_SynUnits.begin()->m_SentPeriod.m_iFirstWord, m_SynUnits.back().m_SentPeriod.m_iLastWord);

};


bool CMorphVariant::IsInSubclause(int iWrd) const
{
	int UnitNo = UnitNoByWordNo(iWrd); 

	return		(UnitNo != -1) 
			&&	m_SynUnits[UnitNo].m_Type == EClause;
}


int CMorphVariant::GetUnitLastWord(int iUnit) const
{
	return m_SynUnits[iUnit].m_SentPeriod.m_iLastWord;
}


int CMorphVariant::GetUnitFirstWord(int iUnit) const
{
	return m_SynUnits[iUnit].m_SentPeriod.m_iFirstWord;
}



int CMorphVariant::GetClauseLastWord(int iWord) const
{
	int UnitNo = UnitNoByWordNo(iWord); 
	if (UnitNo == -1) return -1;
	return m_SynUnits[UnitNo].m_SentPeriod.m_iLastWord;
}


int CMorphVariant::GetClauseFirstWord(int iWord) const
{
	int UnitNo = UnitNoByWordNo(iWord); 
	if (UnitNo == -1) return -1;
	return m_SynUnits[UnitNo].m_SentPeriod.m_iFirstWord;

}






CWordAndHomonym::CWordAndHomonym()
{
	SetEmpty();
};
void CWordAndHomonym::SetEmpty ()
{
	m_WordNo = -1;
	m_HomonymNo = -1;
};
bool CWordAndHomonym::IsEmpty () const
{
	return m_WordNo == -1;
};

bool CWordAndHomonym::operator == (const  CWordAndHomonym& X) const
{
	return		m_WordNo == X.m_WordNo
			&&	m_HomonymNo ==X.m_HomonymNo;
};



void CWordAndHomonym::UpdateWhenDeleteHomonym(int WordNo, int DeleteHomonymNo)
{
	if( m_WordNo != WordNo) return;

	if(m_HomonymNo == DeleteHomonymNo) 
			m_HomonymNo = -1;
		else
			if( m_HomonymNo > DeleteHomonymNo )
				m_HomonymNo--;
};

bool CWordAndHomonym::IsValid(const CPeriod& Period, const CSentence* Parent) const
{
	if (IsEmpty()) return true;
	if (m_WordNo > Period.m_iLastWord) return false;
	if (m_WordNo < Period.m_iFirstWord) return false;
	if (m_WordNo >=  Parent->m_Words.size()) return false;
	if (m_HomonymNo == -1) return false;
	if (m_HomonymNo >= Parent->m_Words[m_WordNo].m_Homonyms.size()) return false;
	return true;
};

void CWordAndHomonym::UpdateWhenInsertWord(int InsertWordNo)
{
	if(m_WordNo >= InsertWordNo )
		m_WordNo++;
};



