#include "stdafx.h"
#include "VisualSentences.h"


CVisualClause::CVisualClause()
{
	m_iActiveType = -1;
	m_strDescription.Empty();
	m_bBestVariant = TRUE;
}

CVisualClause::~CVisualClause()
{

	CClauseType* pType;

	for(int i = 0 ; i < m_arrTypes.GetSize() ; i++ )
	{
		pType = (CClauseType*)m_arrTypes.GetAt(i);
		delete pType;
	}

}


void CVisualClause::SetActiveType(int iType)
{
	m_iActiveType = iType;	
	if (m_iActiveType == -1)
		m_strTypeName = "";
	else
		m_strTypeName = ((CClauseType*)m_arrTypes.GetAt(m_iActiveType))->m_strName;		
	m_strDescription.Empty();
	if( !m_strConjsName.IsEmpty() )
	{

		m_strDescription = m_strTypeName + "; ";
		m_strDescription += m_strConjsName;
	}
	else
		m_strDescription = m_strTypeName;

	char s [20];
	sprintf (s, "%i",  m_Weight);
	m_strDescription += CString(";")+CString(s);

}

BOOL CVisualClause::InitVisualClause(IClausePtr& piClause)
{
	try
	{
		assert (m_pWordsArr);

		m_iFirstWord = piClause->GetFirstWord();
		m_iLastWord = piClause->GetLastWord();

		{
			int RelWordNo = piClause->RelativeWord;
			if (RelWordNo != -1)
			{
				CVisualWord* RelativeWord = (CVisualWord*)m_pWordsArr->GetAt(RelWordNo);
				RelativeWord->m_ReferenceWordNo  = piClause->AntecedentWord;
			};
		};
		_bstr_t bstrDescr = piClause->GetDescription();
		m_strConjsName = CString((char*)bstrDescr);
		int iTypeCount = piClause->GetClauseRootsCount();

		BOOL bFirst = TRUE;
	
		CString strPrev("");
		int iPrev = -2;
		for(int i = 0 ; i < iTypeCount ; i++)
		{
			
			IClauseRootPtr piType = piClause->GetClauseRoots(i);

			int iWordNum = piType->RootWordNo;
			_bstr_t bstrDesc = piType->GetDescription();

			CClauseType* pType = new CClauseType;

			if( !pType->Init(piType) )
				return FALSE;

			strPrev = pType->m_strName;
			iPrev = pType->m_iWordNum;

			if( bFirst )
				if(pType->HasHom(0) )
				{
					m_iActiveType = i;
					bFirst = FALSE;				
					pType->m_bHomChoosen = TRUE;
				}

			if( (pType->HasHom(-1)) || (pType->HasHom(0)) )
				pType->m_bHomChoosen = TRUE;

			m_arrTypes.Add(pType);
		}

		if( bFirst )
			m_iActiveType = iTypeCount - 1;
	}
	catch(...)
	{
		ErrorMessage ("VisualSynan", "An exception occured while reading types of the clause");
		return  FALSE;
	}

	try
	{
		SetActiveType(m_iActiveType);



		ASSERT(piClause->GetVariantsCount() > 0);
		for(int  i = 0 ; i < piClause->GetVariantsCount() ; i++ )
		{
			CVisualSynVariant SynVar;
			SynVar.InitClauseVariant(piClause, i);
			m_SynVars.push_back(SynVar);
		}
		if (m_SynVars.size() > 0)
			m_Weight = m_SynVars[0].m_Weight;
	}
	catch(...)
	{
		ErrorMessage ("VisualSynan", "An exception occured while reading synvarinats of the clause");
		return  FALSE;
	}

	return TRUE;
}

void CVisualClause::SetBestHomonyms()
{
	if( m_SynVars.size() == 0)
	{
		::MessageBox (0, "Clause with no synvariants found", "VisualSynan", MB_OK);
		return;
	};
	CVisualSynVariant& SynVar = m_SynVars[0];
	CVisualWord* pWord;
	for(int i = 0 ; i < SynVar.m_vectorUnits.size() ; i++ )
	{
		if( SynVar.m_vectorUnits[i].m_Type == EWord )
		{
			pWord = (CVisualWord*)m_pWordsArr->GetAt(SynVar.m_vectorUnits[i].m_iWordNum);
			pWord->m_iActiveHomonym = SynVar.m_vectorUnits[i].m_iHommonyNum;
			pWord->m_strActiveGrammems = SynVar.m_vectorUnits[i].m_strGrammems;
			CSynHomonym* pHom = ((CSynHomonym*)pWord->m_arrHomonyms.GetAt(pWord->m_iActiveHomonym));
			if( !SynVar.m_vectorUnits[i].m_strOborotsNum.IsEmpty() )
				pWord->m_strSomeDescr = SynVar.m_vectorUnits[i].m_strOborotsNum;
			else
				pWord->m_strSomeDescr = pHom->m_strOborotsNum;

			m_Weight = SynVar.m_Weight;
			SetActiveType(SynVar.m_iClauseTypeNum);
			
		}
	}
}

BOOL CVisualClause::CalculateClauseGroupCoordinates(CDC* pDC,int iWidth,BOOL bShowGroups, int& iLine)
{
	int iBottom = ((CVisualWord*)m_pWordsArr->GetAt(m_iFirstWord))->m_WordRect.bottom;		
	CalculateGroupsCoordinates(pDC,iWidth, iLine, iBottom);

	return TRUE;

}




BOOL CVisualClause::FillActiveGroupsArray(BOOL bUseGroupID, map<int, int>& mapClauseNumTypeNum, int iClauseNum)
{			

	if( m_bBestVariant )
	{
		SetBestHomonyms();
		m_bBestVariant = FALSE;
	}

	vector<SUnit> vectorUnits;
	CVisualWord* pWord;

	ASSERT(m_SynVars.size() > 0);
	if( m_SynVars.size() == 0 )
			return FALSE;
	CVisualSynVariant& SynVar1 = m_SynVars[0];
	int sz = mapClauseNumTypeNum.size();
	int i = 0;
	for( ; i < SynVar1.m_vectorUnits.size() ; i++ )	
	{
		SUnit unit;
		if( SynVar1.m_vectorUnits[i].m_Type == EWord )
		{
			pWord = (CVisualWord*)(m_pWordsArr->GetAt(SynVar1.m_vectorUnits[i].m_iWordNum));
			pWord->ResetSubjAndPred();			
			unit.m_iHommonyNum = pWord->m_iActiveHomonym;
			unit.m_iWordNum = SynVar1.m_vectorUnits[i].m_iWordNum;			
		}
		else
		{
			//map<int, int>::iterator it = mapClauseNumTypeNum.find(SynVar1.m_vectorUnits[i]..m_iClauseNum);
			//ASSERT(it != mapClauseNumTypeNum.end() );
			//if( it == mapClauseNumTypeNum.end() )
			//	return FALSE;
			//unit.m_iClauseTypeNum = it->second;
			unit.m_iClauseTypeNum = SynVar1.m_vectorUnits[i].m_iClauseTypeNum;
			unit.m_iClauseNum = SynVar1.m_vectorUnits[i].m_iClauseNum;			
		}
		unit.m_Type = SynVar1.m_vectorUnits[i].m_Type;
		vectorUnits.push_back(unit);
	}

	for(i = 0 ; i < m_SynVars.size() ; i++ )
	{
		if( m_SynVars[i] == vectorUnits )
			break;
	}
	
	if(i == m_SynVars.size() )
		return FALSE;

	CVisualSynVariant& SynVar = m_SynVars[i];

	m_Weight = SynVar.m_Weight;
	SetActiveType(SynVar.m_iClauseTypeNum);
	
	mapClauseNumTypeNum[iClauseNum] = SynVar.m_iClauseTypeNum;

	for(int k = 0 ; k < SynVar.m_vectorUnits.size() ; k++ )
	{
		if( SynVar.m_vectorUnits[k].m_Type == EWord )
		{
			CVisualWord* pWord = (CVisualWord*)m_pWordsArr->GetAt(SynVar.m_vectorUnits[k].m_iWordNum);
			pWord->m_strActiveGrammems = SynVar.m_vectorUnits[k].m_strGrammems;

			CSynHomonym* pHom = ((CSynHomonym*)pWord->m_arrHomonyms.GetAt(SynVar.m_vectorUnits[k].m_iHommonyNum));
			if( !SynVar.m_vectorUnits[k].m_strOborotsNum.IsEmpty() )
				pWord->m_strSomeDescr = SynVar.m_vectorUnits[k].m_strOborotsNum;
			else
				pWord->m_strSomeDescr = pHom->m_strOborotsNum;


		}
	}

	if (SynVar.m_iPredk != -1)
	{
		pWord = (CVisualWord*)m_pWordsArr->GetAt(SynVar.m_iPredk);
		CSynHomonym* pHom = (CSynHomonym*)pWord->m_arrHomonyms.GetAt(pWord->m_iActiveHomonym);
		pHom->m_bPredk = TRUE;
		for (size_t l=0; l<  pWord->m_MainVerbs.size(); l++)
		{
			CVisualWord* pWord1 = (CVisualWord*)m_pWordsArr->GetAt(pWord->m_MainVerbs[l]);
			CSynHomonym* pHom1 = (CSynHomonym*)pWord1->m_arrHomonyms.GetAt(pWord1->m_iActiveHomonym);
			pHom1->m_bPredk = TRUE;
			for (size_t l1=0; l1<  pWord1->m_MainVerbs.size(); l1++)
			{
				CVisualWord* pWord2 = (CVisualWord*)m_pWordsArr->GetAt(pWord1->m_MainVerbs[l1]);
				CSynHomonym* pHom2 = (CSynHomonym*)pWord2->m_arrHomonyms.GetAt(pWord2->m_iActiveHomonym);
				pHom2->m_bPredk = TRUE;
			};
			
		};
	}

	for (size_t j=0; j < SynVar.m_Subjects.size(); j++)
	{
		pWord = (CVisualWord*)m_pWordsArr->GetAt(SynVar.m_Subjects[j] );
		CSynHomonym* pHom = (CSynHomonym*)pWord->m_arrHomonyms.GetAt(pWord->m_iActiveHomonym);
		pHom->m_bSubj = TRUE;
	};




	m_arrActiveGroups.clear();
	for(i = 0 ; i < SynVar.m_Groups.size() ; i++ )
	{
			m_arrActiveGroups.push_back(&(SynVar.m_Groups[i]) );
	}


	return TRUE;
}




bool PeriodsCompare_sort(const CVisualPeriod* pPeriod1, const CVisualPeriod* pPeriod2 )
{
	int iFirstWord1, iLastWord1,iFirstWord2, iLastWord2;

	if( pPeriod1->m_iFirstWord < pPeriod1->m_iLastWord )
	{
		iFirstWord1 = pPeriod1->m_iFirstWord;
		iLastWord1 =  pPeriod1->m_iLastWord;
	}
	else
	{
		iLastWord1 = pPeriod1->m_iFirstWord;
		iFirstWord1 = pPeriod1->m_iLastWord;		
	}

	if( pPeriod2->m_iFirstWord < pPeriod2->m_iLastWord )
	{
		iFirstWord2 = pPeriod2->m_iFirstWord;
		iLastWord2 =  pPeriod2->m_iLastWord;
	}
	else
	{
		iLastWord2 = pPeriod2->m_iFirstWord;
		iFirstWord2 = pPeriod2->m_iLastWord;		
	}

	if (	(iFirstWord1  == iFirstWord2)
		&&	(iLastWord1  == iLastWord2)
		)
	{
		return (long)pPeriod1 < (long)pPeriod2;
	}

	if( (iFirstWord1 >= iFirstWord2) &&
		((iLastWord1 <= iLastWord2) ) )
		return true;

	if( iLastWord1 <= iFirstWord2)
		return true;


	return false;
}






//!!! can be applied only to the sorted array of CVisualPeriod
int  AssignPeriodLevel(vector<CVisualGroup*>& pPeriodArr)
{
	CVisualPeriod* pPeriod;
	int iMaxGroupLevel = 1;

	for(int i = 0 ; i < pPeriodArr.size() ; i++)
	{
		pPeriod = pPeriodArr[i];
		int iFirstWord, iLastWord;
		if( pPeriod->m_iFirstWord < pPeriod->m_iLastWord )
		{
			iFirstWord = pPeriod->m_iFirstWord;
			iLastWord = pPeriod->m_iLastWord;
		}
		else
		{
			iLastWord = pPeriod->m_iFirstWord;
			iFirstWord = pPeriod->m_iLastWord;		
		}

		int iMaxLevel = 0 ;
		for(int j = i - 1 ; j >= 0 ; j-- )
		{
			CVisualPeriod* pPeriod1;
			int iFirstWord1, iLastWord1;
			pPeriod1 = pPeriodArr[j];

			if( pPeriod1->m_iFirstWord < pPeriod1->m_iLastWord )
			{
				iFirstWord1 = pPeriod1->m_iFirstWord;
				iLastWord1 = pPeriod1->m_iLastWord;
			}
			else
			{
				iLastWord1 = pPeriod1->m_iFirstWord;
				iFirstWord1 = pPeriod1->m_iLastWord;		
			}

			if( !( (iLastWord1 <=  iLastWord) && (iFirstWord1 >= iFirstWord) ) )
				break;
			if(iMaxLevel < pPeriod1->m_iLevel)
				iMaxLevel = pPeriod1->m_iLevel;
		}

		int iCoef = 1;


		if( iMaxLevel + iCoef > iMaxGroupLevel )
			iMaxGroupLevel = iMaxLevel + iCoef;

		pPeriod->m_iLevel = iMaxLevel + iCoef;
	}

	return iMaxGroupLevel;
}


BOOL CClauseType::Init(IClauseRootPtr piClauseType)
{
	try
	{
		m_arrHomNum.Add(piClauseType->RootHomonymNo);
		m_iWordNum = piClauseType->RootWordNo;
		_bstr_t bstrDesc = piClauseType->GetDescription();
		m_strName = CString((char*)bstrDesc);
	}
	catch(...)
	{
		return FALSE;
	}
	return TRUE;
}


