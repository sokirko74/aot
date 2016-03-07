// Morphan.cpp : Implementation of CMorphan
#include "../common/string_socket.h"
#include "Morphan.h"
#include "../common/MorphologyHolder.h"


const int ParagigmGroupsCount = 45;
const string  ParagigmGroups [ParagigmGroupsCount] = {
	"� ��,��",
	"� ��,��",
	"� ��,��",
	"� ��",
	"� �����",
	"� ��",
	"� ��",
	"� ���",
	"� ���",
	"� ���",
	"� ���",
	"��������� ���,��,���",
	"��������� ���,��,���",
	"��������� ���,��,���",
	"��������� ���,��,���",
	"��������� ���,��,���",
	"��������� ���,��,���",
	"��������� ���,��,���",
	"��������� ���,��,���",
	"��������� ���,��,���",
	"��������� ���,��,���",
	"��������� ���,��,���",
	"��������� ���,��,���",
	"��������� ���,��,���",
	"��������� ���,��,���",
	"��������� ���,��,���",
	"��������� ���,��,���",
	"���� ��",
	"���� ��",
	"���� ��",
	"����-� ��,��",
	"����-� ��,��",
	"����-� ��,��",
	"����-� ��",
	"��-� ��,��",
	"��-� ��,��",
	"��-� ��,��",
	"��-� ��",
	"�� ��",
	"�� ��",
	"�� ��,��",
	"�� ��,��",
	"�� ��,��",
	"VBE sg",
	"VBE pl"
};

/////////////////////////////////////////////////////////////////////////////
// CMorphan

string GetGramInfoStr (string GramInfo, const CMorphologyHolder* Holder)
{
	const CAgramtab* pGramtab = Holder->m_pGramTab;
	assert (!GramInfo.empty());
	int POS = pGramtab->GetPartOfSpeech(GramInfo.c_str());
	string Result;
	Result += pGramtab->GetPartOfSpeechStr(POS);
	Result += " ";
	QWORD grammems = pGramtab->GetAllGrammems(GramInfo.c_str());
	Result += pGramtab->GrammemsToStr(grammems);
	Result += " ";
	return Result;

};

string GetStringByParadigm (const CFormInfo*  piParadigm, const CMorphologyHolder* Holder)
{
	string Result = "<TR> <TD>";	
	Result +=  piParadigm->m_bFound ? "+" : "-";
	Result += "</TD><TD>";	

	string TypeAncode = piParadigm->GetCommonAncode();
	if (TypeAncode.empty())
	{
		Result += " Main ";
	}
	else
	{
		const CAgramtab* pGramtab = Holder->m_pGramTab;
		try {
			QWORD grammems = pGramtab->GetAllGrammems(TypeAncode.c_str());
			Result += " "+pGramtab->GrammemsToStr(grammems)+ " ";
		}
		catch (...)
		{
			Result += " an exception occurred while getting Common Ancode";
		};

	};

	Result += "</TD><TD>";	
	Result +=  piParadigm->GetWordForm(0);
	Result += "</TD><TD>";	
	string GramInfo;
	try
	{
		GramInfo = piParadigm->GetSrcAncode();
	} catch(...)
	{
		GramInfo = piParadigm->GetAncode(0);
	}

	 if (!GramInfo.empty())
		Result += GetGramInfoStr(GramInfo, Holder);

	Result += "</TD><TD>";	

	if (!piParadigm->m_bFound)
	{
		Result += piParadigm->GetSrcNorm();
	}
	Result += "</TD></TR>";	
	return Result;
}


inline  bool IsUpper(int x, MorphLanguageEnum Langua)     
{
	return is_upper_alpha(x, Langua);
};



bool GetParadigmCollection(string WordForm, vector<CFormInfo>&	Paradigms, const CMorphologyHolder* Holder)
{
	if (WordForm.length() == 0) 
	{
		return false;
	};

	try 
	{
		if (Holder->m_pLemmatizer == 0) return false;
		Holder->m_pLemmatizer->CreateParadigmCollection(false, WordForm, IsUpper((unsigned char)WordForm[0], Holder->m_CurrentLanguage), true, Paradigms);
	}
	catch (...)
	{
		return false;;
	};
	return true;


};

struct CFormAndGrammems {
	string m_Form;
	string m_POS;
	QWORD  m_Grammems;

	bool operator  < (const CFormAndGrammems& X) const
	{
		return m_POS < X.m_POS;

	};

};

struct CFormGroup {
	QWORD			   m_IntersectGrammems;
	vector<int>    m_FormNos;
};

int GetWidePOS (BYTE POS, QWORD Grammems, MorphLanguageEnum Langua)
{
	 return POS;

};


vector<CFormGroup> GetParadigmByGroups (const vector<CFormAndGrammems>& Forms, const CMorphologyHolder* Holder, QWORD& CommonGrammems)
{
	const CAgramtab* pGramtab = Holder->m_pGramTab;
	vector<CFormGroup> Results;
	vector<bool> IncludedVector;
	int i=0;

	for (; i < Forms.size(); i++) IncludedVector.push_back(false);

	for (long GroupNo=0; GroupNo < ParagigmGroupsCount; GroupNo++)
	{
		BYTE POS;
		QWORD Grammems = 0;
		if (!pGramtab->ProcessPOSAndGrammems(ParagigmGroups[GroupNo].c_str(), POS, Grammems)) continue;;
		string strPOS  = pGramtab->GetPartOfSpeechStr(GetWidePOS(POS, Grammems, Holder->m_CurrentLanguage));
		CFormGroup F;
		F.m_IntersectGrammems =   GetMaxQWORD();
		for (long i=0; i < Forms.size(); i++)
		  if (!IncludedVector[i])
			if (Forms[i].m_POS == strPOS)
				if ( (Grammems & Forms[i].m_Grammems) == Grammems)
				{
					int k =0;
					for (; k < F.m_FormNos.size(); k++)
						if (Forms[F.m_FormNos[k]].m_Grammems >= Forms[i].m_Grammems)
							break;
					F.m_FormNos.insert(F.m_FormNos.begin() + k, i);
					IncludedVector[i] = true;
					F.m_IntersectGrammems &=  Forms[i].m_Grammems;
				};

		if (F.m_FormNos.size() > 0)
	  	 Results.push_back(F);

	};

	 CFormGroup LastGroup;
	 LastGroup.m_IntersectGrammems = 0;
	 for (i = 0; i < Forms.size(); i++)
		if(!IncludedVector[i])
			LastGroup.m_FormNos.push_back(i);

	 if (LastGroup.m_FormNos.size() > 0)
		Results.push_back(LastGroup);

	CommonGrammems = GetMaxQWORD();
	for (i=0; i < Forms.size(); i++)

		CommonGrammems &=  Forms[i].m_Grammems;

	

	return Results;	

};

string GetInterfacePOS (string POS)
{
	Trim(POS);
	if (POS == "�") return "������ �����";
	if (POS == "�") return "���������������";
	if (POS == "�") return "��������������";
	if (POS == "��") return "�����������";
	if (POS == "��-�") return "�����������-��������������";
	if (POS == "��-�����") return "�����������-����������";
	if (POS == "����") return "������������";
	if (POS == "����-�") return "���������� ������������";
	if (POS == "�") return "�������";
	if (POS == "�����") return "����������";
	if (POS == "�����") return "�������";
	if (POS == "����") return "����������";
	if (POS == "�����") return "������� �����";
	if (POS == "����") return "�������";
	if (POS == "��_����") return "������� ��������������";
	if (POS == "��_���������") return "������� ���������";
	return POS;
};


string GetParadigmFromDictionary (CFormInfo* piParadigm, const CMorphologyHolder* Holder)
{
	// �������� ����� ����
	string Result;
	const CAgramtab* pGramtab = Holder->m_pGramTab;


	
	// �������� ��� ����� � �������� � ������ FormAndGrammems
	vector<CFormAndGrammems> FormAndGrammems;
	for (int j = 0; j < piParadigm->GetCount(); j++)
	{
		string GramInfo = piParadigm->GetAncode(j);
		for (long i=0; i < GramInfo.length(); i+=2)
		{
			CFormAndGrammems F;
			F.m_Form = piParadigm->GetWordForm(j);
   			BYTE AccentedCharNo = piParadigm->GetAccentedVowel(j);
			if (AccentedCharNo != 255)
                F.m_Form.insert(AccentedCharNo+1, "'");
            F.m_Grammems = pGramtab->GetAllGrammems(GramInfo.substr(i,2).c_str());
			BYTE POS = pGramtab->GetPartOfSpeech(GramInfo.substr(i,2).c_str());
			F.m_POS  = pGramtab->GetPartOfSpeechStr(GetWidePOS(POS, F.m_Grammems, Holder->m_CurrentLanguage));

			// ��� ����������
			if  (F.m_POS == "���������")
				F.m_POS.insert(0, " ");
			  

			FormAndGrammems.push_back(F);
		};
	};
	sort (FormAndGrammems.begin(), FormAndGrammems.end());
	for (int FormNo=0; FormNo < FormAndGrammems.size(); )
	{
		int EndFormNo = FormNo+1;
		for (; EndFormNo< FormAndGrammems.size(); EndFormNo++)
			if (FormAndGrammems[FormNo].m_POS != FormAndGrammems[EndFormNo].m_POS)
				break;
		vector<CFormAndGrammems> FormAndGrammemsPart;
		FormAndGrammemsPart.insert (FormAndGrammemsPart.begin(), FormAndGrammems.begin()+FormNo, FormAndGrammems.begin()+EndFormNo);
		QWORD CommonGrammems;
		vector<CFormGroup> FormGroups = GetParadigmByGroups(FormAndGrammemsPart, Holder, CommonGrammems);

		Result += "<TABLE BORDER=5>";
		Result +=  "<TR><TD><B>";
		// �������� ����� ����
		Result += GetInterfacePOS(FormAndGrammems[FormNo].m_POS);
		// �������� ����� ��������
		Result += " ";
		if (CommonGrammems > 0)
			Result += pGramtab->GrammemsToStr( CommonGrammems );
		Result += "</TD></TR></B>";

		

		// �������������
		for (int j = 0; j < FormGroups.size(); j++)
		{
			Result += "<TR><TD><B>";
			Result += pGramtab->GrammemsToStr(FormGroups[j].m_IntersectGrammems & ~CommonGrammems);
			if (Result[Result.length() - 1] == ',') Result[Result.length() - 1] = ' ';
			Result += "</B></TD></TR>\n";
			for (long i=0; i < FormGroups[j].m_FormNos.size(); i++)
			{
				const CFormAndGrammems& F = FormAndGrammems[FormGroups[j].m_FormNos[i] + FormNo];
				Result += "<TR><TD>&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp";
				Result += F.m_Form;
				Result += "</TD><TD><B>";
				// �������� ������ �� ��������, ������� �� ����� � �����  �����������
				Result += pGramtab->GrammemsToStr( F.m_Grammems & (~(FormGroups[j].m_IntersectGrammems)) & ~CommonGrammems );
				if (Result[Result.length() - 1] == ',') Result[Result.length() - 1] = ' ';
				Result += "\n";
				Result += "</B></TD></TR>";
			};
		};
		Result += "\n";
		Result += "</TABLE>";
		FormNo = EndFormNo;
	};

	return Result;
};


string Lemmatize(string WordForm, const CMorphologyHolder* Holder, int& LemmasCount)
{
	vector<CFormInfo>	Paradigms;
	if (!GetParadigmCollection(WordForm, Paradigms, Holder))
	{
		return "Not found";
	};

	LemmasCount = Paradigms.size();

	if (!Paradigms.empty())
	{
		string strResult = "<TABLE BORDER=1 CELLPADDING=10 CELLSPACING=0>";
		strResult += "<TR><TD>Found</TD><TD>Dict ID</TD><TD>Lemma</TD><TD>Grammems</TD></TR>";
		for (long i=0; i < Paradigms.size(); i++)
		{
			strResult += GetStringByParadigm(& (Paradigms[i]), Holder);
		};
		strResult += "</TABLE>";
		return strResult;
	}
	else
	{
		return " <br> not found! <br>";
	}

	
}


string GetParadigm(string WordForm, int LemmaNo, const CMorphologyHolder* Holder)
{
	vector<CFormInfo>	piParadigmCollection;
	if (   (!GetParadigmCollection(WordForm, piParadigmCollection, Holder))
		|| (LemmaNo >= piParadigmCollection.size())
	   )
	{
		return "Not found";
	};
	return  GetParadigmFromDictionary(&piParadigmCollection[LemmaNo], Holder);
}



