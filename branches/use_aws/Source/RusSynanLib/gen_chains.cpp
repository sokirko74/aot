// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#include "StdSynan.h"
#include "RusFormatCaller.h"


// �������:
// ��� ���� 
// ��� ������ ���������

bool CRusFormatCaller::format_for_gen_chains (CGroup& G)
{
	G.m_iLastWord = G.m_iFirstWord;
    const CGroup& LeftGroup = get_maximal_group (G.m_iFirstWord);
    int i = LeftGroup.m_MainWordNo;
	if ( !Wi.is_morph_noun()) return false;
	if (Wi.m_UnitType != EWord) return false;

	/*
		��� ������� ������������ � ������������ ����� ����� ������ �������� ������ 
		����-���, �������� "������ ����������",
		���� ��� ������� �� ������� ����� ��� �������� �����_��
		��� "������ ����������" (������������� �����)	����� �����_�� ��������.
		�������
	*/
	if  ( !Wi.has_grammem(rPlural))
		if (FindInList((const char*)g_BigNumerals, g_BigNumeralsCount, sent[i].get_lemma() ) != -1) 
			return false;

	// �� ������ "���" �����_�� �� ���������� 
	if (Wi.is_lemma("���"))
		return false;


	 // ���� ����� ������� ������� ����� ���������������, �� ����� 	 
	if (G.m_iFirstWord > 0)
	{ 
        const CGroup& H = get_maximal_group (G.m_iFirstWord - 1);
		if( H.m_GroupType != OBOROTS )
			if (sent[H.m_MainWordNo].is_morph_noun()) 
				return false;
	};

	if( LeftGroup.m_GroupType== NUMERALS )
		return false;

	if( LeftGroup.m_GroupType== OBOROTS )
		return false;

	int k = get_next_main_word(i);
	if (k == sent.size()) return false;

	// �������� �������, ���� ��� �����������
	if	(GetGramTab()->IsSimpleParticle(Wk.get_lemma(), Wk.GetPoses()) )
		k = get_next_main_word(k);
	if (k == sent.size()) return false;

	if (!Wk.is_morph_noun() || Wk.is_lemma("�������")) return false;
	const CGroup& RightGroup = get_maximal_group (k);
	if( RightGroup.m_GroupType == NUMERALS || RightGroup.m_GroupType == NUMERAL_NOUN)
			return false;
	if (!(RightGroup.GetGrammems() & _QM(rGenitiv))) return false;

	change_group_grammems((CGroup&)RightGroup, _QM(rGenitiv), rAllCases);
	sent[RightGroup.m_MainWordNo].SetGrammems(sent[RightGroup.m_MainWordNo].GetGrammems() & ~rAllCases | _QM(rGenitiv));
	//change_words_in_group_grammems((CGroup&)RightGroup, _QM(rGenitiv), rAllCases);
	if (Wk.HasFlag(fl_ambiguous)) return false;
	


	G.m_MainGroup = LeftGroup;
	G.m_iLastWord = RightGroup.m_iLastWord;
	G.m_GroupType = GEN_NOUN;
	G.SetGrammems( LeftGroup.GetGrammems() );


	return true;
	
};

/*
  ����� ����
  ����� ������ ��������
*/
bool CRusFormatCaller::format_for_from_compar (CGroup& G)
{

	try
	{
	 G.m_iLastWord = G.m_iFirstWord;
     const CGroup& P = get_maximal_group (G.m_iFirstWord);
     int i = P.m_MainWordNo;
     if ( !Wi.has_grammem(rComparative) ) return false;
     if ( !Wi.HasPOS(ADJ_FULL) ) return false;


	 

	 if (P.m_iLastWord + 1 >= sent.size()) return false;
     const CGroup& H = get_maximal_group (P.m_iLastWord + 1);
	 int j = H.m_MainWordNo;
	 if (!Wj.is_syn_noun() || Wj.is_lemma("�������")) return false;
	 
	 if( H.m_GroupType == NUMERALS)		 return false;
	 if (!(H.GetGrammems() & _QM(rGenitiv))) return false;

	 // ������ � ����. ���������������� ������� ��� ������, ����� ���. ��-�� ������� 
	 // "��� ������� ��������� ��� ������."
	 // !!!! change_words_in_group_grammems(H, (1<<rGenitiv), rAllCases);  

	 G.m_MainGroup = P;
	 G.m_iLastWord = H.m_iLastWord;
	 G.m_Cause = "������������� �������������� � ������ ����� �� ��� �� � ���. ������ ";
	 G.m_GroupType = FROM_COMPAR;
	 G.SetGrammems( 0 );

	}
	catch(...)
	{
		GetOpt()->OutputErrorString("Failed from-compar rule");	
	return false;
}


 return true;

};

