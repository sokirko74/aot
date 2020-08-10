#include "Morphan.h"
#include "MorphologyHolder.h"

#include <common/json.h>


const int ParagigmGroupsCount = 45;
const string  ParagigmGroups[ParagigmGroupsCount] = {
	"П ед,мр",
	"П ед,жр",
	"П ед,ср",
	"П мн",
	"П сравн",
	"С ед",
	"С мн",
	"Г нст",
	"Г прш",
	"Г буд",
	"Г пвл",
	"ПРИЧАСТИЕ нст,мр,стр",
	"ПРИЧАСТИЕ нст,жр,стр",
	"ПРИЧАСТИЕ нст,ср,стр",
	"ПРИЧАСТИЕ нст,мн,стр",
	"ПРИЧАСТИЕ прш,мр,стр",
	"ПРИЧАСТИЕ прш,жр,стр",
	"ПРИЧАСТИЕ прш,ср,стр",
	"ПРИЧАСТИЕ прш,мн,стр",
	"ПРИЧАСТИЕ нст,мр,дст",
	"ПРИЧАСТИЕ нст,жр,дст",
	"ПРИЧАСТИЕ нст,ср,дст",
	"ПРИЧАСТИЕ нст,мн,дст",
	"ПРИЧАСТИЕ прш,мр,дст",
	"ПРИЧАСТИЕ прш,жр,дст",
	"ПРИЧАСТИЕ прш,ср,дст",
	"ПРИЧАСТИЕ прш,мн,дст",
	"ЧИСЛ мр",
	"ЧИСЛ жр",
	"ЧИСЛ ср",
	"ЧИСЛ-П ед,мр",
	"ЧИСЛ-П ед,жр",
	"ЧИСЛ-П ед,ср",
	"ЧИСЛ-П мн",
	"МС-П ед,мр",
	"МС-П ед,жр",
	"МС-П ед,ср",
	"МС-П мн",
	"МС ед",
	"МС мн",
	"МС ед,мр",
	"МС ед,жр",
	"МС ед,ср",
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
	// получаем все формы и граммемы в массив FormAndGrammems
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

			// для сортировки
			if (F.m_POS == "ИНФИНИТИВ")
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
	if (POS == "Г") return "ЛИЧНАЯ ФОРМА";
	if (POS == "С") return "СУЩЕСТВИТЕЛЬНОЕ";
	if (POS == "П") return "ПРИЛАГАТЕЛЬНОЕ";
	if (POS == "МС") return "МЕСТОИМЕНИЕ";
	if (POS == "МС-П") return "МЕСТОИМЕНИЕ-ПРИЛАГАТЕЛЬНОЕ";
	if (POS == "МС-ПРЕДК") return "МЕСТОИМЕНИЕ-ПРЕДИКАТИВ";
	if (POS == "ЧИСЛ") return "ЧИСЛИТЕЛЬНОЕ";
	if (POS == "ЧИСЛ-П") return "ПОРЯДКОВОЕ ЧИСЛИТЕЛЬНОЕ";
	if (POS == "Н") return "НАРЕЧИЕ";
	if (POS == "ПРЕДК") return "ПРЕДИКАТИВ";
	if (POS == "ПРЕДЛ") return "ПРЕДЛОГ";
	if (POS == "МЕЖД") return "МЕЖДОМЕТИЕ";
	if (POS == "ВВОДН") return "ВВОДНОЕ СЛОВО";
	if (POS == "ЧАСТ") return "ЧАСТИЦА";
	if (POS == "КР_ПРИЛ") return "КРАТКОЕ ПРИЛАГАТЕЛЬНОЕ";
	if (POS == "КР_ПРИЧАСТИЕ") return "КРАТКОЕ ПРИЧАСТИЕ";
	return POS;
};

nlohmann::json  GetParadigmFromDictionary(const CFormInfo* piParadigm, const CMorphologyHolder* Holder, bool sortForms) {
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
			if (sortForms) {
				sort(subg["forms"].begin(), subg["forms"].end());
			}
			prdPart["formsGroups"].push_back(subg);
		};
		result.push_back(prdPart);
	};
	return result;
};


nlohmann::json GetStringByParadigmJson(const CFormInfo*  piParadigm, const CMorphologyHolder* Holder, bool withParadigm, bool sortForms)
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
		result["paradigm"] = GetParadigmFromDictionary(piParadigm, Holder, sortForms);
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

string LemmatizeJson(string WordForm, const CMorphologyHolder* Holder, bool withParadigm, bool prettyJson, bool sortForms) {
	vector<CFormInfo>	Paradigms;
	if (!GetParadigmCollection(WordForm, Paradigms, Holder)) {
		return "[]";
	};

	nlohmann::json result = nlohmann::json::array();
	string strResult = "[";
	for (long i = 0; i < Paradigms.size(); i++) {
		result.push_back ( GetStringByParadigmJson(&(Paradigms[i]), Holder, withParadigm, sortForms) );
	};
	ConvertToUtfRecursive(result, Holder->m_CurrentLanguage);
	return result.dump(prettyJson?1:-1);
}



