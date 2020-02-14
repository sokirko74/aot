// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko


#include "StdSynan.h"
#include "RusWord.h"
#include "RusSentence.h"
#include "RusFormatCaller.h"

//  ���������, ��� ������� ������ iClauseNum1 � ������� iClauseNum2 �����������  ��� ������ ���������� ����� 
bool CRusSentence::CheckCoordinarionBetweenClauseRoots(GrammemCompare CompareFunc, int iClauseNum1, int iClauseNum2 ) const
{
	
	const CClause& C1 = GetClause(iClauseNum1);
	const CClause& C2 = GetClause(iClauseNum2);


	for (int i = 0; i < C1.m_vectorTypes.size(); i++)
		for (int k = 0; k < C2.m_vectorTypes.size(); k++)
			if (		!C1.m_vectorTypes[i].m_Root.IsEmpty()
					&&	!C2.m_vectorTypes[k].m_Root.IsEmpty()
				)
			{
				const CSynHomonym& H1 = m_Words[C1.m_vectorTypes[i].m_Root.m_WordNo].m_Homonyms[C1.m_vectorTypes[i].m_Root.m_HomonymNo];
				const CSynHomonym& H2 = m_Words[C2.m_vectorTypes[k].m_Root.m_WordNo].m_Homonyms[C2.m_vectorTypes[k].m_Root.m_HomonymNo];
                if (GetOpt()->GetGramTab()->Gleiche(CompareFunc, H1.GetGramCodes().c_str(), H2.GetGramCodes().c_str()))
                    return true;
			};
	return false;

};

bool CRusSentence::RuleForSubClauseWithCoordiantion(int iClauseNum)
{
	const CClause* pAbstClause = &GetClause(iClauseNum);
	int iWord = pAbstClause->m_iFirstWord;

	if(		pAbstClause->HasType(INP_T)  
		||  (pAbstClause->m_vectorTypes.size() == 0)
		)
		return false;


	//  ����� ��������� ������, ������� ������ ���� � ������������� ������
	int iNext = GetMaxClauseByFirstWord(m_Clauses[iClauseNum].m_iLastWord+1);
	if(iNext == -1)
		return false;


	const CClause* pAbstClause1 = &GetClause(iNext);



    int type = pAbstClause1->FindCommonType(*pAbstClause);
    switch (type)
    {
        case INP_T  :

        case -1     :       return false;

        case PARTICIPLE_T:
                            if	(!CheckCoordinarionBetweenClauseRoots(GenderNumberCaseRussian, iClauseNum, iNext))
                                return false;
                            break;

        case ADVERB_PARTICIPLE_T:  break;// do noting

        default:
      	            if  (        ClauseHasSubjPredik(GetClause(iNext)) // ���� ������  ������  ����� ����������, �����  ������������ ������ �� ����
                            ||  !CheckCoordinarionBetweenClauseRoots(FiniteFormCoordRussian, iClauseNum, iNext)
                        )
                         //  �������� �� ��������������� �������
                            if(GetMinClauseByLastWord(m_Clauses[iClauseNum].m_iLastWord) !=  GetMaxClauseByLastWord(m_Clauses[iClauseNum].m_iLastWord))
                                return false;
                    /*
                        ����  �� �����������  ������  ������� ����������, �� �� ������ ������ ���������:
                        1. ���� ������� ��� ����e�������� �������, ������� ������ ��e������, ���������  ������� �� ����� ���� �������;
                        2. ���� �� ������������ ��� ������ ������, ���������  ����� � �������������� ��������� ����� ���-������ �������
                        ���������
                        �� ������������ ������ �������, ���� ��� ���������� ������������� ������� ����������� �������, ��� �� ����� 
                        �����������. �����-������ ����� ����� ��������. ������� 31.10.02
                    */
                    if (!pAbstClause1->HasUnambiguousStrongRootWithoutWeakHomonyms())
                        return false;
                    break;
    }
	
	if( pAbstClause1->size() < 2 )
		return false;

	int iFirstWord = pAbstClause1->m_iFirstWord;
	bool bCoordConj =  
								(		(GetWords()[iFirstWord].m_strWord[0] == ',') 
									&&	m_Words[iFirstWord+1].m_bSimilarConj
								)
						||		m_Words[iFirstWord].m_bSimilarConj;
					
	if (!bCoordConj) 
		return false;

	return EncloseClauseAsWord(iNext, iClauseNum);
}

