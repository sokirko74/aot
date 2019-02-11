// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#include "StdSynan.h"
#include "RusFormatCaller.h"


struct CSelectiveWord
{
	const char* m_Lemma;
	QWORD		m_Numder;		
	CSelectiveWord (const char* Lemma, QWORD Numder)
	{
		m_Lemma = Lemma;
		m_Numder = Numder;

	};
};

const int g_iSelectiveWordsCount = 6;
const CSelectiveWord g_strSelectiveWords[g_iSelectiveWordsCount] =
{
	CSelectiveWord("�����", rAllNumbers ),
	CSelectiveWord("������",(1<<rPlural) ),
	CSelectiveWord("���������", (1<<rPlural)),  // ?*"���������  �� ���"
	CSelectiveWord("������",(1<<rSingular)),  // "???������ �� ���"
	CSelectiveWord("����", rAllNumbers),   
	CSelectiveWord("�����", rAllNumbers)   
};

/*
 ��������:
  ������ �� ���� �����
  ����� �� ������� ������
  ������ �� ��� 
  ����� �� ��� �� ������� 
  ����� �������� �� ���
  ����������� �� ��� 
*/
bool CRusFormatCaller::format_for_selective_groups(CGroup& G)
{
	const CSynPlmLine& first_word_plm = sent[G.m_iFirstWord];
	size_t next_word;	

	if (first_word_plm.m_UnitType == EClause) return false;
    int gr_num = get_maximal_group_no(G.m_iFirstWord);
	if( gr_num == -1 )
	{		
		
		int i = 0;
		for( ; i < g_iSelectiveWordsCount ; i++ )
			if( first_word_plm.is_lemma(g_strSelectiveWords[i].m_Lemma) )
				break;

		//  ���� ������� � ����� ������ �� �������
		if( i < g_iSelectiveWordsCount)
		{	
			/*
			 ��������� ����� 
			*/
			if ( (first_word_plm.GetGrammems() & g_strSelectiveWords[i].m_Numder) == 0)
				return false;

			next_word = G.m_iFirstWord + 1;
		}
		else
			/*
			 ���� ��� ��������������, ������� ������������� �� "���" 
			 (������������� ������������ �������)
			*/
        if (first_word_plm.HasPOS(ADJ_FULL) )
		{
				const char* lemma = first_word_plm.get_lemma();
				if (first_word_plm.get_lemma() == 0) return false;
				if( strlen(lemma) < 4)
					return false;
				lemma = lemma + ( strlen(lemma) - 3);
				if( strcmp(lemma,"���") )
					return false;
				next_word = G.m_iFirstWord + 1;
		}
		else
			/*
			 ���� ��� ���������� ����������� 
			*/
		if  (first_word_plm.HasPOS(NUMERAL_P) || (first_word_plm.HasPOS(NUMERAL) ) )//42 �� �������� ������ �����
		{
	  		next_word = G.m_iFirstWord + 1;
		}
		else
			// "����  �� ���"
			if (CanNumeralBeNoun (first_word_plm.get_lemma()))
				{
	  				next_word = G.m_iFirstWord + 1;
				}
			else
				next_word = sent.size();
	}
	 /*
	  ���� ��� ������ "����� �������� �� ���"
	 */ 
	else
	{
		if( GetGroups()[gr_num].m_GroupType != MODIF_ADJ )
			return false;

		if( !first_word_plm.is_lemma("�����") )
			return false;
		next_word = GetGroups()[gr_num].m_iLastWord + 1;
	}

	if( next_word >= sent.size() )
			return false;

	/*
	 ������ ������ ���� ���������� ������ � ��������� ��+�
	*/
	int next_gr_num = get_maximal_group_no(next_word);
	if( next_gr_num == -1 )
		return false;

	
	const CGroup& prep_gr = GetGroups()[next_gr_num];
	if(		(prep_gr.m_GroupType != PREP_NOUN)
		||	!sent[prep_gr.m_iFirstWord].is_lemma("��")
	  )
		return false;

	G.m_iLastWord = prep_gr.m_iLastWord;
	G.m_GroupType = SELECTIVE_GR;
	//��������� ��� � ������� ������ ��������
	QWORD LWGen = sent[G.m_iLastWord].GetGrammems() & rAllGenders;
	if( (sent[G.m_iFirstWord].GetGrammems() & rAllGenders)>0 && LWGen>0 &&
		((sent[G.m_iFirstWord].GetGrammems() & rAllGenders & LWGen) == 0) )
		return false;
	if( CountBits(LWGen)==1 )
		sent[G.m_iFirstWord].SetGrammems(sent[G.m_iFirstWord].GetGrammems() & ~rAllGenders | LWGen);

	G.SetGrammems( sent[G.m_iFirstWord].GetGrammems() );
	G.m_MainGroup = get_maximal_group(G.m_iFirstWord);
	create_syn_rel(G, get_main_word(G.m_iFirstWord) , get_main_word(prep_gr.m_iFirstWord), SELECTIVE_GR);
	return true;
}
