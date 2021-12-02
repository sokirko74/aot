// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko

#include "morph_dict/common/json.h"
#include "MorphanHolder.h"

CMorphanHolder::CMorphanHolder()
{
	m_pLemmatizer = 0;
	m_pGramTab = 0;
    m_bUsePrediction = true;
};

CMorphanHolder::~CMorphanHolder()
{
	DeleteProcessors();
};

void CMorphanHolder::DeleteProcessors()
{
	if (m_pLemmatizer) 
	{
		delete m_pLemmatizer;
		m_pLemmatizer = 0;
	}
	if (m_pGramTab)
	{
		delete m_pGramTab;
		m_pGramTab = 0;
	}

};

void CMorphanHolder::CreateMorphDicts(MorphLanguageEnum langua) 
{
    if (langua == morphRussian)
    {
        m_pGramTab = new CRusGramTab;
        m_pLemmatizer = new CLemmatizerRussian;
    }
    else
        if (langua == morphGerman)
        {
            m_pGramTab = new CGerGramTab;
            m_pLemmatizer = new CLemmatizerGerman;
        }
        else
            if (langua == morphEnglish)
            {
                m_pGramTab = new CEngGramTab;
                m_pLemmatizer = new CLemmatizerEnglish;
            }
            else
            {
                throw CExpc("unsupported language");
            };
}

void CMorphanHolder::LoadLemmatizer(MorphLanguageEnum langua, std::string custom_folder)
{
        DeleteProcessors();
        CreateMorphDicts(langua);
        if (custom_folder.empty()) {
            m_pLemmatizer->LoadDictionariesRegistry();
            m_pGramTab->LoadFromRegistry();
        }
        else {
            m_pLemmatizer->LoadDictionariesFromPath(custom_folder);
            m_pGramTab->Read(MakePath(custom_folder, m_pGramTab->GramtabFileName).c_str());
        }
		m_CurrentLanguage = langua;
}

DwordVector CMorphanHolder::GetParadigmIdsByNormAndAncode(std::string &str, const std::string &code) const
{
	std::vector<CFormInfo> ParadigmCollection;
	DwordVector res;
	if (!m_pLemmatizer->CreateParadigmCollection(true, str, true, false, ParadigmCollection ))
		throw CExpc(Format("Cannot lemmatize %s", str.c_str()));

	for(int i = 0; i < ParadigmCollection.size(); i++)
	{
		const CFormInfo& Paradigm = ParadigmCollection[i];
		if(!Paradigm.m_bFound) continue;
		std::string ancode = Paradigm.GetAncode(0);
		for (long j=0;  j < code.size(); j+=2)
			for (long k=0;  k < ancode.size(); k+=2)
				if(ancode.substr(k, 2) == code.substr(j, 2))
				{
					res.push_back(Paradigm.GetParadigmId());
					goto next_paradigm;
				}
next_paradigm:;
	}
	return res;
}

void CMorphanHolder::string_to_ids(const char *str, DwordVector &ids, bool bNorm) const
{
	ids.clear();
	
	std::vector<CFormInfo > ParadigmCollection;
	
	std::string Word = str;
	if (!m_pLemmatizer->CreateParadigmCollection(bNorm, Word, true, false, ParadigmCollection))
	{
		fprintf (stderr,"cannot lemmatize %s\n", str);
		return;
	}
	

	for(int i = 0; i < ParadigmCollection.size(); i++)
	{
		const CFormInfo& Paradigm = ParadigmCollection[i];
		if(!Paradigm.m_bFound) continue;

		ids.push_back(Paradigm.GetParadigmId());
	}
}

std::string CMorphanHolder::id_to_string(long id) const
{
	
	CFormInfo Res;
	if (!m_pLemmatizer->CreateParadigmFromID(id, Res))
		throw CExpc (Format( "cannot get lemma  by id %i", id));
	return Res.GetWordForm(0);
}

CFormInfo CMorphanHolder::id_to_paradigm(long id) const
{
	
	CFormInfo Res;
	if (!m_pLemmatizer->CreateParadigmFromID(id, Res))
		throw CExpc (Format( "cannot get lemma  by id %i", id));
	return Res;
}


std::string CMorphanHolder::GetGrammems(const char* tab_str) const {
	uint64_t G;
	m_pGramTab->GetGrammems(tab_str, G);
	std::string s = m_pGramTab->GrammemsToStr(G);
	if (!s.empty() && (s[s.length() - 1] == ','))
		s.erase(s.length() - 1);
	return s;
}

std::string CMorphanHolder::PrintMorphInfoUtf8(std::string Form, bool printIds, bool printForms, bool sortParadigms) const
{
	bool bCapital = is_upper_alpha((BYTE)Form[0], m_CurrentLanguage);

	std::vector<CFormInfo> Paradigms;
	m_pLemmatizer->CreateParadigmCollection(false, Form, bCapital, true, Paradigms);

	std::vector<std::string> Results;
	for (int i = 0; i < Paradigms.size(); i++) {
		std::string Result;
		const CFormInfo& F = Paradigms[i];
		Result += F.m_bFound ? "+ " : "- ";

		Result += F.GetWordForm(0) + " ";

		{
			std::string GramCodes = F.GetSrcAncode();
			BYTE PartOfSpeech = m_pGramTab->GetPartOfSpeech(GramCodes.c_str());
			Result += m_pGramTab->GetPartOfSpeechStr(PartOfSpeech) + std::string(" ");

			std::string CommonAncode = F.GetCommonAncode();
			Result += Format("%s ", (CommonAncode.empty()) ? "" : GetGrammems(CommonAncode.c_str()).c_str());

			for (long i = 0; i < GramCodes.length(); i += 2) {
				if (i > 0)
					Result += ";";
				Result += Format("%s", GetGrammems(GramCodes.c_str() + i).c_str());
			}

		}

		if (printIds)
			Result += Format(" %i", F.GetParadigmId());

		BYTE Accent = F.GetSrcAccentedVowel();
		if (Accent != 0xff)
			Result += Format(" %s'%s", Form.substr(0, Accent + 1).c_str(), Form.substr(Accent + 1).c_str());

		if (printForms) {
			Result += " ";
			for (int k = 0; k < F.GetCount(); k++) {
				if (k > 0)
					Result += ",";
				Result += Paradigms[i].GetWordForm(k);
			};
		};
		Results.push_back(Result);
	};

	if (sortParadigms) {
		std::sort(Results.begin(), Results.end());
	};
	std::string Result;
	for (int i = 0; i < Results.size(); i++) {
		if (i > 0)
			Result += "\t";
		Result += Results[i] + "\n";
	}
	return convert_to_utf8(Result, m_CurrentLanguage);
};

inline  bool IsUpper(int x, MorphLanguageEnum Langua)
{
	return is_upper_alpha(x, Langua);
};

bool CMorphanHolder::GetParadigmCollection(std::string WordForm, std::vector<CFormInfo>&	Paradigms) const {
	if (WordForm.length() == 0)	{
		return false;
	};

	try
	{
		if (m_pLemmatizer == nullptr) return false;
		m_pLemmatizer->CreateParadigmCollection(false,
                                                  WordForm,
                                                  IsUpper((unsigned char)WordForm[0],
                                                          m_CurrentLanguage),
                                                          m_bUsePrediction,
                                                          Paradigms);
	}
	catch (...)
	{
		return false;;
	};
	return true;
};

bool CMorphanHolder::IsInDictionary(std::string WordForm) const {
	std::vector<CFormInfo>	Paradigms;
	if (!GetParadigmCollection(WordForm, Paradigms)) {
		return false;
	};
	return !Paradigms.empty();
}



const int ParagigmGroupsCount = 45;
const std::string ParagigmGroups[ParagigmGroupsCount] = {
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
        "VBE pl"};

struct CFormAndGrammems
{
    std::string m_Form;
    std::string m_POS;
    grammems_mask_t m_Grammems;

    bool operator<(const CFormAndGrammems &X) const
    {
        return m_POS < X.m_POS;
    };
};

struct CFormGroup
{
    grammems_mask_t m_IntersectGrammems;
    std::vector<int> m_FormNos;
};


std::string &TrimCommaRight(std::string &str)
{
    if (str.size() == 0)
        return str;
    size_t i = str.find_last_not_of(",");
    str.erase(i + 1);
    return str;
};

std::string GetGramInfoStr(std::string GramInfo, const CMorphanHolder *Holder)
{
    const CAgramtab *pGramtab = Holder->m_pGramTab;
    assert(!GramInfo.empty());
    int POS = pGramtab->GetPartOfSpeech(GramInfo.c_str());
    std::string Result;
    Result += pGramtab->GetPartOfSpeechStr(POS);
    Result += " ";
    grammems_mask_t grammems = pGramtab->GetAllGrammems(GramInfo.c_str());
    Result += pGramtab->GrammemsToStr(grammems);
    TrimCommaRight(Result);
    return Result;
};

std::vector<CFormGroup> GetParadigmByGroups(const std::vector<CFormAndGrammems> &Forms, const CMorphanHolder *Holder, grammems_mask_t &CommonGrammems)
{
    std::vector<CFormGroup> Results;
    std::vector<bool> IncludedVector;
    int i = 0;
    const CAgramtab *pGramtab = Holder->m_pGramTab;

    for (; i < Forms.size(); i++)
        IncludedVector.push_back(false);

    for (long GroupNo = 0; GroupNo < ParagigmGroupsCount; GroupNo++)
    {
        part_of_speech_t POS;
        grammems_mask_t Grammems = 0;
        if (!pGramtab->ProcessPOSAndGrammems(ParagigmGroups[GroupNo].c_str(), POS, Grammems))
            continue;
        ;
        std::string strPOS = pGramtab->GetPartOfSpeechStr(POS);
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

std::vector<CFormGroup> BuildInterfaceParadigmPart(const CMorphanHolder *Holder, const std::vector<CFormAndGrammems> FormAndGrammems, int &FormNo, grammems_mask_t &commonGrammems)
{
    int EndFormNo = FormNo + 1;
    for (; EndFormNo < FormAndGrammems.size(); EndFormNo++)
        if (FormAndGrammems[FormNo].m_POS != FormAndGrammems[EndFormNo].m_POS)
            break;
    std::vector<CFormAndGrammems> FormAndGrammemsPart;
    FormAndGrammemsPart.insert(FormAndGrammemsPart.begin(), FormAndGrammems.begin() + FormNo, FormAndGrammems.begin() + EndFormNo);
    FormNo = EndFormNo;
    return GetParadigmByGroups(FormAndGrammemsPart, Holder, commonGrammems);
};

std::vector<CFormAndGrammems> BuildFormAndGrammems(const CMorphanHolder *Holder, const CFormInfo *piParadigm)
{
    const CAgramtab *pGramtab = Holder->m_pGramTab;
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
            part_of_speech_t POS = pGramtab->GetPartOfSpeech(GramInfo.substr(i, 2).c_str());
            F.m_POS = pGramtab->GetPartOfSpeechStr(POS);

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
    if (POS == _R("Г"))
        return _R("ЛИЧНАЯ ФОРМА");
    if (POS == _R("С"))
        return _R("СУЩЕСТВИТЕЛЬНОЕ");
    if (POS == _R("П"))
        return _R("ПРИЛАГАТЕЛЬНОЕ");
    if (POS == _R("МС"))
        return _R("МЕСТОИМЕНИЕ");
    if (POS == _R("МС-П"))
        return _R("МЕСТОИМЕНИЕ-ПРИЛАГАТЕЛЬНОЕ");
    if (POS == _R("МС-ПРЕДК"))
        return _R("МЕСТОИМЕНИЕ-ПРЕДИКАТИВ");
    if (POS == _R("ЧИСЛ"))
        return _R("ЧИСЛИТЕЛЬНОЕ");
    if (POS == _R("ЧИСЛ-П"))
        return _R("ПОРЯДКОВОЕ ЧИСЛИТЕЛЬНОЕ");
    if (POS == _R("Н"))
        return _R("НАРЕЧИЕ");
    if (POS == _R("ПРЕДК"))
        return _R("ПРЕДИКАТИВ");
    if (POS == _R("ПРЕДЛ"))
        return _R("ПРЕДЛОГ");
    if (POS == _R("МЕЖД"))
        return _R("МЕЖДОМЕТИЕ");
    if (POS == _R("ВВОДН"))
        return _R("ВВОДНОЕ СЛОВО");
    if (POS == _R("ЧАСТ"))
        return _R("ЧАСТИЦА");
    if (POS == _R("КР_ПРИЛ"))
        return _R("КРАТКОЕ ПРИЛАГАТЕЛЬНОЕ");
    if (POS == _R("КР_ПРИЧАСТИЕ"))
        return _R("КРАТКОЕ ПРИЧАСТИЕ");
    return POS;
};

nlohmann::json GetParadigmFromDictionary(const CFormInfo *piParadigm, const CMorphanHolder *Holder, bool sortForms)
{
    const CAgramtab *pGramtab = Holder->m_pGramTab;
    const std::vector<CFormAndGrammems> FormAndGrammems = BuildFormAndGrammems(Holder, piParadigm);
    nlohmann::json result = nlohmann::json::array();
    int FormNo = 0;
    while (FormNo < FormAndGrammems.size())
    {
        int saveFormNo = FormNo;
        grammems_mask_t commonGrammems;
        const std::vector<CFormGroup> FormGroups = BuildInterfaceParadigmPart(Holder, FormAndGrammems, FormNo, commonGrammems);
        assert(FormNo > saveFormNo);
        auto prdPart = nlohmann::json::object();
        std::string pos = GetInterfacePOS(FormAndGrammems[saveFormNo].m_POS);
        if (commonGrammems > 0)
            pos += std::string(" ") + pGramtab->GrammemsToStr(commonGrammems);
        prdPart["pos"] = TrimCommaRight(pos);

        prdPart["formsGroups"] = nlohmann::json::array();
        for (auto fg : FormGroups)
        {
            auto subg = nlohmann::json::object();
            std::string grm = pGramtab->GrammemsToStr(fg.m_IntersectGrammems & ~commonGrammems);
            subg["grm"] = TrimCommaRight(grm);
            subg["forms"] = nlohmann::json::array();
            for (auto formNo : fg.m_FormNos)
            {
                auto &f = FormAndGrammems[formNo + saveFormNo];
                std::string grm = pGramtab->GrammemsToStr(f.m_Grammems & ~(fg.m_IntersectGrammems | commonGrammems));
                subg["forms"].push_back(
                        {{"f", f.m_Form},
                         {"grm", TrimCommaRight(grm)}});
            };
            if (sortForms)
            {
                sort(subg["forms"].begin(), subg["forms"].end());
            }
            prdPart["formsGroups"].push_back(subg);
        };
        result.push_back(prdPart);
    };
    return result;
};

nlohmann::json GetStringByParadigmJson(const CFormInfo *piParadigm, const CMorphanHolder *Holder, bool withParadigm, bool sortForms)
{
    auto result = nlohmann::json::object();
    result["found"] = piParadigm->m_bFound;

    std::string typeAncode = piParadigm->GetCommonAncode();
    std::string commonGrammems;
    if (!typeAncode.empty())
    {
        const CAgramtab *pGramtab = Holder->m_pGramTab;
        try
        {
            commonGrammems = pGramtab->GrammemsToStr(pGramtab->GetAllGrammems(typeAncode.c_str()));
        }
        catch (...)
        {
            throw CExpc(" an exception occurred while getting Common Ancode");
        };
    };
    result["commonGrammems"] = TrimCommaRight(commonGrammems);
    result["wordForm"] = piParadigm->GetWordForm(0);
    if (!piParadigm->m_bFound)
    {
        result["srcNorm"] = piParadigm->GetSrcNorm();
    }
    std::string GramInfo;
    try
    {
        GramInfo = piParadigm->GetSrcAncode();
    }
    catch (...)
    {
        GramInfo = piParadigm->GetAncode(0);
    }
    result["morphInfo"] = GetGramInfoStr(GramInfo, Holder);

    if (withParadigm)
    {
        result["paradigm"] = GetParadigmFromDictionary(piParadigm, Holder, sortForms);
    }
    return result;
}

std::string CMorphanHolder::LemmatizeJson(std::string WordForm, bool withParadigm, bool prettyJson, bool sortForms) const
{
    std::vector<CFormInfo> Paradigms;
    if (!GetParadigmCollection(WordForm, Paradigms))
    {
        return "[]";
    };

    nlohmann::json result = nlohmann::json::array();
    std::string strResult = "[";
    for (auto& p : Paradigms)
    {
        result.push_back(GetStringByParadigmJson(&(p), this, withParadigm, sortForms));
    };
    ConvertToUtfRecursive(result, m_CurrentLanguage);
    return result.dump(prettyJson ? 1 : -1);
}
