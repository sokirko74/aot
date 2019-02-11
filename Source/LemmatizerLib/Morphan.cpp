#include "Morphan.h"
#include "MorphologyHolder.h"

#include <common/json.h>


const int ParagigmGroupsCount = 45;
const string  ParagigmGroups[ParagigmGroupsCount] = {
	"Ï åä,ìğ",
	"Ï åä,æğ",
	"Ï åä,ñğ",
	"Ï ìí",
	"Ï ñğàâí",
	"Ñ åä",
	"Ñ ìí",
	"Ã íñò",
	"Ã ïğø",
	"Ã áóä",
	"Ã ïâë",
	"ÏĞÈ×ÀÑÒÈÅ íñò,ìğ,ñòğ",
	"ÏĞÈ×ÀÑÒÈÅ íñò,æğ,ñòğ",
	"ÏĞÈ×ÀÑÒÈÅ íñò,ñğ,ñòğ",
	"ÏĞÈ×ÀÑÒÈÅ íñò,ìí,ñòğ",
	"ÏĞÈ×ÀÑÒÈÅ ïğø,ìğ,ñòğ",
	"ÏĞÈ×ÀÑÒÈÅ ïğø,æğ,ñòğ",
	"ÏĞÈ×ÀÑÒÈÅ ïğø,ñğ,ñòğ",
	"ÏĞÈ×ÀÑÒÈÅ ïğø,ìí,ñòğ",
	"ÏĞÈ×ÀÑÒÈÅ íñò,ìğ,äñò",
	"ÏĞÈ×ÀÑÒÈÅ íñò,æğ,äñò",
	"ÏĞÈ×ÀÑÒÈÅ íñò,ñğ,äñò",
	"ÏĞÈ×ÀÑÒÈÅ íñò,ìí,äñò",
	"ÏĞÈ×ÀÑÒÈÅ ïğø,ìğ,äñò",
	"ÏĞÈ×ÀÑÒÈÅ ïğø,æğ,äñò",
	"ÏĞÈ×ÀÑÒÈÅ ïğø,ñğ,äñò",
	"ÏĞÈ×ÀÑÒÈÅ ïğø,ìí,äñò",
	"×ÈÑË ìğ",
	"×ÈÑË æğ",
	"×ÈÑË ñğ",
	"×ÈÑË-Ï åä,ìğ",
	"×ÈÑË-Ï åä,æğ",
	"×ÈÑË-Ï åä,ñğ",
	"×ÈÑË-Ï ìí",
	"ÌÑ-Ï åä,ìğ",
	"ÌÑ-Ï åä,æğ",
	"ÌÑ-Ï åä,ñğ",
	"ÌÑ-Ï ìí",
	"ÌÑ åä",
	"ÌÑ ìí",
	"ÌÑ åä,ìğ",
	"ÌÑ åä,æğ",
	"ÌÑ åä,ñğ",
	"VBE sg",
	"VBE pl"
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

int GetWidePOS(BYTE POS, QWORD Grammems, MorphLanguageEnum Langua)
{
	return POS;

};

string&  TrimCommaRight(string& str)
{
	if (str.size() == 0) return str;
	size_t i = str.find_last_not_of(",");
	str.erase(i + 1);
	return str;
};


string GetGramInfoStr(string GramInfo, const CMorphologyHolder* Holder)
{
	const CAgramtab* pGramtab = Holder->m_pGramTab;
	assert(!GramInfo.empty());
	int POS = pGramtab->GetPartOfSpeech(GramInfo.c_str());
	string Result;
	Result += pGramtab->GetPartOfSpeechStr(POS);
	Result += " ";
	QWORD grammems = pGramtab->GetAllGrammems(GramInfo.c_str());
	Result += pGramtab->GrammemsToStr(grammems);
	TrimCommaRight(Result);
	return Result;

};

vector<CFormGroup> GetParadigmByGroups(const vector<CFormAndGrammems>& Forms, const CMorphologyHolder* Holder, QWORD& CommonGrammems)
{
	vector<CFormGroup> Results;
	vector<bool> IncludedVector;
	int i = 0;
	const CAgramtab* pGramtab = Holder->m_pGramTab;

	for (; i < Forms.size(); i++) IncludedVector.push_back(false);

	for (long GroupNo = 0; GroupNo < ParagigmGroupsCount; GroupNo++)
	{
		BYTE POS;
		QWORD Grammems = 0;
		if (!pGramtab->ProcessPOSAndGrammems(ParagigmGroups[GroupNo].c_str(), POS, Grammems)) continue;;
		string strPOS = pGramtab->GetPartOfSpeechStr(GetWidePOS(POS, Grammems, Holder->m_CurrentLanguage));
		CFormGroup F;
		F.m_IntersectGrammems = GetMaxQWORD();
		for (long i = 0; i < Forms.size(); i++)
			if (!IncludedVector[i])
				if (Forms[i].m_POS == strPOS)
					if ((Grammems & Forms[i].m_Grammems) == Grammems)
					{
						int k = 0;
						for (; k < F.m_FormNos.size(); k++)
							if (Forms[F.m_FormNos[k]].m_Grammems >= Forms[i].m_Grammems)
								break;
						F.m_FormNos.insert(F.m_FormNos.begin() + k, i);
						IncludedVector[i] = true;
						F.m_IntersectGrammems &= Forms[i].m_Grammems;
					};

		if (F.m_FormNos.size() > 0)
			Results.push_back(F);

	};

	CFormGroup LastGroup;
	LastGroup.m_IntersectGrammems = 0;
	for (i = 0; i < Forms.size(); i++)
		if (!IncludedVector[i])
			LastGroup.m_FormNos.push_back(i);

	if (LastGroup.m_FormNos.size() > 0)
		Results.push_back(LastGroup);

	CommonGrammems = GetMaxQWORD();
	for (i = 0; i < Forms.size(); i++)

		CommonGrammems &= Forms[i].m_Grammems;



	return Results;

};


vector<CFormGroup>  BuildInterfaceParadigmPart(const CMorphologyHolder* Holder, const vector<CFormAndGrammems> FormAndGrammems, int& FormNo, QWORD& commonGrammems) {
	int EndFormNo = FormNo + 1;
	for (; EndFormNo < FormAndGrammems.size(); EndFormNo++)
		if (FormAndGrammems[FormNo].m_POS != FormAndGrammems[EndFormNo].m_POS)
			break;
	vector<CFormAndGrammems> FormAndGrammemsPart;
	FormAndGrammemsPart.insert(FormAndGrammemsPart.begin(), FormAndGrammems.begin() + FormNo, FormAndGrammems.begin() + EndFormNo);
	FormNo = EndFormNo;
	return GetParadigmByGroups(FormAndGrammemsPart, Holder, commonGrammems);
};


vector<CFormAndGrammems> BuildFormAndGrammems(const CMorphologyHolder* Holder, const CFormInfo* piParadigm) {
	const CAgramtab* pGramtab = Holder->m_pGramTab;
	// ïîëó÷àåì âñå ôîğìû è ãğàììåìû â ìàññèâ FormAndGrammems
	vector<CFormAndGrammems> FormAndGrammems;
	for (int j = 0; j < piParadigm->GetCount(); j++)
	{
		string GramInfo = piParadigm->GetAncode(j);
		for (long i = 0; i < GramInfo.length(); i += 2)
		{
			CFormAndGrammems F;
			F.m_Form = piParadigm->GetWordForm(j);
			BYTE AccentedCharNo = piParadigm->GetAccentedVowel(j);
			if (AccentedCharNo != 255)
				F.m_Form.insert(AccentedCharNo + 1, "'");
			F.m_Grammems = pGramtab->GetAllGrammems(GramInfo.substr(i, 2).c_str());
			BYTE POS = pGramtab->GetPartOfSpeech(GramInfo.substr(i, 2).c_str());
			F.m_POS = pGramtab->GetPartOfSpeechStr(GetWidePOS(POS, F.m_Grammems, Holder->m_CurrentLanguage));

			// äëÿ ñîğòèğîâêè
			if (F.m_POS == "ÈÍÔÈÍÈÒÈÂ")
				F.m_POS.insert(0, " ");


			FormAndGrammems.push_back(F);
		};
	};
	sort(FormAndGrammems.begin(), FormAndGrammems.end());
	return FormAndGrammems;
}


static string GetInterfacePOS(string POS)
{
	Trim(POS);
	if (POS == "Ã") return "ËÈ×ÍÀß ÔÎĞÌÀ";
	if (POS == "Ñ") return "ÑÓÙÅÑÒÂÈÒÅËÜÍÎÅ";
	if (POS == "Ï") return "ÏĞÈËÀÃÀÒÅËÜÍÎÅ";
	if (POS == "ÌÑ") return "ÌÅÑÒÎÈÌÅÍÈÅ";
	if (POS == "ÌÑ-Ï") return "ÌÅÑÒÎÈÌÅÍÈÅ-ÏĞÈËÀÃÀÒÅËÜÍÎÅ";
	if (POS == "ÌÑ-ÏĞÅÄÊ") return "ÌÅÑÒÎÈÌÅÍÈÅ-ÏĞÅÄÈÊÀÒÈÂ";
	if (POS == "×ÈÑË") return "×ÈÑËÈÒÅËÜÍÎÅ";
	if (POS == "×ÈÑË-Ï") return "ÏÎĞßÄÊÎÂÎÅ ×ÈÑËÈÒÅËÜÍÎÅ";
	if (POS == "Í") return "ÍÀĞÅ×ÈÅ";
	if (POS == "ÏĞÅÄÊ") return "ÏĞÅÄÈÊÀÒÈÂ";
	if (POS == "ÏĞÅÄË") return "ÏĞÅÄËÎÃ";
	if (POS == "ÌÅÆÄ") return "ÌÅÆÄÎÌÅÒÈÅ";
	if (POS == "ÂÂÎÄÍ") return "ÂÂÎÄÍÎÅ ÑËÎÂÎ";
	if (POS == "×ÀÑÒ") return "×ÀÑÒÈÖÀ";
	if (POS == "ÊĞ_ÏĞÈË") return "ÊĞÀÒÊÎÅ ÏĞÈËÀÃÀÒÅËÜÍÎÅ";
	if (POS == "ÊĞ_ÏĞÈ×ÀÑÒÈÅ") return "ÊĞÀÒÊÎÅ ÏĞÈ×ÀÑÒÈÅ";
	return POS;
};

nlohmann::json  GetParadigmFromDictionary(const CFormInfo* piParadigm, const CMorphologyHolder* Holder) {
	const CAgramtab* pGramtab = Holder->m_pGramTab;
	const vector<CFormAndGrammems> FormAndGrammems = BuildFormAndGrammems(Holder, piParadigm);
	nlohmann::json result = nlohmann::json::array();
	int FormNo = 0;
	while (FormNo < FormAndGrammems.size()) {
		int saveFormNo = FormNo;
		QWORD commonGrammems;
		const vector<CFormGroup> FormGroups = BuildInterfaceParadigmPart(Holder, FormAndGrammems, FormNo, commonGrammems);
		assert(FormNo > saveFormNo);
		auto prdPart = nlohmann::json::object();
		string pos = GetInterfacePOS(FormAndGrammems[saveFormNo].m_POS);
		if (commonGrammems > 0)
			pos += string(" ") + pGramtab->GrammemsToStr(commonGrammems);
		prdPart["pos"] = TrimCommaRight(pos);

		prdPart["formsGroups"] = nlohmann::json::array();
		for (auto fg : FormGroups)	{
			auto subg = nlohmann::json::object();
            string grm = pGramtab->GrammemsToStr(fg.m_IntersectGrammems & ~commonGrammems);
			subg["grm"] = TrimCommaRight(grm);
			subg["forms"] = nlohmann::json::array();
			for (auto formNo : fg.m_FormNos) {
				auto& f = FormAndGrammems[formNo + saveFormNo];
                string grm = pGramtab->GrammemsToStr(f.m_Grammems & ~(fg.m_IntersectGrammems | commonGrammems));
				subg["forms"].push_back(
					{ {"f", f.m_Form},
					{"grm", TrimCommaRight(grm)} });
			};
			prdPart["formsGroups"].push_back(subg);
		};
		result.push_back(prdPart);
	};
	return result;
};


nlohmann::json GetStringByParadigmJson(const CFormInfo*  piParadigm, const CMorphologyHolder* Holder, bool withParadigm)
{
	auto result = nlohmann::json::object();
	result["found"] = piParadigm->m_bFound;
	
	string typeAncode = piParadigm->GetCommonAncode();
	string commonGrammems;
	if (!typeAncode.empty()) {
		const CAgramtab* pGramtab = Holder->m_pGramTab;
		try {
			commonGrammems = pGramtab->GrammemsToStr(pGramtab->GetAllGrammems(typeAncode.c_str()));
		}
		catch (...)	{
			throw CExpc (" an exception occurred while getting Common Ancode");
		};

	};
	result["commonGrammems"] = TrimCommaRight(commonGrammems);
	result["wordForm"] = piParadigm->GetWordForm(0);
	if (!piParadigm->m_bFound)	{
		result["srcNorm"] = piParadigm->GetSrcNorm();

	}
	string GramInfo;
	try	{
		GramInfo = piParadigm->GetSrcAncode();
	}
	catch (...)	{
		GramInfo = piParadigm->GetAncode(0);
	}
	result["morphInfo"] = GetGramInfoStr(GramInfo, Holder);

	if (withParadigm) {
		result["paradigm"] = GetParadigmFromDictionary(piParadigm, Holder);
	}
	return result;
}


inline  bool IsUpper(int x, MorphLanguageEnum Langua)
{
	return is_upper_alpha(x, Langua);
};



bool GetParadigmCollection(string WordForm, vector<CFormInfo>&	Paradigms, const CMorphologyHolder* Holder)
{
	if (WordForm.length() == 0)	{
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

string LemmatizeJson(string WordForm, const CMorphologyHolder* Holder, bool withParadigm) {
	vector<CFormInfo>	Paradigms;
	if (!GetParadigmCollection(WordForm, Paradigms, Holder)) {
		return "[]";
	};

	nlohmann::json result = nlohmann::json::array();
	string strResult = "[";
	for (long i = 0; i < Paradigms.size(); i++) {
		result.push_back ( GetStringByParadigmJson(&(Paradigms[i]), Holder, withParadigm) );
	};
	ConvertToUtfRecursive(result, Holder->m_CurrentLanguage);
	return result.dump();
}



