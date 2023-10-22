#include "StdAfx.h"
#include "VisualSentences.h"
#include "VisualSynan.h"
#include "WaitThread.h"
#include <atlbase.h>
#include "synan/SynCommonLib/RelationsIterator.h"


CComModule _Module;



CVisualSentences::CVisualSentences()
{
	m_bLocked = FALSE;
}

CVisualSentences::~CVisualSentences()
{
	Reset();
}





BOOL CVisualSentences::FillSentencesArray(const CSentencesCollection& coll)
{
	try
	{
		for(const auto& s: coll.m_vectorSents)
		{			
			CVisualSentence * vs = new CVisualSentence;
			vs->Init(*s);
			m_arrSentences.Add(vs);
		}
	}
	catch(...)
	{
		return FALSE;
	}

	return TRUE;
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

		auto& piSentCollection = ((CVisualSynanApp*)::AfxGetApp())->m_SyntaxHolder.m_Synan;

		
		for(auto& piSent: piSentCollection.m_vectorSents)
		{
			
			report += "Sentence № ";
			report += "\n";
			
			CRelationsIterator iter;
			iter.SetSentence(piSent);
			for(int j = 0 ; j < piSent->GetClausesCount(); j++ )		
				iter.AddClauseNoAndVariantNo(j , 0);

			iter.BuildRelations();

			report += "Firm groups: " ;
			report += "\n";

			for(const CGroup& g: iter.GetFirmGroups())
			{
				CString str;
				str += FromRMLEncode(piSent->GetOpt()->GetGroupNameByIndex(g.m_GroupType));
				str += "(";
				for(int k = g.m_iFirstWord ; k <= g.m_iLastWord; k++ )
				{
					str = str + FromRMLEncode(piSent->GetWords()[k].m_strWord) + " ";
				}

				str += ")";
				report += str;
				report += "\n";

			}

			report += "Relations: " ;
			report += "\n";

			for(const CSynOutputRelation& piRel: iter.GetRelations())
			{
				CString str;
				str += FromRMLEncode(piSent->GetOpt()->GetGroupNameByIndex(piRel.m_Relation.type));
				str += "(";

				if( piRel.m_iSourceGroup == -1 )
				{
					int iSourceWord = piRel.m_Relation.m_iFirstWord;
					str = str + FromRMLEncode(piSent->GetWords()[iSourceWord].m_strWord);
				}
				else
				{
					const CGroup& piGroup = iter.GetFirmGroups()[piRel.m_iSourceGroup];
					for(int k = piGroup.m_iFirstWord; k <= piGroup.m_iLastWord ; k++ )
					{
						str = str + FromRMLEncode(piSent->GetWords()[k].m_strWord) + " ";
					}
				}

				str += " -> ";

				if (piRel.m_iTargetGroup == -1)
				{
					str = str + FromRMLEncode(piSent->GetWords()[piRel.m_Relation.m_iLastWord].m_strWord);
				}
				else
				{
					const CGroup& piGroup = iter.GetFirmGroups()[piRel.m_iTargetGroup];
					for (int k = piGroup.m_iFirstWord; k <= piGroup.m_iLastWord; k++)
					{
						str = str + FromRMLEncode(piSent->GetWords()[k].m_strWord) + " ";
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
		::AfxMessageBox(_T("Build-rels error!"));
	}

}



void CVisualSynVariant::InitClauseVariant(const CClause& piClause, int ClauseVariantNo)
{
	CSVI pSynVar   = piClause.GetSynVariantByNo(ClauseVariantNo);
	if (!piClause.IsValidCSVI(pSynVar))
		return;

	for(auto in_unit: pSynVar->m_SynUnits)
	{
		SUnit unit;

		unit.m_Type = in_unit.m_Type;
		if( unit.m_Type == EClause )
		{
			int iClauseNum = piClause.m_pSent->FindClauseIndexByPeriod(in_unit.m_SentPeriod);
			if (iClauseNum == -1) {
				continue;
			}
			unit.m_iClauseNum = iClauseNum;
			unit.m_iClauseTypeNum = in_unit.m_iClauseTypeNum;
		}
		else
		{
			for(const auto& prep_no: in_unit.m_SimplePrepNos)
			{
				CString str = "Ob: ";
				str += FromRMLEncode(piClause.GetOpt()->GetOborDic()->m_Entries[prep_no].m_OborotEntryStr);
				unit.m_strOborotsNum += str;					
			}
			
			std::string s = in_unit.GetGrammemsByAncodes();
			if (in_unit.m_Type == EWord)
			{
				auto& H = piClause.GetWords()[in_unit.m_SentPeriod.m_iFirstWord].m_Homonyms[in_unit.m_iHomonymNum];
				if (H.m_bPassive)
					s = "PASSIVE " + s;
				if (in_unit.m_bReflexive)
					s = "REFLEXIVE " + s;
			};
			unit.m_strGrammems = FromRMLEncode(s);
			unit.m_iHommonyNum = in_unit.m_iHomonymNum;
			unit.m_iWordNum = in_unit.m_SentPeriod.m_iFirstWord;
		}
		

		m_vectorUnits.push_back(unit);

	}

	m_iClauseTypeNum = pSynVar->m_ClauseTypeNo;


	for(const CGroup& g: pSynVar->m_vectorGroups.GetGroups())
	{
		CVisualGroup Group;
		Group.Init(piClause, g);
		m_Groups.push_back(Group);
	}		

	
	
	if (pSynVar->m_iPredk != -1)
	{
		//  convert predicate to sentence coordinates
		m_iPredk = pSynVar->GetSentenceCoordinates(pSynVar->m_iPredk).m_iFirstWord;
	}


	//  convert subjects to sentence coordinates
	m_Subjects.clear();
	for (auto subj: pSynVar->m_Subjects)
	{
		auto subj1 = pSynVar->GetSentenceCoordinates(subj).m_iFirstWord;
		m_Subjects.push_back(subj1);
	};

	m_Weight = pSynVar->m_iWeight;
}

