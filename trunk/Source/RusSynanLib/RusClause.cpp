// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko


#include "StdSynan.h"
#include "RusFormatCaller.h"
#include "RusClause.h"
#include "RusSentence.h"


/*
	��������  ��� ����. �������� �� �������� vectorTypes �� ��������� �������������
	1. ������� ������� �� ����, ��� ������� ���������� ���������� ����������� �������;
	(if( node != -1 ))
	2. ����� ��� "����"
	3. ����� �������, ������� �������������� �����. 
*/



/*
	�������  ������� ��� ������� ���������, ���� ��� ��������� ����� � ������ ��������� ������
	�������  �� ������ ����� ��� �����, ������� ��� ���� ������� ��� ��������� �������, � �� ���������� 
	��������: ������� ���������� ��� ���������� ������, � ����� ������� ������������ ���������� ���.
*/
void ChangeClauseTypeForParticiples(CClause& C)
{
	int ClauseNum = C.m_pSent->FindClauseIndexByPeriod(C);
	if (ClauseNum == -1) return;
	if (C.m_pSent->IsEnclosedClause(ClauseNum)) return;

	if (!C.HasType(PARTICIPLE_T)) return;
	int j = C.FindType(PARTICIPLE_T);
	int WordNo = C.m_vectorTypes[j].m_Root.m_WordNo;
	const CGroup* G = C.GetMaxLastHost(WordNo, C.m_vectorTypes[j].m_Root.m_HomonymNo);
	if (!G) return;

	int UnitNo = C.UnitNoByWordNo(WordNo);
	assert (UnitNo != -1);
	if (G->m_MainWordNo != WordNo)
		C.DeleteClauseType( j );
}



/*
����� ������  ������������ ������� WordNoByUnitNo, ���������  ����������� ��� �� ���������
*/
int CRusSentence::GetCountOfStrongRoots(const CClause& C, const CMorphVariant& synVar) const
{
	int iHypPredk = 0;
	
	
	for (int i = 0; i < synVar.GetUnitsCount(); i++)
	{
		if ( synVar.GetUnitType(i) == EClause ) 
		{
			continue;
		};

		int iWord = synVar.m_SynUnits[i].m_SentPeriod.m_iFirstWord;
			
		const CSynHomonym& Hom = C.GetWords()[iWord].GetSynHomonym(synVar.GetHomNum(i));

		//�������� ����� ����, ������� ����� ��������� ��������
		//������ "������" � ���������� ����� ������ � ����� ������ � ������ ����������
		//"������� ����� �������"
		//������� "�����������", "���������" ���� ���������� ������������� ����
		if ( C.GetOpt()->GetGramTab()->IsStrongClauseRoot( Hom.m_iPoses ) )

			if (!Hom.IsLemma("������")&&
				!Hom.IsLemma("�����������") &&
				!Hom.IsLemma("���������")&&
				!Hom.IsLemma("�����") &&
				!Hom.IsLemma("�����") &&
				!Hom.IsLemma("����"))
		{
				if ( C.m_pSent->FindFirstAuxVerb(iWord) == -1 )
					iHypPredk++;
		}
	}

	return iHypPredk;
};


void CRusSentence::AfterBuildGroupsTrigger(CClause& C)
{
	ChangeClauseTypeForParticiples(C);
};





