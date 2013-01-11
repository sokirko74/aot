// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#include "StdSynan.h"
#include "RusFormatCaller.h"



//������� ����������� ������
bool CRusFormatCaller::format_for_foreign_term(CGroup& G)
{
    if( W1.HasFlag(fl_ile) )
	 {
		   for (int j = G.m_iFirstWord; (j < sent.size()) &&  (sent[j].HasFlag(fl_ile) 
			   || (sent[j].HasFlag(fl_digit) && j>G.m_iFirstWord && is_english_upper(sent[j-1].get_word()[0]))); j++) //Corning ���������� Gorilla Glass 3 �� CES 2013
		   {
			   // ��������, ��� ����������� ������� ����� ����-��, ����� ���� �����
			   if (get_maximal_group(j).size() > 1) return false;
			   G.m_iLastWord = j;
		   }

		   if  (G.m_iLastWord == G.m_iFirstWord) return false;
		   G.SetGrammems( W1.GetGrammems() );
		   G.m_MainGroup.m_iFirstWord = G.m_iFirstWord;
		   G.m_MainGroup.m_iLastWord = G.m_iLastWord;
		   G.m_GroupType = FOREIGN_TERM;
		   G.m_Cause = "������� ��� ����������� ������.";
		   return true;
	 }
	else
      return false;
}
