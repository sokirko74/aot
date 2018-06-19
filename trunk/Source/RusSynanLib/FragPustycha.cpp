// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko


#include "StdSynan.h"
#include "RusSentence.h"


bool CRusSentence::HasStrictestLeftClauseBorder(int ClauseNum) const
{
	int iWord = m_Clauses[ClauseNum].m_iFirstWord;

	if (IsStrictLeftClauseBorder(m_Clauses[ClauseNum])) return true;

   	/*
	 � ������ ���� "zzzz, � zzzz",�.�. ����� ���� ���. ���� � �������, �����
	 ������ ���������� �� ����, ���������  ������ ", � zzzz" ������ ����� ���������� ������,
	 � �� ����������� ��������� ������, � �� �����.
	 �������� ���������� ������������� ������ ���������� ����� (�� ��� � ������ ��������) ����:
			"� ����, � ����� ����."

	*/

		if(     ( GetWords()[iWord].m_strWord[0] ==  ',' )
			&&  (iWord  < GetWords().size() - 2 )
			&&  (   GetWords()[iWord+1].FindLemma("�") 
				 || GetWords()[iWord+1].FindLemma("���")  
				 || GetWords()[iWord+1].FindLemma("��")  
				 || GetWords()[iWord+1].FindLemma("�")  
				) 
		  )
		  return true;

	return false;
}


int HasSubConjWithInfinitiveGovernment(const CClause& C) 
{
	for(int i = 0; i < C.m_vectorConjs.size() ; i++ )
	{
		const SConjIndex& conj = C.m_vectorConjs[i];


		if(		(conj.m_FromWhere == FROM_OBOR_DIC)
			&&	(C.GetOpt()->GetOborDic()->m_Entries[conj.m_index].m_ConjType == sub_conj) 
			&&  (C.GetOpt()->GetOborDic()->m_Entries[conj.m_index].m_bConjWithInfinitiveSubord) 
		  )
			return i;
	}

	return -1;	
}


/*
��� ������� ���������� ������ �� RuleForEmptyClauses. ���������, ���
�������� ������ ����� ���� ������ ������, ������� ��������� ����� �� ���, �.�. �� ��� ����������� 
��������� ��������� ������������.  �������������� �������, ����������, ����� (��� ����) 
�.�. ��� ������� ����������, ��� �������� ������ �� ����� ���� �� ������ ������������
��������, � ������ ���� �������� � �������� ������, ���� �� �������� ��� �������
*/
bool CRusSentence::IsGoodEmptyClause(const CClause* pAbstClause) const 
{
	if (pAbstClause->m_vectorTypes.size() > 1)   return false;

	/*
	����� ������������� ������ ������,� ������� ���� ��� ���� ���, ���� �����
	*/
	if (pAbstClause->m_vectorTypes.size() == 1)   
		if (			(pAbstClause->m_vectorTypes[0].m_Type != INFINITIVE_T)
				&&		(pAbstClause->m_vectorTypes[0].m_Type != COMPARATIVE_T)
			)
			return false;


	/*
	 ���� � ������ ���� ����� "�����", ����� ����� ������� ��� ������ 
	 ��������:
	   "��� �������, � ����� ����� �����"
	*/
	for (int jj = pAbstClause->m_iFirstWord; jj < pAbstClause->m_iLastWord; jj++)
		if ( GetWords()[jj].FindLemma("�����") )
			return false;

	int iFirstWord = pAbstClause->m_iFirstWord;


    /*
	  ���� �� ������ ���������� �� ����� ����������, ����� ���� ���� ����������
	  ������ ���� ���� �������, ���� ����������, ���� ����:
	  ��������:
	    � ������ ���� ����������, � 60 �� ������-�������� ������, �� ������. 
		 (����� "� 60 �� ������-�������� ������" ���������� � �������)
		� ���� ���� �����: ���� � ����
		 ("���� � ����")
		�� ����� �� ������ ������ - �������� ������
		 ("- �������� ������")

	*/
	bool FirstWordIsComma =  GetWords()[iFirstWord].m_strWord[0] == ',';
    if(      GetWords()[iFirstWord].HasDes(OPun)
		&& ! (    FirstWordIsComma
		       || (GetWords()[iFirstWord].m_strWord[0] == ':') 
			   || (GetWords()[iFirstWord].m_strWord[0] == '-') 
		     ) 
	  )
	return false;

	/*
	 ���� ������ ���� ��� ���������� � ����. �����, ������� ����� ������� ���������,
	 �����  ����� �������, ��� ��� ������ ����������
	*/

	if (pAbstClause->HasType(INFINITIVE_T))
		if (-1 != HasSubConjWithInfinitiveGovernment(*pAbstClause) )
			return false;

   	/*
	 ���� � ������ ���� ����� �� ���� ������� �����������,
	 �����  ����� �������, ��� ��� ������ �� ����������
	*/
	if (pAbstClause->HasType(COMPARATIVE_T))
		if ( ClauseHasSubjPredik(*pAbstClause) )
			return false;

	
	return true;
}


/*
 ��� ������� ���������� ������ �� RuleForEmptyClauses. ���������, ����� �� 
 ������ ����� � �����������. ���� �� �����, ����� ���������� ������.
*/
bool CRusSentence::HasStrictBorderLeft(int iClauseNum)
{
	CClause* pAbstClause = &GetClause(iClauseNum);

	if( HasStrictestLeftClauseBorder(iClauseNum) )
		return true;

	if( iClauseNum == 0)
		return true;

	if( pAbstClause->HasLeftStarter() )
		return true;

	return false;
}

/*
 ��� ������� ���������� ������ �� RuleForEmptyClauses. ���������, ����� �� 
 ������� ������ �������� � ���� ����������� ������. ����  �����, ����� ���������� ������.
*/

bool CRusSentence::CanBeMainClauseOfTheLeftGoodEmptyClause(const CClause* pAbstClause1, bool bStrictPrevClauseBorder) const 
{
	bool bGoodClause;
	const string& strPunct = GetWords()[pAbstClause1->m_iFirstWord].m_strWord;
	bGoodClause =     
					  //  ���� � ���� ������ 
				      ( strPunct[0] != ',') 
					  // ����, ���� ������
		           || !pAbstClause1->HasOnlyOneSimConj()
					   // ����, � �� ���� ������ �����
				   ||  (   pAbstClause1->HasOnlyOneSimConjFromOborDic() 
					    && bStrictPrevClauseBorder 
						&& ( strPunct[0] == ',')
					   );

	/*
	 �� ����� ����������� ����� �������������, ���������� � ������� �������.
	*/
	bGoodClause &= !(    pAbstClause1->HasType(ADVERB_PARTICIPLE_T) 
		              || pAbstClause1->HasType(INP_T) 
					  || pAbstClause1->HasType(PARTICIPLE_T)
					);

	/*
	 ����  � ������ ���� ������. ����, ����� �� ������ ��������� �����
	*/
	bGoodClause &= !pAbstClause1->HasLeftStarter();


	return bGoodClause;
}

/*
  � ������ ����������� ������ ����������, � 60 �� ������-�������� ��������� ������, �� ��������� �� ����� �������������� � ������. 
  ����, ���� � ����

*/

bool CRusSentence::RuleForEmptyClauses(int iClauseNum)
{
	const CClause& Clause = GetClause(iClauseNum);
	int iWord = Clause.m_iFirstWord; 

	if( !IsGoodEmptyClause( &Clause ) ) return false;

	bool bStrictPrevClauseBorder = HasStrictBorderLeft(iClauseNum);	

	//����� ��� ��������, ������� ����� ������ �� ��������� �������	
	//  ���� � ������� ������ ���� ������� ������ ������, ������� ������������� ��� ��, ��� ������� ������
	// �����  �� ����� ��������� ������� ������� (��������������� ������ ),
	int iNext = GetMaxClauseByFirstWord(m_Clauses[iClauseNum].m_iLastWord+1);
	CClause* pAbstClauseRight = NULL;
	if( iNext != -1 )
		pAbstClauseRight = &GetClause(iNext);
	bool bCanConnectRight =		pAbstClauseRight 
							&& 	CanBeMainClauseOfTheLeftGoodEmptyClause(pAbstClauseRight, bStrictPrevClauseBorder)
							&&  (GetMinClauseByLastWord(Clause.m_iLastWord) == iClauseNum)
							&& !HasStrictBorderLeft(iNext);

	//����� ��� ��������, ������� ����� ����� �� ��������� �������
	CClause* pAbstClauseLeft = NULL;
	int iPrev = GetMinClauseByLastWord(Clause.m_iFirstWord-1);
	if( iPrev != -1 ) 
		pAbstClauseLeft = &GetClause(iPrev);

	//  ���� �������� ������ ���� ������� � ������ ������, �����  �� ����� ��������� ������� ������,
	//  ���������  �������  ����� ����� ��� ������ (� ������� � � �������"),
	//   ��������:
	//  "��������, �������������� � �������, 12 ������" // "12 ������"  ���� ���������� � ����������  ������
	//  "���� ����, �������  ��� �������� ������, 12 ������" // "12 ������"  ���� ���������� � ������� ������
	// �� ����� �����  ���������� ��������������
	bool bCanConnectLeft =		pAbstClauseLeft 
							&&  !pAbstClauseLeft->HasType(INP_T)
							&&  !Clause.HasLeftStarter()
							&&  !IsEnclosedClause(iPrev)
							&&  !HasStrictestLeftClauseBorder(iClauseNum);


	// ���������� ����� ����� ������� ����� ������������
	if(bCanConnectLeft)
	{
		/* ���� ������������ �������  � �����������,
		   �����  ��� ������������ ������ ������ ���� ����������� 
		   ���� ������������ �������  �� �������,
		   �����  ��� ������������ ������ ������ ���� ����� 
		*/
		if  ( (    Clause.HasType(INFINITIVE_T) 
			       || Clause.HasType(COMPARATIVE_T) 
			      ) 
				&& pAbstClauseLeft->m_vectorTypes.empty() 
				&& 1 == pAbstClauseLeft->m_vectorTypes.size() 
			)
			UniteClauses(iPrev, iClauseNum, RightClauseParams);
		else
			UniteClauses(iPrev, iClauseNum, LeftClauseParams);			

		return true;	
	}

	if(bCanConnectRight) 
	{
		UniteClauses(iClauseNum, iNext, RightClauseParams);
		return  true;
	} 



	return false;
}

