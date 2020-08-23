#include "Morphan.h"
#include "MorphologyHolder.h"

#include "../common/json.h"


const int ParagigmGroupsCount = 45;
const std::string  ParagigmGroups[ParagigmGroupsCount] = {
	_R("П ед,мр"),
	_R("П ед,жр"),
	_R("П ед,ср"),
	_R("П мн"),
	_R("П сравн"),
	_R("С ед"),
	_R("С мн"),
	_R("Г нст"),
	_R("Г прш"),
	_R("Г буд"),
	_R("Г пвл"),
	_R("ПРИЧАСТИЕ нст,мр,стр"),
	_R("ПРИЧАСТИЕ нст,жр,стр"),
	_R("ПРИЧАСТИЕ нст,ср,стр"),
	_R("ПРИЧАСТИЕ нст,мн,стр"),
	_R("ПРИЧАСТИЕ прш,мр,стр"),
	_R("ПРИЧАСТИЕ прш,жр,стр"),
	_R("ПРИЧАСТИЕ прш,ср,стр"),
	_R("ПРИЧАСТИЕ прш,мн,стр"),
	_R("ПРИЧАСТИЕ нст,мр,дст"),
	_R("ПРИЧАСТИЕ нст,жр,дст"),
	_R("ПРИЧАСТИЕ нст,ср,дст"),
	_R("ПРИЧАСТИЕ нст,мн,дст"),
	_R("ПРИЧАСТИЕ прш,мр,дст"),
	_R("ПРИЧАСТИЕ прш,жр,дст"),
	_R("ПРИЧАСТИЕ прш,ср,дст"),
	_R("ПРИЧАСТИЕ прш,мн,дст"),
	_R("ЧИСЛ мр"),
	_R("ЧИСЛ жр"),
	_R("ЧИСЛ ср"),
	_R("ЧИСЛ-П ед,мр"),
	_R("ЧИСЛ-П ед,жр"),
	_R("ЧИСЛ-П ед,ср"),
	_R("ЧИСЛ-П мн"),
	_R("МС-П ед,мр"),
	_R("МС-П ед,жр"),
	_R("МС-П ед,ср"),
	_R("МС-П мн"),
	_R("МС ед"),
	_R("МС мн"),
	_R("МС ед,мр"),
	_R("МС ед,жр"),
	_R("МС ед,ср"),
	"VBE sg",
	"VBE pl"
};

struct CFormAndGrammems {
	std::string m_Form;
	std::string m_POS;
	QWORD  m_Grammems;

	bool operator  < (const CFormAndGrammems& X) const
	{
		return m_POS < X.m_POS;

	};

};

struct CFormGroup {
	QWORD			   m_IntersectGrammems;
	std::vector<int>    m_FormNos;
};

int GetWidePOS(BYTE POS, QWORD Grammems, MorphLanguageEnum Langua)
{
	return POS;

};

std::string&  TrimCommaRight(std::string& str)
{
	if (str.size() == 0) return str;
	size_t i = str.find_last_not_of(",");
	str.erase(i + 1);
	return str;
};


std::string GetGramInfoStr(std::string GramInfo, const CMorphologyHolder* Holder)
{
	const CAgramtab* pGramtab = Holder->m_pGramTab;
	assert(!GramInfo.empty());
	int POS = pGramtab->GetPartOfSpeech(GramInfo.c_str());
	std::string Result;
	Result += pGramtab->GetPartOfSpeechStr(POS);
	Result += " ";
	QWORD grammems = pGramtab->GetAllGrammems(GramInfo.c_str());
	Result += pGramtab->GrammemsToStr(grammems);
	TrimCommaRight(Result);
	return Result;

};

std::vector<CFormGroup> GetParadigmByGroups(const std::vector<CFormAndGrammems>& Forms, const CMorphologyHolder* Holder, QWORD& CommonGrammems)
{
	std::vector<CFormGroup> Results;
	std::vector<bool> IncludedVector;
	int i = 0;
	const CAgramtab* pGramtab = Holder->m_pGramTab;

	for (; i < Forms.size(); i++) IncludedVector.push_back(false);

	for (long GroupNo = 0; GroupNo < ParagigmGroupsCount; GroupNo++)
	{
		BYTE POS;
		QWORD Grammems = 0;
		if (!pGramtab->ProcessPOSAndGrammems(ParagigmGroups[GroupNo].c_str(), POS, Grammems)) continue;;
		std::string strPOS = pGramtab->GetPartOfSpeechStr(GetWidePOS(POS, Grammems, Holder->m_CurrentLanguage));
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


std::vector<CFormGroup>  BuildInterfaceParadigmPart(const CMorphologyHolder* Holder, const std::vector<CFormAndGrammems> FormAndGrammems, int& FormNo, QWORD& commonGrammems) {
	int EndFormNo = FormNo + 1;
	for (; EndFormNo < FormAndGrammems.size(); EndFormNo++)
		if (FormAndGrammems[FormNo].m_POS != FormAndGrammems[EndFormNo].m_POS)
			break;
	std::vector<CFormAndGrammems> FormAndGrammemsPart;
	FormAndGrammemsPart.insert(FormAndGrammemsPart.begin(), FormAndGrammems.begin() + FormNo, FormAndGrammems.begin() + EndFormNo);
	FormNo = EndFormNo;
	return GetParadigmByGroups(FormAndGrammemsPart, Holder, commonGrammems);
};


std::vector<CFormAndGrammems> BuildFormAndGrammems(const CMorphologyHolder* Holder, const CFormInfo* piParadigm) {
	const CAgramtab* pGramtab = Holder->m_pGramTab;
	// получаем все формы и граммемы в массив FormAndGrammems
	std::vector<CFormAndGrammems> FormAndGrammems;
	for (int j = 0; j < piParadigm->GetCount(); j++)
	{
		std::string GramInfo = piParadigm->GetAncode(j);
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
			if (F.m_POS == _R("ИНФИНИТИВ"))
				F.m_POS.insert(0, " ");


			FormAndGrammems.push_back(F);
		};
	};
	sort(FormAndGrammems.begin(), FormAndGrammems.end());
	return FormAndGrammems;
}


static std::string GetInterfacePOS(std::string POS)
{
	Trim(POS);
	if (POS == _R("Г")) return _R("ЛИЧНАЯ ФОРМА");
	if (POS == _R("С")) return _R("СУЩЕСТВИТЕЛЬНОЕ");
	if (POS == _R("П")) return _R("ПРИЛАГАТЕЛЬНОЕ");
	if (POS == _R("МС")) return _R("МЕСТОИМЕНИЕ");
	if (POS == _R("МС-П")) return _R("МЕСТОИМЕНИЕ-ПРИЛАГАТЕЛЬНОЕ");
	if (POS == _R("МС-ПРЕДК")) return _R("МЕСТОИМЕНИЕ-ПРЕДИКАТИВ");
	if (POS == _R("ЧИСЛ")) return _R("ЧИСЛИТЕЛЬНОЕ");
	if (POS == _R("ЧИСЛ-П")) return _R("ПОРЯДКОВОЕ ЧИСЛИТЕЛЬНОЕ");
	if (POS == _R("Н")) return _R("НАРЕЧИЕ");
	if (POS == _R("ПРЕДК")) return _R("ПРЕДИКАТИВ");
	if (POS == _R("ПРЕДЛ")) return _R("ПРЕДЛОГ");
	if (POS == _R("МЕЖД")) return _R("МЕЖДОМЕТИЕ");
	if (POS == _R("ВВОДН")) return _R("ВВОДНОЕ СЛОВО");
	if (POS == _R("ЧАСТ")) return _R("ЧАСТИЦА");
	if (POS == _R("КР_ПРИЛ")) return _R("КРАТКОЕ ПРИЛАГАТЕЛЬНОЕ");
	if (POS == _R("КР_ПРИЧАСТИЕ")) return _R("КРАТКОЕ ПРИЧАСТИЕ");
	return POS;
};

nlohmann::json  GetParadigmFromDictionary(const CFormInfo* piParadigm, const CMorphologyHolder* Holder, bool sortForms) {
	const CAgramtab* pGramtab = Holder->m_pGramTab;
	const std::vector<CFormAndGrammems> FormAndGrammems = BuildFormAndGrammems(Holder, piParadigm);
	nlohmann::json result = nlohmann::json::array();
	int FormNo = 0;
	while (FormNo < FormAndGrammems.size()) {
		int saveFormNo = FormNo;
		QWORD commonGrammems;
		const std::vector<CFormGroup> FormGroups = BuildInterfaceParadigmPart(Holder, FormAndGrammems, FormNo, commonGrammems);
		assert(FormNo > saveFormNo);
		auto prdPart = nlohmann::json::object();
		std::string pos = GetInterfacePOS(FormAndGrammems[saveFormNo].m_POS);
		if (commonGrammems > 0)
			pos += std::string(" ") + pGramtab->GrammemsToStr(commonGrammems);
		prdPart["pos"] = TrimCommaRight(pos);

		prdPart["formsGroups"] = nlohmann::json::array();
		for (auto fg : FormGroups)	{
			auto subg = nlohmann::json::object();
            std::string grm = pGramtab->GrammemsToStr(fg.m_IntersectGrammems & ~commonGrammems);
			subg["grm"] = TrimCommaRight(grm);
			subg["forms"] = nlohmann::json::array();
			for (auto formNo : fg.m_FormNos) {
				auto& f = FormAndGrammems[formNo + saveFormNo];
                std::string grm = pGramtab->GrammemsToStr(f.m_Grammems & ~(fg.m_IntersectGrammems | commonGrammems));
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
	
	std::string typeAncode = piParadigm->GetCommonAncode();
	std::string commonGrammems;
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
	std::string GramInfo;
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



bool GetParadigmCollection(std::string WordForm, std::vector<CFormInfo>&	Paradigms, const CMorphologyHolder* Holder)
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

std::string LemmatizeJson(std::string WordForm, const CMorphologyHolder* Holder, bool withParadigm, bool prettyJson, bool sortForms) {
	std::vector<CFormInfo>	Paradigms;
	if (!GetParadigmCollection(WordForm, Paradigms, Holder)) {
		return "[]";
	};

	nlohmann::json result = nlohmann::json::array();
	std::string strResult = "[";
	for (long i = 0; i < Paradigms.size(); i++) {
		result.push_back ( GetStringByParadigmJson(&(Paradigms[i]), Holder, withParadigm, sortForms) );
	};
	ConvertToUtfRecursive(result, Holder->m_CurrentLanguage);
	return result.dump(prettyJson?1:-1);
}



