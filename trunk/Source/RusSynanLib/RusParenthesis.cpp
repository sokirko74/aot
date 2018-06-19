// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko


#include "StdSynan.h"
#include "RusSentence.h"
#include "RusFormatCaller.h"
#include "RusWord.h"


bool  CRusSentence::CanBeInsideParenthesisOborot(const CSynWord& W) const
{
    for (size_t i=0; i < W.GetHomonymsCount(); i++)
    {
        const  CHomonym* pH = W.GetHomonym(i);
        // ������ ����� ��� ������� �������
        if ( pH->HasPos(CONJ) || GetOpt()->GetGramTab()->IsSimpleParticle(pH->m_strLemma.c_str(), pH->m_iPoses))
            return true;
    }
    return false;
}

bool  CRusSentence::CheckBorders (int start, int end) const
{
    for ( int j = start-1; j >= 0 && !m_Words[j].HasDes(OPun); j--)
        if ( !CanBeInsideParenthesisOborot(m_Words[j] ))
            return false;

    for ( int j = end+1; j < m_Words.size() && !m_Words[j].HasDes(OPun); j++)
        if ( !CanBeInsideParenthesisOborot(m_Words[j] ))
            return false;

    return true;

};

// ���������� ������ ��������, �� ������� �� ����� ���� ������� ��������, ��������� ��� �� �������� �� ����������� ����� ������� �����������
vector<CPeriod> CRusSentence::FindParenthesisOborotsWithoutComma() const
{
	vector<CPeriod> OborotsToDel;
    for (int i = 0; i < m_Words.size(); i++)
    {
        for (size_t HomNo=0; HomNo< m_Words[i].GetHomonymsCount(); HomNo++)
        {
            const CSynHomonym* pH = &m_Words[i].GetSynHomonym(HomNo);
            if ( pH->m_bOborot1 &&  pH->GetOborotPtr()->HasPartOfSpeech(INP) )
            {
                for (int j=i; j < m_Words.size(); j++)
                    if (m_Words[j].HasOborot2())
                    {
                        if ( !CheckBorders (i, j) )
                            OborotsToDel.push_back(CPeriod(i, j));
                        break;
                    }
                
                break;
            }
        }

    }
    return OborotsToDel;
}

void CRusSentence::DetermineParenthesis()
{

	vector<CPeriod> v_oborots_del = FindParenthesisOborotsWithoutComma();
	
    for (int i = 0; i < v_oborots_del.size(); i++)
    {
        int FirstWordNo = v_oborots_del[i].m_iFirstWord;
        CSynWord& FirstWord = m_Words[FirstWordNo];

        if ( v_oborots_del[i].size() == 1 ) // �����������
        {
            for (int HomNo = FirstWord.GetHomonymsCount()-1; HomNo >=0; HomNo--)
            {
                //example : ��� �������.
                if ( FirstWord.GetHomonymsCount() > 1 )
                    if ( FirstWord.GetHomonym(HomNo)->HasPos(INP) ) 
                        FirstWord.EraseHomonym(HomNo);			

            }
        }
        else
        {
            if (!FirstWord.GetSynHomonym(0).GetOborotPtr()->HasOnlyPartOfSpeech(INP) )
            {
                // ��������,  ��� ������ ������ �� ����� ����� �������� �������� "�����", 
                // ��������� ����������, ��� ������ ����� ������ ������ � ������ ������� � 
                // � �� ��������� ������� ����������, ������ �� �� ����� ���� �������� ������, � ������ �� ����� ���� ������� ��� �����.

                // �� �� ����� �������, ��������� ��� �� ����� ������� �������
                FirstWord.m_bBadParenthesis = true;
            }
            else
                DeleteOborotThatContains(FirstWordNo);
            
        }
    }
}

