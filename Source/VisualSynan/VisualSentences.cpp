#include "stdafx.h"
#include "VisualSentences.h"
#include "VisualSynan.h"
#include "WaitThread.h"
#include <atlbase.h>

CComModule _Module;



CVisualSentences::CVisualSentences()
{
	m_bLocked = FALSE;
}

CVisualSentences::~CVisualSentences()
{
	Reset();
}





BOOL CVisualSentences::FillSentencesArray(ISentencesCollectionPtr& piSentCollection)
{
	BOOL bRes = TRUE;
	try
	{
		
		CVisualSentence* pVisualSentence;
		BOOL bRes = TRUE;		
		int iSentCount = piSentCollection->GetSentencesCount();

		for(int i = 0 ; i < iSentCount ; i++ )
		{			
			ISentencePtr piSentence;
			piSentence = piSentCollection->GetSentence(i);
			pVisualSentence = new CVisualSentence;
			bRes = pVisualSentence->Init(piSentence);
			m_arrSentences.Add(pVisualSentence);
		}
	}
	catch(...)
	{
		return FALSE;
	}

return bRes;
}


int CVisualSentences::SentCount()
{
	return m_arrSentences.GetSize();
}



BOOL CVisualSentences::CalculateCoordinates(CDC* pDC,int iWidth,BOOL bShowGroups)
{
	try
	{
		if( m_bLocked )
			return TRUE; 

		CVisualSentence* pVisualSentence;
		int iHeight = 0;

		for(int i = 0 ; i < m_arrSentences.GetSize() ; i++)
		{
			pVisualSentence = (CVisualSentence*)(m_arrSentences.GetAt(i));
			iHeight = pVisualSentence->CalculateCoordinates(pDC, iHeight, iWidth, bShowGroups);
		}
	}
	catch(...)
	{
		return FALSE;
	}
	return TRUE;	
}

int CVisualSentences::PrintSentencesOnPrn(CDC* pDC, int iSent, int& iLine, int iBottom,int& iOffset)
{
	m_bLocked = TRUE;

	ASSERT( (iSent >= 0) && (iLine >= 0) );
	CVisualSentence* pVisualSentence;
	if( iSent || iLine)
	{
		if( iLine > 0)
		{
			pVisualSentence = (CVisualSentence*)(m_arrSentences.GetAt(iSent));
			iBottom += pVisualSentence->GetLineBottom( iLine - 1 );
		}
		else if( (iSent > 0) && ( iLine == 0) )
		{
			pVisualSentence = (CVisualSentence*)(m_arrSentences.GetAt(iSent));
			iBottom += pVisualSentence->GetLineBottom(0);			
		}

	}

	for(; iSent < m_arrSentences.GetSize() ; iSent++)
	{
		pVisualSentence = (CVisualSentence*)(m_arrSentences.GetAt(iSent));
		if( !pVisualSentence->PrintAllLinesTillBottom(iLine, pDC, iBottom, iOffset) )
			break;
		iLine = 0;
	}

	m_bLocked = FALSE;
	if( iSent == m_arrSentences.GetSize() ) 	
		return -1;


	if( iLine == 0 )
		iOffset = pVisualSentence->GetLineBottom(pVisualSentence->GetLinesCount() - 1);	
	else
		iOffset = pVisualSentence->GetLineBottom( iLine - 1 );


	return iSent;
}


BOOL CVisualSentences::PrintSentences(CDC* pDC,CRect& rectForDrawing, int iOffset)
{
	try
	{
		CVisualSentence* pVisualSentence;
		int iHeight = 0, iLine = 0;

		int iSent = 0;
		for( ; iSent < m_arrSentences.GetSize() ; iSent++)
		{
			pVisualSentence = (CVisualSentence*)(m_arrSentences.GetAt(iSent));
			if( (iLine = pVisualSentence->InRect(rectForDrawing) ) != -1 )
				break;			
		}

		for(; iSent < m_arrSentences.GetSize() ; iSent++)
		{
			pVisualSentence = (CVisualSentence*)(m_arrSentences.GetAt(iSent));
			if( !pVisualSentence->PrintAllLinesInRect(iLine, pDC, rectForDrawing, iOffset) )
				break;
			iLine = 0;
		}

	}
	catch(...)
	{
		return FALSE;
	}

	return TRUE;	
}


BOOL CVisualSentences::GetHomonymsArray(CPoint& point,CPtrArray** pHomonymsArray, int* iActiveSentence,int* iActiveWord)
{
	CVisualSentence* pVisualSentence;

	for(int i = 0 ; i < m_arrSentences.GetSize() ; i++)
	{
		pVisualSentence = (CVisualSentence*)(m_arrSentences.GetAt(i));
		if(pVisualSentence->PointInSentRect(point))
		{
			if (pVisualSentence->GetHomonymsArray(point,pHomonymsArray, iActiveWord))
			{
				*iActiveSentence = i;
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CVisualSentences::SetActiveHomonym(int iActiveSentence,int iActiveWord, int iActiveHomonim)
{
	if( (iActiveSentence < 0) || (iActiveSentence >= m_arrSentences.GetSize()) )
		return FALSE;

	return ((CVisualSentence*)(m_arrSentences.GetAt(iActiveSentence)))->SetActiveHomonym(iActiveWord, iActiveHomonim);
return TRUE;
}

CSize CVisualSentences::GetSentencesSize()
{
	CSize size(0,0);
	if(m_arrSentences.GetSize())
	{
		size.cx = ((CVisualSentence*)(m_arrSentences.GetAt(m_arrSentences.GetSize() - 1)))->m_SentRect.right;
		size.cy = ((CVisualSentence*)(m_arrSentences.GetAt(m_arrSentences.GetSize() - 1)))->m_SentRect.bottom;
	}
	return size;
}

BOOL CVisualSentences::GetActiveHomDescr(int iActiveSentence,int iActiveWord,CString& strLemma,CString& strGramChar)
{
	if( (iActiveSentence < 0) || (iActiveSentence >= m_arrSentences.GetSize()) )
		return FALSE;

	return ((CVisualSentence*)(m_arrSentences.GetAt(iActiveSentence)))->GetActiveHomDescr(iActiveWord, strLemma, strGramChar);	
}



void CVisualSentences::Reset()
{
	CVisualSentence* pVisualSentence;

	for(int i = 0 ; i < m_arrSentences.GetSize() ; i++ )
	{
		pVisualSentence = (CVisualSentence*)(m_arrSentences.GetAt(i));
		delete pVisualSentence;
	}

	m_arrSentences.RemoveAll();
	
}

void CVisualSentences::BuildRels(CString& report)
{
	try
	{

		ISentencesCollectionPtr piSentCollection = ((CVisualSynanApp*)::AfxGetApp())->m_SyntaxHolder.m_piSentCollection;

		
		for(int i = 0 ; i < piSentCollection->SentencesCount ; i++ )
		{
			
			//file << "Sentence ¹ " << i << endl;
			report += "Sentence ¹ ";
			CString ss;
			ss.Format("%d ", i);
			report += ss;
			report += "\n";
			
			IRelationsIteratorPtr piRelIt;
			ISentencePtr piSent = piSentCollection->Sentence[i];

			for(int l = 0 ; l < piSent->GetPrimitiveClausesCount() ; l++ )
			{
				int i1 = piSent->GetPrimitiveClause(l)->GetFirstWord();
				int i2 = piSent->GetPrimitiveClause(l)->GetLastWord();
			}


			piRelIt = piSent->CreateRelationsIterator();
			for(int j = 0 ; j < piSent->ClausesCount ; j++ )		
				piRelIt->AddClauseNoAndVariantNo(j , 0);

			piRelIt->BuildRelations();

			report += "Firm groups: " ;
			report += "\n";

			for(int j = 0 ; j < piRelIt->FirmGroupsCount ; j++ )
			{
				CString str;
				IGroupPtr& piGroup = piRelIt->GetFirmGroup(j);
				str += (const char*)piGroup->TypeStr;
				str += "(";
				for(int k = piGroup->FirstWord ; k <= piGroup->LastWord ; k++ )
				{
					str = str + (const char*)piSent->GetWord(k)->GetWordStr() + " ";					
				}

				str += ")";
				report += str;
				report += "\n";

			}

			report += "Relations: " ;
			report += "\n";

			for(int j = 0 ; j < piRelIt->RelationsCount ; j++ )
			{
				CString str;
				IRelationPtr piRel;
				piRel = piRelIt->Relation[j];
				str += (const char*)piRel->GetName();
				str += "(";

				if( piRel->SourceItemType == EWord )
				{
					int iSourceWord = piRel->GetSourceItemNo();
					int iClause = piSent->GetWord(iSourceWord)->GetClauseNo();
					str = str + (const char*)piSent->GetWord(iSourceWord)->GetWordStr();					
				}
				else
				if( piRel->SourceItemType == EGroup )
				{
					int iSourceGroup =  piRel->GetSourceItemNo();
					IGroupPtr piGroup = piRelIt->GetFirmGroup(iSourceGroup);
					for(int k = piGroup->FirstWord ; k <= piGroup->LastWord ; k++ )
					{
						str = str + (const char*)piSent->GetWord(k)->GetWordStr() + " ";	
					}
				}

				str += " -> ";

				if( piRel->TargetItemType == EWord )
				{
					int iTargetWord = piRel->GetTargetItemNo();
					int iClause = piSent->GetWord(iTargetWord)->GetClauseNo();
					str = str + (const char*)piSent->GetWord(iTargetWord)->GetWordStr();
				}
				else
				if( piRel->TargetItemType == EGroup )
				{
					int iTargetGroup =  piRel->GetTargetItemNo();
					IGroupPtr piGroup = piRelIt->GetFirmGroup(iTargetGroup);
					for(int k = piGroup->FirstWord ; k <= piGroup->LastWord ; k++ )
					{
						str = str + (const char*)piSent->GetWord(k)->GetWordStr() + " ";	
					}			
				}
				str = str + ")";

				report += "  ";
				report += str;
				report += "\n";
			}		
		}

	}
	catch(...)
	{
		::AfxMessageBox("Build-rels error!");
	}

}



void CVisualSynVariant::InitClauseVariant(IClausePtr& piClause, int ClauseVariantNo)
{
	IClauseVariantPtr& piClauseVariant  = piClause->GetClauseVariant(ClauseVariantNo);
	for(int i = 0 ; i < piClauseVariant->GetUnitsCount() ; i++ )
	{
		SUnit unit;

		unit.m_Type = (EUnitType)piClauseVariant->GetUnit(i)->GetType();
		if( unit.m_Type == EClause )
		{
			unit.m_iClauseNum = piClauseVariant->GetUnit(i)->GetClauseNum();
			unit.m_iClauseTypeNum = piClauseVariant->GetUnit(i)->GetActiveClauseTypeNum();
		}
		else
		{
			for(int k = 0 ; k < piClauseVariant->GetUnit(i)->GetSimplePrepsCount(); k++ )
			{
				CString str = "Ob:";
				str.Format(" %s", (const char*)piClauseVariant->GetUnit(i)->GetSimplePrepStr(k));
				unit.m_strOborotsNum += str;					
			}
			
			unit.m_strGrammems = (const char*)piClauseVariant->GetUnit(i)->GetGrammemsStr();
			unit.m_iHommonyNum = piClauseVariant->GetUnit(i)->GetActiveHomonymNum();
			unit.m_iWordNum = piClauseVariant->GetUnit(i)->GetWordNum();
		}
		

		m_vectorUnits.push_back(unit);

	}

	m_iClauseTypeNum = piClauseVariant->ClauseRootNo;


	for(int i = 0 ; i < piClauseVariant->GetGroupsCount() ; i++ )
	{
		CVisualGroup Group;
		Group.Init(piClauseVariant->GetGroup(i));
		m_Groups.push_back(Group);
	}		

	
	
	if (piClauseVariant->Predicate != -1)
	{
		//  convert predicate to sentence coordinates
		int predk  = piClauseVariant->Predicate;
		ASSERT(predk < piClauseVariant->GetUnitsCount());
		ASSERT(piClauseVariant->GetUnit(predk)->GetType() == EWord);
		m_iPredk = piClauseVariant->GetUnit(predk)->GetWordNum();
	}


	//  convert subjects to sentence coordinates
	m_Subjects.clear();
	for (size_t i=0; i <  piClauseVariant->SubjectsCount; i++)
	{
		int subj = piClauseVariant->Subjects[i];
		ASSERT(piClauseVariant->GetUnit(subj)->GetType() == EWord);
		ASSERT(subj < piClauseVariant->GetUnitsCount());
		m_Subjects.push_back(piClauseVariant->GetUnit(subj)->GetWordNum());
	};

	m_Weight = piClauseVariant->VariantWeight;
}

