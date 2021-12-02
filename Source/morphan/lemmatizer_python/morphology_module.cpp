#include "morphology_module.h"
#include "morph_dict/common/utilit.h"
#include "morph_dict/common/argparse.h"
#include "morph_dict/AgramtabLib/RusGramTab.h"
#include "morph_dict/AgramtabLib/EngGramTab.h"
#include "morph_dict/AgramtabLib/GerGramTab.h"
#include "morph_dict/LemmatizerBaseLib/Lemmatizers.h"
#include "morph_dict/LemmatizerBaseLib/Paradigm.h"
#include "../../LemmatizerLib/MorphologyHolder.h"

CMorphologyHolder Holder;


bool load_morphology(int language, bool use_prediction) {
    try {
        Holder.LoadLemmatizer((MorphLanguageEnum) language);
        Holder.m_pGramTab->m_bUseNationalConstants = false;
        Holder.m_bUsePrediction = use_prediction;
    }
    catch (CExpc e) {
        std::cerr << "Cannot load morphology\n";
        return false;
    }
    return true;
}

std::string lemmatize_json(std::string word_form, bool allForms) {
    word_form = convert_from_utf8(word_form.c_str(), Holder.m_CurrentLanguage);
    return Holder.LemmatizeJson(word_form.c_str(), allForms, true, true);
}

    bool is_in_dictionary(std::string word_form) {
    word_form = convert_from_utf8(word_form.c_str(), Holder.m_CurrentLanguage);
    return Holder.IsInDictionary(word_form);
}

std::string synthesize(std::string word_form, std::string part_of_speech_and_grammems) {
    word_form = convert_from_utf8(word_form.c_str(), Holder.m_CurrentLanguage);
    part_of_speech_and_grammems = convert_from_utf8(part_of_speech_and_grammems.c_str(), Holder.m_CurrentLanguage);
    part_of_speech_t partOfSpeech;
    grammems_mask_t grammems;
    if (!Holder.m_pGramTab->ProcessPOSAndGrammemsIfCan(part_of_speech_and_grammems.c_str(), &partOfSpeech, &grammems)) {
        return "{\"forms\":[], \"error\": \"cannot process part_of_speech_and_grammems\"}";
    }
    std::vector<CFormInfo>	paradigms;
    Holder.m_pLemmatizer->CreateParadigmCollection(false, 
                        word_form, 
                        is_upper_alpha((unsigned char)word_form[0], 
                        Holder.m_CurrentLanguage), Holder.m_bUsePrediction, paradigms);
    if (paradigms.empty()) {
        return "{\"forms\":[], \"error\": \"word form not found\"}";
    };
    std::vector<std::string> found_forms;
    for (auto& p : paradigms) {
        if (Holder.m_pGramTab->GetPartOfSpeech(p.GetAncode(0).c_str()) != partOfSpeech) {
            continue;
        }
        for (size_t i = 0; i < p.GetCount(); i++) {
            
            std::string gramCodes = p.GetAncode(i) + p.GetCommonAncode();
            grammems_mask_t form_mask = Holder.m_pGramTab->GetAllGrammems(gramCodes.c_str());
            if ((form_mask & grammems) == grammems) {
                found_forms.push_back( std::string("\"") + convert_to_utf8(p.GetWordForm(i), Holder.m_CurrentLanguage) + std::string("\"") );
            }
        }
    }
    return std::string("{\"forms\": [") + join_string(found_forms,",") + std::string("]}");

}
