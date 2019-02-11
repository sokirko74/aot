// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Posmorphological Module (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Alexey Sokirko (1999-2002)

#include "MAPostMain.h"
#include "../common/PlmLine.h"
#include "../common/utilit.h"
#include "../common/rus_numerals.h"
#include "../TrigramLib/TrigramModel.h"
#include "../AgramtabLib/RusGramTab.h"

void CMAPost::log(string s)
{
	if (m_LogFileName == "")  return;
	try {
		FILE* fp = fopen(m_LogFileName.c_str(), "a");
		if (!fp)  
		{ 
			m_LogFileName = "";
			return;
		};
		fprintf (fp, "%s\n", s.c_str());
		fclose(fp);
	}
	catch (...) {
	};

}


void CMAPost::RunRules()
{
	try
	{
            Rule_TwoPredicates();

            if (m_bUseTrigrams)
               FilterPostMorphWords();

			log("Rule_FilterProperName");
			Rule_FilterProperName();

			log("Rule_AdverbFromAdjectives");
			Rule_AdverbFromAdjectives(); 

			log("Odnobuk");
			Odnobuk();


				if (m_bCanChangeInputText)
			{
				log("Cifrdef");
				Cifrdef();

				log("ILeDefLe");
				ILeDefLe();	

			}

			log("ParticipleAndVerbInOneForm");
			ParticipleAndVerbInOneForm();	

			log("PronounP_Pronoun_Homonymy");
			PronounP_Pronoun_Homonymy();


			if (m_bCanChangeInputText)
			{
				log("FixedCollocations");
				FixedCollocations();
			};

			log("CorrectOborots");
			CorrectOborots();

			log("SemiAdjectives");
			SemiAdjectives();

			log("Interjections");
			Rule_Interjections();
			
			log("SemiNouns");
			SemiNouns();

			log("Rule_UZHE");
			Rule_UZHE();

			log("Rule_Ideclinable");
			Rule_Ideclinable();

			log("Rule_DeadPlurals");
			Rule_DeadPlurals();
			
			log("Rule_RelationalAdjective");
			Rule_RelationalAdjective();

			log("Rule_Fio");
			Rule_Fio();


			if (m_bCanChangeInputText)
			{
				log("Rule_QuoteMarks");
				Rule_QuoteMarks();
			};

			log("Rule_ILE");
			Rule_ILE();

			if (m_bCanChangeInputText)
			{
				log("Rule_KAK_MOZHNO");
				Rule_KAK_MOZHNO();

				log("Rule_Redublication");
				Rule_Redublication();

				log("Rule_CHTO_ZA");
				Rule_CHTO_ZA();

				log("Rule_VOT_UZHE");
				Rule_VOT_UZHE();

			};
			
			log("Rule_UnknownNames");
			Rule_UnknownNames();

			log("Rule_SOROK");
			Rule_SOROK();

			Rule_Abbreviation();

			Rule_ChangePatronymicLemmas();

			OtherRules();
	}
	catch(...)
	{
		ErrorMessage("Mapost", "Mapost has crushed!");
		return;
	}
}



bool CMAPost::LoadWords(const CPlmLineCollection *piInTextItems)
{
    long i = 0;
   	try
	{
		m_Words.clear();
        int CurrOborotNo = -1;
	    
        for (; i < piInTextItems->m_Items.size(); i++)
        {
	        const char* strPlmLine = piInTextItems->m_Items[i].c_str();
    		if ( !CheckIfHomonymPlmLine(strPlmLine))
            {
    		    //  reading the first homonym and word's  properties 
			    CPostLemWord Word(m_pRusGramTab);
                if (!Word.ProcessPlmLineForTheFirstHomonym(strPlmLine, morphRussian, CurrOborotNo))
			        return false;

		        Word.m_bHasSpaceBefore = 	m_Words.empty() 
								||	m_Words.back().m_GraphematicalUnitOffset+m_Words.back().m_strWord.length() < Word.m_GraphematicalUnitOffset
								||	m_Words.back().m_bSpace;
                m_Words.push_back(Word); 			

		
	        }
	        else
                if (!m_Words.empty()) // // ������ ������ ������ �������� ������ ���������� ���� ��������
	            {	
                    if (!m_Words.back().AddNextHomonym(strPlmLine))
			        return false;
	            }

    	}
        return true;
	}
	catch(...)
	{
		ErrorMessage ("MAPOST", Format("Cannot read line %i (\"%s\") from morphology", i, piInTextItems->m_Items[i].c_str()));
		return false;
	}
}

bool CMAPost::ProcessData(const CPlmLineCollection *piInTextItems)
{
	try
	{
        LoadWords(piInTextItems);

		RunRules();

		ClearVector(m_ResultLemWords);
        for (list<CPostLemWord>::const_iterator it = m_Words.begin(); it !=  m_Words.end(); it++)
            for (size_t i=0; i < it->GetHomonymsCount(); i++)
            {
                if (m_bHumanFriendlyOutput)
                    m_ResultLemWords.push_back(it->GetDebugString(it->GetHomonym(i), i==0));
                else
			        m_ResultLemWords.push_back(it->GetPlmStr(it->GetHomonym(i), i==0));
            }
                
		m_Words.clear();
		return true;

	}
	catch(...)
	{
		return false;
	}
}



//================================================
//======          �������     ===================
//================================================





/*
 ���� ����� �������������� ����� � ������� �������� ����� �����, 
 ����� ������� ���� ���������,  � �� ������.
*/

void CMAPost::Odnobuk()    
{
	for (CLineIter it=m_Words.begin(); it !=  m_Words.end(); it++)
	{
        // ������������� �����,  ����.- �����
        CPostLemWord& W = *it;
        if (!W.HasDes(ORLE)) continue;
        if (W.m_strWord.length() != 1 ) continue;
        if (!W.m_bFirstUpperAlpha) continue;
        if (W.HasOborot1() != W.HasOborot2()) continue;// "�.�." - ��� ������, ��� ������������ �� ����

		
        const poses_mask_t poses = (1<<PREP) | (1<<CONJ) | (1<<INTERJ) | (1<<PARTICLE);
        // ������ ����� ����� ��������  ������ ���������, ������, ����������� ��� ��������
        if (W.GetPoses() & ~poses) continue;//?? "�� �.��������� �����"
        CLineIter next_it=it;
        next_it++;
        if(next_it==m_Words.end())continue;
        if (next_it->m_strWord == ".") 
	    {
            W.DeleteOborotMarks();
            W.DeleteAllHomonyms();
            CHomonym *pH = W.AddNewHomonym();
            pH->m_strLemma = W.m_strUpperWord;
            pH->SetMorphUnknown();
            pH->SetGramCodes(m_DURNOVOGramCode);
            pH->InitAncodePattern();
	    }
	}
}

/*
 1. ���� ����� ����� ��� ����� ����������, ����� ��� ����� �� ����� �����������
 2. ���� ����� ������� "�" � "��" ��� ����� ��������� � ��� ���� �� �������� �����������,
	�����  ��� �� ����� ���� �������,
	
 
*/

void CMAPost::Rule_Interjections()    
{
    bool bAfterPunctOrSentEnd = true;
    for (CLineIter it=m_Words.begin(); it !=  m_Words.end(); it++)
        if (!it->m_bSpace)
        {
            CPostLemWord& W = *it;
            if (W.HasDes(ORLE)) 
            {
                if (W.m_strUpperWord == "�" || W.m_strUpperWord == "��")
                {
                    if (!bAfterPunctOrSentEnd)
                        W.KillHomonymOfPartOfSpeech(CONJ);
                    else
                        W.KillHomonymOfPartOfSpeech(PARTICLE);
                }

                if (W.HasPos(INTERJ))
                {
                    CLineIter next_it = NextNotSpace(it);
                    if (next_it == m_Words.end() || !next_it->HasDes(OPun))
                        W.KillHomonymOfPartOfSpeech(INTERJ);
                }
            }
            bAfterPunctOrSentEnd =  W.HasDes(OPun) || W.HasDes(OSentEnd);;
        };
};



//
/*
  ���������  ����  ������������������  LLE HYP �, ��� � - ��� ��, � �������
  ���� ������� �� �������������� ��� ��������������, ���� �������. ����� LLE � HYP
  �������� � ������ ����� � ���������� �, � ������  LLE � HYP ������������.
  �������� �� ��������:
     ftp-������
	 ftp-���������
*/
void CMAPost::ILeDefLe()
{
    for (CLineIter it=m_Words.begin(); it !=  m_Words.end(); it++)
	{
        CPostLemWord& W = *it;
        if (W.m_strWord.length() < 3) continue;
        int hyp = W.m_strWord.find("-");
        if (hyp == string::npos) continue;
        // ������ ����� - ��������, ������� - �������
        if (    !is_english_alpha((BYTE)W.m_strWord[0]) 
            ||  is_russian_alpha((BYTE)W.m_strWord[0]) //   is_english_alpha � is_russian_alpha ������������
            ||  !is_russian_alpha((BYTE)W.m_strWord[W.m_strWord.length() - 1])) continue;
        if (W.IsInOborot()) continue;

        if (W.LemmatizeForm(W.m_strWord.substr(hyp+1), m_pRusLemmatizer))
            for (int i=0; i < W.GetHomonymsCount(); i++)
            {   
                CHomonym* pH = W.GetHomonym(i);
                pH->m_lPradigmID = -1;
                pH->m_LemSign = '-';
                if (pH->m_CommonGramCode.empty())
                    pH->m_CommonGramCode = "??";
                pH->m_strLemma.insert(0, W.m_strUpperWord.substr(0, hyp+1));
            }
	}
}

string CMAPost::GetSimilarNumAncode (const string&  Lemma, const string&  Flexia, bool IsNoun) 
{
	if (Lemma.length() == 0) return "";
	vector<CFormInfo> Paradigms;
	string h = Lemma;
	m_pRusLemmatizer->CreateParadigmCollection(false, h, false, false, Paradigms);
	if ( Paradigms.size() == 0 ) return ""; // ��������, ���� � �-������� ����� ���������
	// ���� ������������
	long k=0;
	for (; k < Paradigms.size(); k++)
	{
	  string AnCode = Paradigms[k].GetAncode(0);
	  BYTE POS = m_pRusGramTab->GetPartOfSpeech(AnCode.c_str() );
    if (IsNoun)
    {
        if (POS == NOUN)
            break;
    }
    if ( (POS == NUMERAL) ||  (POS == NUMERAL_P) || Lemma == "�������")
	    break;
	};
	assert (k < Paradigms.size());
	const CFormInfo& P = Paradigms[k];

	// ���� ������������ ���������� � ����� 
	string AnCodes;
  	for (k=0; k < P.GetCount(); k++)
	{
		  string Form = P.GetWordForm(k);
		  EngRusMakeLower(Form);
		  if ( IsNoun && Form != h && string("��,��,��,��,��,��,��").find(P.GetAncode(k)) != string::npos ) // 1000 - �� ����, "����� 1000 �������"
			  continue;
		  if (Form.length() > Flexia.length())
			  if (Flexia == "" || Flexia == Form.substr (Form.length()-Flexia.length()))
				  AnCodes += P.GetAncode(k);
	};

	return m_pRusGramTab->UniqueGramCodes(AnCodes);
	

};


void CMAPost::Cifrdef()    
{
	
	// ���� ������������������ ����� � ��������� ����
	// 1960-��                           2 6 DSC
	//� ������������� ����� ����� ��� ������������ 
	//	1960-��                            4 0 4 RLE -= 1848 �� -1 #0
	// � ��� �� ������� ���������
	CLineIter dollar = m_Words.end();
	for (CLineIter it=m_Words.begin(); it !=  m_Words.end(); it++)
	{
        CPostLemWord& W = *it;
		CLineIter next_it=it;
		CLineIter prev_it=it;
		next_it++;
		if ( it !=  m_Words.begin() ) prev_it--;
        //if (W.m_strWord.length() < 3) continue;
        int hyp = W.m_strWord.find("-");
        //if (hyp == string::npos) continue;
        if (W.IsInOborot()) continue;	
		
        // �������
        if (!isdigit((BYTE)W.m_strWord[0]) &&  !(hyp > 0 && is_russian_alpha((BYTE)W.m_strWord[W.m_strWord.length() - 1]))) 
		if (dollar==prev_it)//$9,4 ����
		{
			vector<CFormInfo> Paradigms;
			m_pRusLemmatizer->CreateParadigmCollection(false, W.m_strWord, false, false, Paradigms);
			if ( Paradigms.size() == 0 ) 
				continue;
			for (long k=0; k < Paradigms.size(); k++)
				//if (m_pRusGramTab->GetPartOfSpeech(Paradigms[k].GetAncode(0).c_str()) == NOUN)
				{
					string Form = Paradigms[k].GetWordForm(0);
					for(int i = NumeralToNumberCount; i >= 0;  i--)
						if ( NumeralToNumber[i].m_Cardinal == Form || NumeralToNumber[i].m_Ordinal == Form )
						{
							if((*it).HasDes(OSentEnd)) {(*it).DelDes(OSentEnd);(*prev_it).AddDes(OSentEnd);}
							if((*it).HasDes(CS_Undef)) {(*it).DelDes(CS_Undef);(*prev_it).AddDes(CS_Undef);}
							iter_swap(prev_it, it);
							dollar++;
							break;
						}
					break;
				}
		}
		else 
			if (isdigit((BYTE)W.m_strWord[W.m_strWord.length()-1]) && hyp > 0 && is_russian_alpha((BYTE)W.m_strWord[0]))
			{
				W.DelDes(ONumChar);
				W.AddDes(ORLE);
				W.DeleteAllHomonyms();
				CHomonym* pNew = W.AddNewHomonym();
				vector<CFormInfo> Paradigms;
                string TmpStr = W.m_strWord.substr(0,hyp);
				m_pRusLemmatizer->CreateParadigmCollection(false, TmpStr, false, false, Paradigms);
				if(Paradigms.size() > 0) // ��������-238
				{
					pNew->m_lPradigmID = Paradigms[0].GetParadigmId();
					pNew->m_CommonGramCode = Paradigms[0].GetCommonAncode();				
					pNew->SetGramCodes(Paradigms[0].GetSrcAncode());
					pNew->m_LemSign = '+';
					W.m_bWord = true;
					W.m_bPredicted = false;
				}
				else
				{
					pNew->SetGramCodes(m_DURNOVOGramCode);
					pNew->SetMorphUnknown();
				}
				pNew->m_strLemma = W.m_strUpperWord;
				pNew->InitAncodePattern();
			}
			else
			continue; 
		// ������ ����� - �����, ������� - �������, ���� ���� ���������
        string NumWordForm = hyp < 0 ? it->m_strWord : it->m_strWord.substr(0, hyp); 
		string Flexia = hyp < 0 ? "" : it->m_strWord.substr(hyp+1); 

		if( Flexia == "" && next_it !=  m_Words.end() && isdigit((BYTE)next_it->m_strWord[0]) && next_it->m_strWord.length()==3) // "� 1 300 �������� �����" -> 1300
		{
			next_it->m_strWord = W.m_strWord + next_it->m_strWord;
			W.DeleteAllHomonyms();
			m_Words.erase(it, it);
			continue;
		}
		//  ���� �  ����� ���� ������������, ������� ����������� ��������� � ����� � ������������ �� ������� ������.
		int i = NumeralToNumberCount + (NumWordForm == "0" ? 0 : - 1); //������� ����
		string NumWordForm2 = NumWordForm;
		while(atoi(NumWordForm2.c_str())>=1000 && NumWordForm2.substr(NumWordForm2.length()-3) == "000" )
			NumWordForm2 = NumWordForm2.substr(0, NumWordForm2.length() - 3 );
		for(; i >= 0;  i--)
		{
			string NumValue;
            if (W.HasDes(ORoman) )
				NumValue = NumeralToNumber[i].m_RomanNumber;
			else
				NumValue = IntToStr(NumeralToNumber[i].m_Number);
			
			if (NumValue.length() > 0)
				if (NumWordForm2.length() >= NumValue.length())
					if (NumValue  == NumWordForm2.substr(NumWordForm2.length() - NumValue.length()) )
						break;
		};
		if (i < 0)  continue;
        
	    EngRusMakeLower(Flexia);
        string AnCodes = GetSimilarNumAncode(NumeralToNumber[i].m_Cardinal, Flexia, NumeralToNumber[i].m_bNoun);
		if  ( AnCodes.empty() && Flexia != "" )
			AnCodes = GetSimilarNumAncode(NumeralToNumber[i].m_Ordinal, Flexia, NumeralToNumber[i].m_bNoun);
		if( !strcmp(NumeralToNumber[i].m_Cardinal, "����")) AnCodes = "������������������������������������"; //��� �������� � �����
		string AnCodes0 = AnCodes; //����
        if  (NumWordForm != "0")
			AnCodes = GetSimilarNumAncode(NumeralToNumber[i].m_Ordinal, Flexia, NumeralToNumber[i].m_bNoun );
		if ( Flexia == "" )
			AnCodes = m_pRusGramTab->FilterGramCodes(rAllNumbers,AnCodes, _QM(rSingular));
		if ( FindFloatingPoint(NumWordForm.c_str()) != -1 || AnCodes0 == AnCodes)
			AnCodes = "";
        if  ( AnCodes.empty() && AnCodes0.empty() ) 
        {
            // "20-������"
            if (W.LemmatizeForm(Flexia, m_pRusLemmatizer))
            {
                W.DelDes(ONumChar);
                W.AddDes(ORLE);
                W.SetWordStr(NumWordForm + "#" +Flexia, morphRussian);
            }
        }
        else
        {

            W.AddDes(ORLE);
            W.AddDes(OLw);
            W.DeleteAllHomonyms();

			if  ( !(AnCodes0.empty() || ( next_it !=  m_Words.end() && (next_it->m_strUpperWord == "��") ) ) ) // ����
			{
				CHomonym* pH = W.AddNewHomonym();
				pH->SetMorphUnknown();
				pH->SetGramCodes(AnCodes0);
				pH->m_strLemma = NumWordForm;
				pH->InitAncodePattern();
			}
			CLineIter spec_it=it;
			if( next_it !=  m_Words.end() && (next_it->m_strWord == "%" || next_it->m_strWord == "$"))  //�������, ��������
				spec_it = next_it;
			else if ( it !=  m_Words.begin() && (prev_it->m_strWord == "$"||prev_it->m_strWord == "�"))
				spec_it = prev_it;
			if( it != spec_it)
			{
				CPostLemWord& W2 = *spec_it;
				vector<CFormInfo> Paradigms;
				W2.DeleteOborotMarks();
				W2.AddDes(ORLE);
				W2.DelDes(OPun);
				W2.DeleteAllHomonyms();
				CHomonym* pH = W2.AddNewHomonym();
				pH->SetMorphUnknown();
				pH->m_CommonGramCode = "��";				
				pH->SetGramCodes("��");
				if ( W2.m_strWord == "%" )
				{
					W2.m_strUpperWord = W2.m_strWord = "����";
					pH->m_strLemma = "�������";
				}
				else  if ( W2.m_strWord == "$" )
				{
					if(spec_it == prev_it) //$12
						dollar = prev_it;
					W2.m_strUpperWord = W2.m_strWord = "����";
					pH->m_strLemma = "������";
				}
				else  if ( W2.m_strWord == "�" )
				{
					W2.m_strUpperWord = W2.m_strWord = "�";
					pH->m_strLemma = "�����";
					//pH->m_GramCodes = "";
				}
				m_pRusLemmatizer->CreateParadigmCollection(true,  pH->m_strLemma, true, false, Paradigms);
				pH->m_lPradigmID = Paradigms[0].GetParadigmId();
				W2.m_bWord = true;
				W2.m_bPredicted = false;
				pH->m_LemSign = '+';
				pH->InitAncodePattern();
			}
			else
				if  ( !(( next_it !=  m_Words.end() && (next_it->m_strUpperWord == "���"))	// "� ������� 2 ���"
					//|| (it !=  m_Words.begin() && prev_it->HasGrammem(rComparative) && prev_it->HasPos(NUMERAL)) // "��������� ����� 40 ������ ������������"
					) )  
				 if  ( !AnCodes.empty() )  //����-�
				 {
					 CHomonym* pH = W.AddNewHomonym(); 
					 pH->SetMorphUnknown();
					 pH->SetGramCodes(AnCodes);
					 pH->m_strLemma = NumWordForm;
					 pH->InitAncodePattern();
				 }
			if( dollar == prev_it )
			{
				if((*it).HasDes(OSentEnd)) {(*it).DelDes(OSentEnd);(*prev_it).AddDes(OSentEnd);}
				if((*it).HasDes(CS_Undef)) {(*it).DelDes(CS_Undef);(*prev_it).AddDes(CS_Undef);}
				iter_swap(prev_it, it);
				dollar++;
			}
        }

	};

};









// ���� �� ���� ������
// ���� ������� ����������, ������� ������������ ��������� ������ ����� ������� 
// � ���������, �� ��������� ��� ���������� �� ��� �������
// ��������, "��������  + ��������� ����"
// �� � ��,��,���,���,3�,��
// �� � ��,��,���,���,���,��,��,��
// ����� ������ ��������� �������� 
// ��������  + ��������� ��
//   ��������  + ��������� ��

void CMAPost::ParticipleAndVerbInOneForm() 
{
    for (CLineIter it=m_Words.begin(); it !=  m_Words.end(); it++)
    {
        CPostLemWord& W = *it;
        
        if (!W.HasPos(PARTICIPLE_SHORT) || !W.HasPos(VERB)) continue;

        for (int HomNo=0; HomNo < W.GetHomonymsCount(); HomNo++)
        {
            CHomonym* pH = W.GetHomonym(HomNo);
            const string& GramCodes = pH->GetGramCodes();
            string VerbGramCodes;
            string PartGramCodes;
            if (GramCodes == "??") continue;
            for (long i=0; i < GramCodes.length(); i+=2)
            {
                string gram = GramCodes.substr(i, 2);
                BYTE POS = m_pRusGramTab->GetPartOfSpeech(gram.c_str());
                if (POS == PARTICIPLE_SHORT)
                    PartGramCodes += gram;
                else 
                    if (POS == VERB)
                        VerbGramCodes += gram;
            };
            if (VerbGramCodes.empty() || PartGramCodes.empty()) continue;
            pH->SetGramCodes(  VerbGramCodes );
            pH->InitAncodePattern();
            CHomonym NewHom = *pH;

            CHomonym* pNewHom = W.AddNewHomonym();
            *pNewHom = NewHom;
            pNewHom->SetGramCodes ( PartGramCodes);
            pNewHom->InitAncodePattern();
        }
    };
};

/*
 ���� �� ���� ������, ����� � - �������  ������� ���������� =/= "���", "��", "��", "���", "��"
 ����� � ������������ ��������� ��� ������� ��-� 
 � �� (��������, "����", "������"). ���������, ����� �� ����� ���� ���������� 
 ������������� ���������������,  ���������, ��������������, ��� �������. ����, ������������� � ���� �� ���� ����� � ������.
 ���� �����, ����� ������� ������� ��.
 ��������, 
   +���� ���� 
   +������  �������
   -���� � �����, �� ������ �������.
   -��� ������.
   -� ������ ���/�� ������� ��������
   -� ������ �� ��������.
*/
void CMAPost::PronounP_Pronoun_Homonymy() 
{
    for (CLineIter it=m_Words.begin(); it !=  m_Words.end(); it++)
    {
        CPostLemWord& W = *it;
        if (!W.HasPos(PRONOUN_P) || !W.HasPos(PRONOUN)) continue;

        if (   (W.m_strUpperWord == "���") 
            || (W.m_strUpperWord == "��") 
            || (W.m_strUpperWord == "��") 
            || (W.m_strUpperWord == "���") 
            || (W.m_strUpperWord == "��") 
            || (W.m_strUpperWord == "Ũ") 
            )
            continue;

        int HomNo = W.GetHomonymByPOS(PRONOUN_P);
        if (HomNo == -1)  continue;

        CLineIter next_it = NextNotSpace(it);
        if ( next_it == m_Words.end() ) continue;
        if ( next_it->GetHomonymsCount() == 0)  continue;

        bool bAgreed = true;
        for (int i =0; i < next_it->GetHomonymsCount(); i++)
        {
            const CHomonym* pNoun = next_it->GetHomonym(i);
            bAgreed = bAgreed && m_pRusGramTab->GleicheGenderNumberCase (pNoun->m_CommonGramCode.c_str(), pNoun->GetGramCodes().c_str(), W.GetHomonym(HomNo)->GetGramCodes().c_str());
        }
        if (bAgreed)
            W.KillHomonymOfPartOfSpeech(PRONOUN);

    };

};

// ������ �� ����� � ����� ���� ���������
bool CMAPost::HasParadigmOfFormAndPoses(string WordForm, poses_mask_t Poses) const
{
	vector<CFormInfo> Paradigms;
	m_pRusLemmatizer->CreateParadigmCollection(false, WordForm, false, false, Paradigms);
	
	for (long k=0; k < Paradigms.size(); k++)
	{
	  string AnCode = Paradigms[k].GetSrcAncode();
	  BYTE POS = m_pRusGramTab->GetPartOfSpeech(AnCode.c_str() );
	  QWORD Grams;
	  m_pRusGramTab->GetGrammems(AnCode.c_str(), Grams);
	  if ( Poses & (1<< POS) )
		 return true;
	};

	return false;
}





/*
 ������� ���� �������������� �� ������, ���� ����� ������� ��� ������, ����� �������� �����
 �  ������� ����� ����� �������� �� ������������ ������ CFixedColloc::m_InterfaceString
 ��������, ������ "��� �� �� �� ���" ��������� �� "���?"
*/
void CMAPost::FixedCollocations() 
{
	for (CLineIter it=m_Words.begin(); it !=  m_Words.end(); it=NextNotSpace(it))
	{
		for (long CollocNo =0; CollocNo  < m_FixedCollocs.size(); CollocNo++)
		{
			CLineIter main_it = m_Words.end();
            CHomonym* main_hom = 0;
			bool HasEndSent = false;
			long ItemNo=0;
            const CFixedColloc& C = m_FixedCollocs[CollocNo];
            int OpenOborots = 0;;
            CLineIter curr_it = it;
			for (; ItemNo < C.m_Lemmas.size() && curr_it != m_Words.end(); ItemNo++, curr_it=NextNotSpace(curr_it))
			{
                CPostLemWord& W = *curr_it;
				/*
					���� ��������� �������������� ������������ � ��������, �� �� �������� ���, � ������ �� �������� ������
					��������������(��������,"��� �� ���� ���" ������ "�� ����"), �����  �� ����� ������� ��� 
				*/
                if (W.HasOborot1())     OpenOborots++;
                if (W.HasOborot2())     OpenOborots--;
                HasEndSent |= W.HasDes(OSentEnd);

                
                int i=0;
                for (; i < W.GetHomonymsCount(); i++)
                {
                    const CHomonym* pH = W.GetHomonym(i);
					
                    if (        (pH->m_strLemma == C.m_Lemmas[ItemNo].m_Lemma) // �������� �����
                            &&   (      (C.m_Lemmas[ItemNo].m_POS == UnknownPartOfSpeech) // �������� ����� ����
                                    || pH->HasPos(C.m_Lemmas[ItemNo].m_POS)
                                )
                        )
                    {
                        W.SetAllOtherHomsDel(i); // ��������  ��������� ������
                        break;
                    }
                }

                if (i == W.GetHomonymsCount()) break; // ��� ����� �� �������

				// ��������� ������� ������ ��������������, ����� ����� �� �� �������
				if (ItemNo  == C.m_MainWordNo)
				    main_it = curr_it;
			};

			if (OpenOborots == 0)
				if (ItemNo == C.m_Lemmas.size())
				{
			        // ����� ��������������.
			        // it  ��������� �� ������ ����� ��������������
			        // curr_it ��������� �� ������ �����, ������� ��� �� ������ � ��������������

                   	if (HasEndSent)
                        main_it->AddDes(OSentEnd);

                    main_it->SafeDeleteMarkedHomonyms();
                    assert (main_it->GetHomonymsCount() == 1);
	                // ������ ����� � ������� ������
                    main_it->GetHomonym(0)->m_strLemma = C.m_InterfaceString;
                    m_Words.erase(it, main_it);
                    it = main_it;
                    it++;
                    m_Words.erase(it, curr_it);
					// ������� �� ����� �� ���������������, ��������� ����� ���� �� ���� ��������������
                    it = main_it;
					break;
				};
		}; // ���� �� ���������������

	}
};


void CMAPost::OtherRules() 
{
	for (CLineIter it=m_Words.begin(); it !=  m_Words.end(); it++)
	{
		CPostLemWord& W = *it;
		CLineIter next_it=it;
		CLineIter prev_it=it;
		next_it++;
		if ( it !=  m_Words.begin() ) 
		{
			prev_it--;
			if (W.HasPos(VERB) && W.GetHomonymsCount()>0 && (*prev_it).GetHomonymsCount() == 1 && (*prev_it).HasPos(PREP))
				for (int HomNo=0; HomNo < W.GetHomonymsCount(); HomNo++)
					if( W.GetHomonym(HomNo)->HasPos(VERB) )
						W.EraseHomonym ( HomNo );
		}
		if( W.HasDes(ONumChar) && is_alpha((BYTE)W.m_strWord[0]) && W.GetPoses()==0 )
		{
			W.DeleteAllHomonyms();
            CHomonym* pNew = W.AddNewHomonym();
            pNew->SetMorphUnknown();
            pNew->m_strLemma = W.m_strUpperWord;
			pNew->m_CommonGramCode = "��";
            pNew->SetGramCodes (m_pRusGramTab->GetGramCodes(NOUN, rAllCases | rAllNumbers | rAllGenders, 0));//m_DURNOVOGramCode;//"����������������������������������������������������������������������������������������";
            pNew->InitAncodePattern();
			W.AddDes( is_alpha((BYTE)W.m_strWord[0], morphEnglish) ? OLLE : ORLE);
		}
	};
}

/*
 ������� ������ EXPR1 � EXPR_NOxxx �� ��������. �����, ���� ��� ����  �� ���� ������� 
  EXPR2 ����� 20 ����
 */

void CMAPost::CorrectOborots() 
{
    for (CLineIter it=m_Words.begin(); it !=  m_Words.end(); it++)
    {
        CPostLemWord& W = *it;
        if (W.HasOborot1())
        {
            int Count = 0;
            for (CLineIter next_it=it; next_it !=  m_Words.end(); next_it++)
            {
                if  (       (Count > 20)
                        ||  (next_it->HasOborot1() && it != next_it)
                    )
                {
                    W.DeleteOborotMarks();
                    break;
                }
                if (next_it->HasOborot2())
                    break;
                Count++;
            }
        }
    }
};

/*
 ������� �������� �� ���� ������, ������� �� ���� ������� � ����������.
 �������, �� ���������� �� ��� � �������� "����-",  ���� ����������, � � ���������� ����
 ����� �� ��������������  ��� ���������, ����� ������ ����� ��� ���������, � � ������.
 ������ �������  ������� "#����"
*/

void CMAPost::SemiAdjectives() 
{
    for (CLineIter it=m_Words.begin(); it !=  m_Words.end(); it++)
    {
        CPostLemWord& W = *it;
        if ( !W.HasDes(ORLE) ) continue;
        if ( !W.m_bPredicted ) continue;
        if ( W.m_strUpperWord.substr(0,4) != "����") continue;
        for (int i=0; i < W.GetHomonymsCount(); i++)
            W.GetHomonym(i)->m_strLemma.erase(0,4);
        // ��������� ��������������� ������
        W.m_UnparsedGraphemDescriptorsStr  += " #���� ";
    };
};


/*
 ������� �������� �� ���� ������, ������� �� ���� ������� � ����������.
 �������, �� ���������� �� ��� � �������� "���-" ("����-"),  ���� ����������, � � ���������� ����
 ����� �� ���������������  ��� ���������, ����� ������ ����� ��� ���������, � � ������.
 ������ �������  ������� "#����".
 ���� ����� ��� ��������� ����� � ����������� ������, ����� ����� ���������
 ������� ��., ��. �  ��.
 ��������,
  ������� ��������� (��)
  � ������� ������� (��)
  ����� ����� �������� (��) ??
 �� ���� ��������� ������� ����� �������� �������� ��� ���.
   ��������� ������ 
*/

/*
  "������ ������� ���� ���� �������� ����������� ����������"
  "������ �������"
   �������,	 ��� ��� �������������� ������������ � "���-" ����� �������� �������������� 
   �����, ����� ��������� ������ "������ �������" . ������� �������:
   "������ �������"
   "���-������ ����� �� ����" 
   ����� �� ���������� �� ����������, �� ��������� �� ���������,
   ��� �������������� �� ���� �� �������� ���������.
*/

void CMAPost::SemiNouns() 
{
    for (CLineIter it=m_Words.begin(); it !=  m_Words.end(); it++)
    {
        CPostLemWord& W = *it;
        if ( !W.HasDes(ORLE) ) continue;
        if ( !W.m_bPredicted ) continue;

        size_t PrefixLen;
        bool bChangeToPlural = false;
        {
            if (W.m_strUpperWord.substr(0,4) == "����")
                PrefixLen = 4;
            else
               if ( W.m_strUpperWord.substr(0,4) == "���-")
               {
                   PrefixLen = 4;
                   bChangeToPlural = true;
               }
               else
                if (W.m_strUpperWord.substr(0,3) == "���")
                {
                    bChangeToPlural = true;
                    PrefixLen = 3;
                }
                else
                    continue;
            
        }


        BYTE POS = NOUN;
        {
            string WordForm = it->m_strUpperWord;
            WordForm.erase(0, PrefixLen);
            if (WordForm.length() <= 3) continue;
            if (!HasParadigmOfFormAndPoses(WordForm, 1<<POS) )
            { 
                POS = NUMERAL;
                if (!HasParadigmOfFormAndPoses(WordForm, 1<<POS)) continue; //�����������
                
            };
        }
        
        
        // ��������� ��������������� ������
        W.m_UnparsedGraphemDescriptorsStr += " #���� ";

        // ��������� �������
        if (bChangeToPlural)
        for (int HomNo=0; HomNo < W.GetHomonymsCount(); HomNo++)
        {
            CHomonym* pH = W.GetHomonym(HomNo);
            QWORD Grammems;
            string GramCodes;
            
            bool SingularGenitivFound = false;
            for (int i=0; i < pH->GetGramCodes().length(); i+=2) 
                if (pH->GetGramCodes()[i]!='?')
                {
                    m_pRusGramTab->GetGrammems((pH->GetGramCodes().c_str() + i), Grammems);
                    if (Grammems & _QM(rSingular))
                    {
                        // ������������ �� ������������� �����
                        Grammems &= (~ _QM(rSingular));
                        Grammems |= _QM (rPlural);

                        string NewGramCode;
                        if (m_pRusGramTab->GetGramCodeByGrammemsAndPartofSpeechIfCan (POS, Grammems, NewGramCode))
                            GramCodes += NewGramCode;

                        if ( Grammems & _QM(rGenitiv) )
                        {
                            QWORD Gram  = Grammems & ~_QM(rGenitiv);
                            // ��������� �����������  � ������������ ��. �����, ���� ����� �����������
                            if (m_pRusGramTab->GetGramCodeByGrammemsAndPartofSpeechIfCan (POS, Gram | _QM(rNominativ), NewGramCode))
                                GramCodes += NewGramCode;

                            if (m_pRusGramTab->GetGramCodeByGrammemsAndPartofSpeechIfCan (POS, Gram | _QM(rAccusativ), NewGramCode))
                                GramCodes += NewGramCode;
                            
                        }
                    }
                    else
                        GramCodes += pH->GetGramCodes().substr(i,2);

                };

            if (!GramCodes.empty() )
            {
                pH->SetGramCodes( GramCodes );
                pH->InitAncodePattern();
            }

            
        }


    }

};




/*
  ������� ����  �� ����� � ���� ���������� ���. ���� ����� ���� ��� ������� ��� �����
  � ����������� �������, ����� �� ���� ���������� ��������� ������ ���������������
  ������������� �������
  ��������:
   + ������ � ����� ���
   + ��� ����
   - ��� ������� ���, ��� �����.
   - ��� ������� ��� ������.
*/
void CMAPost::Rule_UZHE() 
{
    for (CLineIter it=m_Words.begin(); it !=  m_Words.end(); it++)
    {
        CPostLemWord& W = *it;
        if ( !W.HasDes(ORLE) ) continue;
        if (W.m_strUpperWord !=  "���") continue;

        CLineIter next_it = NextNotSpace(it);

        if (        ( next_it != m_Words.end() )
                &&  !next_it->m_bComma 
                &&  ( next_it->GetHomonymByGrammem(rGenitiv) == -1) 
            )
        {
            W.KillHomonymOfPartOfSpeech(ADJ_FULL);
        }
    };

};



bool  CMAPost::NounHasObviousPluralContext(CLineIter it)
{
	if ( it  == m_Words.begin() ) return false;
    it--;
    it = BackSpaces(it);
    CPostLemWord& W = *it;
    return          W.GetHomonymByPOS(NUMERAL) != -1
            ||      W.GetHomonymByPosesandGrammem((1<<ADJ_FULL)|(1<<NUMERAL_P)|(1<<PRONOUN_P), rPlural) != -1;
 
};

/*
 ������� ���� �� ������������ ����������������, ������� ������������� �� "-�".
 ���� ����� �� ���� ����� ������������ (������ "����"), ����., ���.���� ��� ��-�
 � ������������� ������������� ������, ����� ������� �� �������. 
 ����� ������� � ���� ������ �������������� �����. 
 -������� ������
 -�� ������
 +������� ������
 +������ ������
 +��� ������
*/
/*
	��� �� ������� �������� ��� ���������������, ������� ����������� 
	�� ������� ������������ (��� "��"), ���� ��� ������������� �� "-o".
	��� ���� ����� ���������������� ��� ��������������� � ������������ �����,
	����� ���� ���������� ������ ��� "��" ("��"+ "��" = "��")
*/

void CMAPost::Rule_Ideclinable() 
{
	for (CLineIter it=m_Words.begin(); it !=  m_Words.end(); it++)
	{
        CPostLemWord& W = *it;
        if ( !W.HasDes(ORLE) ) continue;
        if ( !W.HasPos(NOUN) ) continue;
		const string& WordForm = W.m_strUpperWord;
        if (WordForm.empty()) continue;
		if ((BYTE)WordForm[WordForm.length() - 1] != (BYTE)'�') continue;
        for (int i=0; i < W.GetHomonymsCount(); i++)
        {
            CHomonym* pH = W.GetHomonym(i);
            if ( (pH->m_iGrammems & rAllCases) != rAllCases) continue; 
		    if ( (pH->m_iGrammems & rAllNumbers) != rAllNumbers) continue;

		    string GramCodes = pH->GetGramCodes();
		    // ����� ������ ���� ������ ����, �������� ��� ������������ ���������������
		    if (GramCodes.length() != 2) continue;
		    if (!NounHasObviousPluralContext(it))
		    {
			    // �������� ������������� �����
			    string NewGramCode;
			    if (m_pRusGramTab->GetGramCodeByGrammemsAndPartofSpeechIfCan (NOUN, pH->m_iGrammems & ~_QM(rPlural), NewGramCode))
                {
                    pH->SetGramCodes( NewGramCode );
                    pH->InitAncodePattern();
                }
                else
                {
                    // �������� � ������� �� -� ��� �������� � ��. ������
                }
		    };
        }

	};
};


/*
	������� ���� �� ���� ������. ���� ����� ���� ������� � ������� ����,  � ������� ����������
	������������� ����� � ����� �� ��������� � "����������� ������������� ���������", �����
	������� ��� �������� �������������� ����� � ���� �����.
*/
void CMAPost::Rule_DeadPlurals() 
{
	for (CLineIter it=m_Words.begin(); it !=  m_Words.end(); it++)
	{
		CPostLemWord& W = *it;
        if (!W.HasPos(NOUN)) continue;
        if (NounHasObviousPluralContext(it)) continue;
        if (W.m_bPredicted) continue;
        if (!W.HasGrammem(rSingular)) continue; // only pluralia 
        for (int i=0; i < W.GetHomonymsCount(); i++)
        {
            CHomonym* pH = W.GetHomonym(i);
            pH->m_bDelete = false;
            if (pH->HasGrammem(rDeFactoSingTantum) )
                if (!pH->DeleteAncodesByGrammemIfCan(rPlural))
                    pH->m_bDelete = true;
		};
        W.SafeDeleteMarkedHomonyms();

	};

};

/*
	������� ���� �� ���� ������. ���� ����� ����������, ����� ������������ ��� �������, 
	������� � ������������� ���� ������������� ��������������. ��������, ��� ����� "�����"
	����� ���������� ������� ����� �� ��������������� "�������", � ��������� ������ ������� 
	"������".

*/
void CMAPost::Rule_RelationalAdjective() 
{
	for (CLineIter it=m_Words.begin(); it !=  m_Words.end(); it++)
	{
		CPostLemWord& W = *it;
        if (((W.GetPoses() & ( (1<<ADJ_SHORT)| (1<<ADJ_FULL))) ) == 0) continue;

        for (int i=0; i < W.GetHomonymsCount(); i++)
        {
            CHomonym* pH = W.GetHomonym(i);
            pH->m_bDelete = false;
            if (pH->m_iPoses & ( (1<<ADJ_SHORT)| (1<<ADJ_FULL)))
            {
                if (!pH->HasGrammem(rQualitative)) 
                {

                    // ������ ������������� ��������������
                    if ( pH->HasGrammem(rComparative))  
                        if (!pH->DeleteAncodesByGrammemIfCan(rComparative))
                            pH->m_bDelete = true; // ���� �� ���������� ��������������� ������, ��������� ������� ���� ������
                    if (pH->HasPos(ADJ_SHORT))
                        pH->m_bDelete = true;
                }
            }
        }
        W.SafeDeleteMarkedHomonyms();
	};

};




enum QuoteMarkEnum {QuoteMark, EOS};
struct CQuoteMark
{
	QuoteMarkEnum m_TokenType;
	CLineIter     m_LineIter;
	bool m_bFirstBeforeLastQuote;
	CQuoteMark () 
	{
		m_bFirstBeforeLastQuote = false;
	};
};


/*
� ������ ����������� �������� ������� �������, ������� ������ ��, ���� ������ ����� ������ �������� (�����������) � �������� �������� 
(�����������) ������ ������ #QUOTED.  ���� ��� ����������� ������� � �� ���� ������� ����������� �� ����� �����������,  
����� ���������, ��� ���������
������� � ����������� ��������� ��� ���������� �������, ��������:
���� �������: "� �������� "������ ������".

*/
void CMAPost::Rule_QuoteMarks()    
{
    //������ #QUOTED
    size_t QuoteNo = 0;
    CLineIter prev_prev_quote_it = m_Words.end();
    CLineIter prev_quote_it = m_Words.end();

    for (CLineIter it=m_Words.begin(); it !=  m_Words.end(); it++)
	{
		CPostLemWord& W = *it;
        if (W.m_bQuoteMark)
        {
            if ((QuoteNo % 2) == 1)
            {
                assert (prev_quote_it !=  m_Words.end());
                for (CLineIter itt=prev_quote_it; itt !=  it; itt++)
                    itt->AddDes(OQuoted);
            }
            QuoteNo++;

            prev_prev_quote_it = prev_quote_it;
            prev_quote_it = it;
        }
        if (W.HasDes(OSentEnd))
        {
            if (((QuoteNo % 2) == 1) && QuoteNo> 1)
            {
                assert (prev_prev_quote_it != m_Words.end());
                assert (prev_quote_it != m_Words.end());
                // ���� �����������, �� ��� �����������
                for (CLineIter itt=prev_prev_quote_it; itt !=  prev_quote_it; itt++)
                    itt->AddDes(OQuoted);
            }
            QuoteNo=0;
        }
    }

    // �������� �������
    CLineIter prev_it = m_Words.begin();
    for (CLineIter it=m_Words.begin(); it !=  m_Words.end(); it++)
	{
        for ( ;(it != m_Words.end()) && it->m_bQuoteMark; it = m_Words.erase(it))
            if (it->HasDes(OSentEnd))
                    prev_it->AddDes(OSentEnd);

        if (it == m_Words.end()) break;
        if (!it->m_bSpace)
            prev_it =  it;
    }


};


/*
����������� ���� ��� ��������������� ������������� ������������
*/
void CMAPost::Rule_ILE()    
{
    for (CLineIter it=m_Words.begin(); it !=  m_Words.end(); it++)
	{
		CPostLemWord& W = *it;
        if ( W.HasDes(OLLE) ) 
		{
            W.DeleteAllHomonyms();
            CHomonym* pNew = W.AddNewHomonym();
            pNew->SetMorphUnknown();
            pNew->m_strLemma = W.m_strUpperWord;
			pNew->m_CommonGramCode = "��";
			//if ( W.HasDes(OUp) ) 
			//	pNew->m_CommonGramCode += "������";//m_DURNOVOGramCode;
			pNew->SetGramCodes ( m_pRusGramTab->GetGramCodes(NOUN, rAllCases | rAllNumbers | rAllGenders, 0));//m_DURNOVOGramCode;//"����������������������������������������������������������������������������������������";
            pNew->InitAncodePattern();
		}
	};
};


/*
  ���� ������������������ ��� ����� �, ���  � � ���� ������ [� �����],
  ������� ����� ���  � �����,   ��������� � � ������  ������ �� [� �����]. 
  ���������  �  �������  #���_�����.
*/
void CMAPost::Rule_KAK_MOZHNO()    
{
    for (CLineIter it=m_Words.begin(); it !=  m_Words.end(); it++)
    {
        // ���� ���������� "���"
        CPostLemWord& W = *it;
        if (!W.HasDes(ORLE)) continue;
        if (W.m_strUpperWord != "���") continue;
        if (W.HasOborot1() != W.HasOborot2()) continue;// "�.�." - ��� ������, ��� ������������ �� ����

        // ���� ���������� "�����"
        CLineIter mozno_it = NextNotSpace(it);
        if ( mozno_it == m_Words.end() ) break;
        if ( mozno_it->m_strUpperWord != "�����") continue;

        // ���� [� �����]
        CLineIter compar_it = NextNotSpace (mozno_it);
        if ( compar_it == m_Words.end() ) break;
        int iHom = compar_it->GetHomonymByGrammem(rComparative);
        if (compar_it->m_strUpperWord == "�����" || compar_it->m_strUpperWord == "�����")
            iHom = compar_it->GetHomonymByPOS(ADV);
        if (iHom == -1) continue;
        if (compar_it->HasOborot1() != compar_it->HasOborot2()) continue;// "�.�." - ��� ������, ��� ������������ �� ����

        //��������� ������ ������ [� �����]
        compar_it->SetAllOtherHomsDel(iHom);
        compar_it->DeleteMarkedHomonymsBeforeClauses();
        compar_it->m_UnparsedGraphemDescriptorsStr += " #���_����� ";
        m_Words.erase(it);
        m_Words.erase(mozno_it);
        it = compar_it;
    }


};

bool CMAPost::CanBeDubleDelimiter(CLineIter it)   
{
		const string& WordForm = it->m_strUpperWord;

		return (   (WordForm == "��") 
			 || (WordForm == ",")
			 || (WordForm == "�")
			 || (WordForm == "-")
			);
};

void CMAPost::Rule_Redublication()    
{
	for (CLineIter it=m_Words.begin(); it !=  m_Words.end(); it++)
	{
		CPostLemWord& W = *it;
        if (!W.HasDes(ORLE)) continue;
   		// ���� ������ � ������, ���� �����,��������, "���� ����, ���� �� ��������"
        if ( W.IsInOborot() ) continue;
        CLineIter last_it = it; 
        bool bHasEndOfSent = false;
        while (true)
        {
		    CLineIter delimiter_iter = NextNotSpace (last_it);
		    if ( delimiter_iter  == m_Words.end() ) break;
		    if (!CanBeDubleDelimiter(delimiter_iter)) break;
            bHasEndOfSent |= delimiter_iter->HasDes(OSentEnd);

			CLineIter duble_word = NextNotSpace (delimiter_iter);
            if ( duble_word == m_Words.end() ) break;
            bHasEndOfSent |= duble_word->HasDes(OSentEnd);
            if (duble_word->m_strUpperWord == "��")
            {
                duble_word = NextNotSpace (duble_word);
                if ( duble_word == m_Words.end() ) break;
                bHasEndOfSent |= duble_word->HasDes(OSentEnd);
            };
  			 // ���� ������ � ������, ���� �����,��������, "���� ����, ���� �� ��������"
            if (duble_word->IsInOborot() )	
				break;
			 // ��� ��������  ��������� ������������!
            if (W.m_strUpperWord != duble_word->m_strUpperWord) break;
            
            last_it = duble_word;
        }

        if (last_it != it)
        {
            CLineIter itt = it;
            itt++;
            while(itt !=  last_it)
                itt = m_Words.erase(itt);
            m_Words.erase(last_it);
            W.m_UnparsedGraphemDescriptorsStr += " #������ ";
            if (bHasEndOfSent)
                W.AddDes(OSentEnd);
        }
    }

};

CLineIter CMAPost::PassSpaces(CLineIter it) 
{
	while (it != m_Words.end())
	{
        if (!it->m_bSpace) break;
		it++;
	};
	return it;
	
};

CLineIter CMAPost::NextNotSpace(CLineIter it) 
{
    if (it == m_Words.end()) return it;
    it++;
    return PassSpaces(it);
};

CLineIter CMAPost::BackSpaces(CLineIter it) 
{
	if (it == m_Words.end()) it--;

	while (it != m_Words.begin())
	{
		if (!it->m_bSpace) break;
		it--;
	};
	return it;
	
};

void CMAPost::Rule_CHTO_ZA() 
{
	for (CLineIter it=m_Words.begin(); it !=  m_Words.end(); it++)
	{
        CPostLemWord& W = *it;
		if (!W.HasDes(ORLE)) continue;
		if (W.m_strUpperWord != "���") continue;
        if( W.HasOborot1() != W.HasOborot2())  continue;

		CLineIter za_it = NextNotSpace(it);
        if (za_it == m_Words.end()) return;  
		if (za_it->m_strUpperWord != "��") continue;
        if( za_it->HasOborot1() != za_it->HasOborot2())  continue;

		CLineIter noun_it = za_it;
        do 
        {
            noun_it = NextNotSpace(noun_it);
            if (noun_it == m_Words.end()) return; 
            if (!m_pRusGramTab->is_left_noun_modifier(noun_it->GetPoses(), 0)) break;
        }
        while (!noun_it->HasPos(NOUN));
        
        if (!noun_it->HasPos(NOUN)) break;
        
        
        vector<CFormInfo> Kakoi;
		string a = "�����";
        m_pRusLemmatizer->CreateParadigmCollection(true,a ,false,false,Kakoi);
        if (Kakoi.empty()) return;

        string GramCodes;
        string WordForm;
        for (int j=0; j<Kakoi[0].GetCount(); j++)
            for (int k=0; k<noun_it->GetHomonymsCount(); k++)
            {
                const CHomonym* pNoun = noun_it->GetHomonym(k);
                if (m_pRusGramTab->GleicheGenderNumberCase(pNoun->m_CommonGramCode.c_str(), pNoun->GetGramCodes().c_str(), Kakoi[0].GetAncode(j).c_str()))
                {
                    GramCodes += Kakoi[0].GetAncode(j);
                    WordForm = Kakoi[0].GetWordForm(j);
                }
            }
        W.DeleteAllHomonyms();
        CHomonym* pNew =  W.AddNewHomonym();
        pNew->SetHomonym(&Kakoi[0]);
        pNew->SetGramCodes (GramCodes);
        RmlMakeLower(WordForm, morphRussian);
        W.SetWordStr(WordForm, morphRussian);
        W.DeleteOborotMarks();
        m_Words.erase(za_it);
	};
};


void CMAPost::InsertComma(CLineIter it)
{
    CPostLemWord P(m_pRusGramTab);
    int dummy;
    P.ProcessPlmLineForTheFirstHomonym(", 0 0 PUN", morphRussian,  dummy);
	m_Words.insert(it,P);
};

/*
 ����  ����������� ���� "��� ��� ... ��� ...",
 ��������, "��� ��� ��������� ��� ��� �� �����".
 ���������  �� � ����������� 
	"��� ��� ��������� ��� ������, ��� �� �����".
*/

void CMAPost::Rule_VOT_UZHE() 
{

    for (CLineIter it=m_Words.begin(); it !=  m_Words.end(); it++)
    {
        CPostLemWord& W = *it;
        if (!W.HasDes(ORLE)) continue;
        if (W.m_strUpperWord != "���") continue;

        CLineIter next_it = NextNotSpace(it);
        if ( next_it == m_Words.end() ) return;  
        if ( next_it->m_strUpperWord != "���" ) continue;


        //	������ ������ ���� �� ���� 
        bool bFound =  false;
        int WordCount =0;
        for (; (WordCount < 10) && (next_it != m_Words.end()); next_it++)
        {
            if (next_it->m_strUpperWord == "���") 
            {	
                bFound = true;
                break;
            };
            if (m_pRusGramTab->IsStrongClauseRoot( next_it->GetPoses())) break;
            WordCount ++;
        };
        if (!bFound) break;

        CPostLemWord P(m_pRusGramTab);
        P.m_bWord = true;
        P.m_TokenLengthInFile = 0;
        P.AddDes(ORLE);
        P.AddDes(OLw);
        P.LemmatizeForm("������", m_pRusLemmatizer);
        P.SetWordStr("������", morphRussian);
        m_Words.insert(next_it,P);
        InsertComma(next_it);
    };
}



/*
  ��������� ���� ���������� �����, ���� ��� �������� � ������� �����, ����� 
  ��������� � ����� �������������� ������ ������������� ����������������
	��������:
	 � ������ ����������� ������ ����������
	 ��� ����� ���-��
*/

void CMAPost::Rule_UnknownNames() 
{	
    for (CLineIter it=m_Words.begin(); it !=  m_Words.end(); it++)
    {
        CPostLemWord& W = *it;
        if (!W.HasDes(ORLE)) continue;
        if (!W.m_bPredicted)  continue;
        if (!W.m_bFirstUpperAlpha) continue;
        for (int i =0; i< W.GetHomonymsCount(); i++)
            W.GetHomonym(i)->m_bDelete = !W.GetHomonym(i)->HasPos(NOUN);
        W.SafeDeleteMarkedHomonyms();
    };
};


/*
	��������� ���� ���������� "�����", ���� ����� ��� ���� 
	������������, �����  ��������� ���������� ������ "������" � ���� ����������
	+ ������ ����� �����
	+ ����� ����
	? ������ ����� ������ ����� (������ �����(����) ����� ������)

	��� �� ��� "�����": "����� �����" - ����� �� �������
*/


void CMAPost::Rule_SOROK() 
{	
	for (CLineIter it=m_Words.begin(); it !=  m_Words.end(); it++)
	{
        CPostLemWord& W = *it;
        if (!W.HasDes(ORLE)) continue;
        if (!W.HasPos(NUMERAL)) continue;
        W.SetHomonymsDel(false);
        for (int i =0; i < W.GetHomonymsCount(); i++)
        {
            CHomonym* pH = W.GetHomonym(i);
            if (pH->m_strLemma == "������")
            {
                CLineIter next_it = NextNotSpace(it);
                if (next_it == m_Words.end()) return;
                if (next_it->GetPoses() & ( (1<<NUMERAL) | (1<<NUMERAL_P)))
                    pH->m_bDelete = true;
            }
			if (pH->m_strLemma == "�����")
			{
				CLineIter next_it = NextNotSpace(it);
				if (next_it == m_Words.end() || !(next_it->GetPoses() & ( (1<<NUMERAL) | (1<<NUMERAL_P))))
					pH->m_bDelete = true;
			}
        }
        W.SafeDeleteMarkedHomonyms();
    }
};



/*
	��������� ���� ����������, ������� �� ���� �������� ������ ���������� �������, 
	���� ����� ����� �������� ������ � ���������  "����", �����  ���� ������ ���������.
	��������, ��� ����� "���" ����� ������ ������ "���" ("��������� ����� ���������"), �������  ������ ��� ���� �������  
	�������� �������.
*/


void CMAPost::Rule_Abbreviation() 
{	
    
	for (CLineIter it=m_Words.begin(); it !=  m_Words.end(); it++)
	{
	    CPostLemWord& W = *it;
        if (!W.HasDes(ORLE)) continue;
		//��      � ��,0 -> 
		for(int HomNo = 0 ; HomNo < W.GetHomonymsCount() ; HomNo++)
			if(W.GetHomonym(HomNo)->HasPos(NOUN) && W.GetHomonym(HomNo)->HasGrammem(rIndeclinable) && !W.GetHomonym(HomNo)->HasGrammem(rInitialism) ) 
			{
				CHomonym* pH = W.GetHomonym(HomNo);
				pH->m_CommonGramCode += pH->GetGramCodes();
				pH->SetGramCodes (m_pRusGramTab->GleicheAncode1(GenderNumber0, 
					m_pRusGramTab->GetGramCodes(NOUN, rAllCases | rAllNumbers | rAllGenders, 0), pH->GetGramCodes()));  //rAllNumbers, AnCodes, _QM(rSingular));
			}
        if (W.HasDes(OUp)) continue;

		for(int HomNo = 0 ; HomNo < W.GetHomonymsCount(); HomNo++)
			if( W.GetHomonym(HomNo)->HasGrammem(rInitialism) )
				if (W.GetHomonymsCount() > 1 && W.GetHomonym(HomNo)->m_strLemma == W.m_strUpperWord) //����� 50 ����
					W.EraseHomonym(HomNo);
				else
				{
					CHomonym* pH = W.GetHomonym(HomNo);
					vector<CFormInfo> Paradigms;
					CFormInfo P;
					string AnCodes;
					m_pRusLemmatizer->CreateParadigmFromID(pH->m_lPradigmID,  P);
					pH->m_CommonGramCode += pH->GetGramCodes();
					AnCodes = pH->GetGramCodes();
					pH->SetGramCodes( "" );
					string xx = P.GetAncode(0);
					for (long k=0; k < P.GetCount(); k++)
						if( m_pRusGramTab->GleicheAncode1(0, pH->m_CommonGramCode + pH->GetGramCodes(), P.GetAncode(k)) == ""
							&& (m_pRusGramTab->GetAllGrammems(P.GetAncode(k).c_str()) & m_pRusGramTab->GetAllGrammems(AnCodes.c_str())) == m_pRusGramTab->GetAllGrammems(P.GetAncode(k).c_str())
							&& m_pRusGramTab->GetPartOfSpeech(P.GetAncode(k).c_str()) == m_pRusGramTab->GetPartOfSpeech(AnCodes.c_str())
							)
							pH->SetGramCodes( pH->GetGramCodes() + P.GetAncode(k) );
				}
	};
};


/*
	��������� ����  ������������������ ���� "��-����������", "��-���������", ("��" + ����� ���������������)
	���������� �� � ���� �����, ���� ��� ����� �� ������� ����������,
	�������, ��� ��� �������.
*/


void CMAPost::Rule_AdverbFromAdjectives() 
{	
   	for (CLineIter it=m_Words.begin(); it !=  m_Words.end(); it++)
	{
	    CPostLemWord& W = *it;
        if (!W.HasDes(ORLE)) continue;
        if (!W.m_bPredicted)    continue;
        if (W.m_strUpperWord.length() < 3) continue;
        if (W.m_strUpperWord.substr(0,3) != "��-") continue;
        if (!HasParadigmOfFormAndPoses(W.m_strUpperWord.substr(3), (1<<ADJ_FULL)|(1<<ADJ_SHORT)))
				continue;
        W.DeleteAllHomonyms();
        CHomonym* pNew = W.AddNewHomonym();
		pNew->m_strLemma = it->m_strUpperWord;
        pNew->SetMorphUnknown();
        string NewGramCode;
        m_pRusGramTab->GetGramCodeByGrammemsAndPartofSpeechIfCan (ADV, 0, NewGramCode);
        pNew->SetGramCodes( NewGramCode );
        pNew->InitAncodePattern();
    }

};



void CMAPost::SaveToFile(string s)
{
	/*FILE * fp = fopen (s.c_str(), "w");
	assert (fp);
	if (!fp) return;
	for (CLineIter it=m_Words.begin(); it !=  m_Words.end(); it++)
	{
        fprintf (fp, "%s\n", it->GetPlmStr()GetStr().c_str());
	};
	fclose(fp);*/

};


/*
	���� �����  ������ �� ������� ����, � ���� �������� � ���������  �����
	����� ������� ����� �������������
*/
void CMAPost::Rule_FilterProperName()
{
   	for (CLineIter it=m_Words.begin(); it !=  m_Words.end(); it++)
	{
	    CPostLemWord& W = *it;
        if (!W.HasDes(OLw)) continue;
        for (int i =0; i < W.GetHomonymsCount(); i++)
            W.GetHomonym(i)->m_bDelete = ((W.GetHomonym(i)->m_iGrammems|W.GetHomonym(i)->m_TypeGrammems) & (_QM(rSurName) | _QM(rName) | _QM(rPatronymic) | _QM(rToponym))) > 0;
        W.SafeDeleteMarkedHomonyms();
	}
}

/*
	���� �����  �������� ���������, ����� ��� ����� ����� ������,
	�������� �������� -> ����
	��������� �������� ����� "����" �� ���������� �����  ��������.
    ��������, ���������� -> ��������
	          ���������  -> ��������
*/
void CMAPost::Rule_ChangePatronymicLemmas()
{

	for (CLineIter it=m_Words.begin(); it !=  m_Words.end(); it++)
	{
        CPostLemWord& W = *it;
        if (!W.HasDes(ORLE)) continue;
        int HomNo = W.GetHomonymByGrammem(rPatronymic);
        if (HomNo == -1) continue;
        CHomonym* pH = W.GetHomonym(HomNo);

		vector<CFormInfo> Paradigms;
		string Word = W.m_strWord;
		m_pRusLemmatizer->CreateParadigmCollection(false, Word, true, true, Paradigms);
		for (long k=0; k < Paradigms.size(); k++)
            if (Paradigms[k].GetSrcAncode() == pH->GetGramCodes())
			{
				for (size_t j=0; j<Paradigms[k].GetCount(); j++)
				{
					QWORD g;
					m_pRusGramTab->GetGrammems(Paradigms[k].GetAncode(j).c_str(), g);
					if	(		(g &  _QM(rPatronymic))
							&&  (g & _QM(rNominativ)) 
							&&  (g & _QM(rSingular)) 
							&&	(rAllGenders & g & pH->m_iGrammems)
						)
					{
                        pH->m_strLemma = Paradigms[k].GetWordForm(j);
						break;
					}
				};
				break;

			};


	}

};


void CMAPost::SolveAmbiguityUsingRuleForTwoPredicates(CLineIter start, CLineIter end)
{
    for (CLineIter root=start; root !=  end; root++)
	{
        CPostLemWord& RootWord = *root;
        if (RootWord.GetHomonymsCount() != 1 || !m_pRusGramTab->IsStrongClauseRoot(RootWord.GetHomonym(0)->m_iPoses)) continue;

        bool bHasAnalytical  = false;
        for (CLineIter it=start; it !=  end; it++)
        {
            CPostLemWord& W = *it;
            // �������������  ����� ��� �� �������, �� �� �����  ����� ������� ������� � �����. ��������
            bHasAnalytical = bHasAnalytical || W.HasAnalyticalBe(  ) ;
        }

        for (CLineIter it=start; it !=  end; it++)
        {
            CPostLemWord& W = *it;
            if (W.HasAnalyticalBe(  ) || it == root) continue;
            
            if (W.GetHomonymsCount()> 1 && m_pRusGramTab->IsStrongClauseRoot(W.GetPoses()))
            {
                W.SetHomonymsDel(false);
                for (int i =0; i < W.GetHomonymsCount(); i++)
                {
                    CHomonym* pH = W.GetHomonym(i);
                    if (bHasAnalytical)
                        if  (	     pH->HasPos(PARTICIPLE_SHORT )
                                ||	 pH->HasPos(ADJ_SHORT )
                                ||	 pH->HasPos(PREDK) 
					        )
                            continue;
                        
                    pH->m_bDelete = m_pRusGramTab->IsStrongClauseRoot(pH->m_iPoses);
                }
                W.SafeDeleteMarkedHomonyms();
            }
        }
        break;
    }
}

void CMAPost::Rule_TwoPredicates()
{
    CLineIter start = m_Words.begin();
    size_t  PeriodSize = 0;
   	for (CLineIter it=m_Words.begin(); it !=  m_Words.end(); it++)
	{
	    CPostLemWord& W = *it;
        if (W.HasDes(OSentEnd) || W.HasDes(OPun) || W.m_strUpperWord == "�" || W.m_strUpperWord == "���")
        {
            CLineIter end_it = it;
            if (W.HasDes(OSentEnd))
                end_it++;
            SolveAmbiguityUsingRuleForTwoPredicates(start, end_it);
            start = it;
            PeriodSize = 0;
        }
        if (PeriodSize > 10)
            start++;
        else
            PeriodSize++;
    }
    if (PeriodSize > 1)
        SolveAmbiguityUsingRuleForTwoPredicates (start, m_Words.end());
}



bool CMAPost::FilterOnePostLemWord(CPostLemWord& W, WORD tagid1, WORD tagid2) const
{
    vector<CTag> Tags =  m_TrigramModel.m_TagSet.DecipherTagStr( m_TrigramModel.m_RegisteredTags[tagid1], m_pRusGramTab);
    if (tagid2 != UnknownTag)
    {
        vector<CTag> Tags2 =  m_TrigramModel.m_TagSet.DecipherTagStr( m_TrigramModel.m_RegisteredTags[tagid2], m_pRusGramTab);
        Tags.insert(Tags.end(), Tags2.begin(), Tags2.end());
    }

    set<string> Lemmas;
    for (int i =0; i < W.GetHomonymsCount(); i++)
    {
        CHomonym* pH = W.GetHomonym(i);
        if (m_TrigramModel.FindGramTabLineInTags(Tags, pH->m_iPoses, pH->m_iGrammems  | pH->m_TypeGrammems))
            Lemmas.insert(pH->m_strLemma);
        pH->m_bDelete = !m_TrigramModel.FindGramTabLineInTags(Tags, pH->m_iPoses, pH->m_iGrammems  | pH->m_TypeGrammems);
    }
    for (int i =0; i < W.GetHomonymsCount(); i++)
    {
        CHomonym* pH = W.GetHomonym(i);
        pH->m_bDelete |= Lemmas.find(pH->m_strLemma) == Lemmas.end();
    }
    W.SafeDeleteMarkedHomonyms();
    return true;
}

bool CMAPost::FilterPostMorphWords() 
{
	vector<string> tokens;
	vector<CWordIntepretation> tags;
    list<CPostLemWord>::iterator sent_start = m_Words.begin();
    for (list<CPostLemWord>::iterator it=m_Words.begin(); it !=  m_Words.end(); )	
    {
        if (it->HasDes(OSentEnd ) || (tokens.size() > 150))
        {
            tokens.push_back( it->m_strWord );
	    	if (!m_TrigramModel.viterbi(tokens, tags))
		    	return false;
            size_t WordNo = 0;
            it++;
            for (list<CPostLemWord>::iterator it2 = sent_start; it2 !=  it; it2++)	
            {
                assert (WordNo < tags.size());
                FilterOnePostLemWord (*it2, tags[WordNo].m_TagId1, tags[WordNo].m_TagId2);
                WordNo++;
            }


            tokens.clear();
            tags.clear();
            sent_start = it;
        }
        else
        {
            tokens.push_back( it->m_strWord );
            it++;
        }

	}
	return true;
}
