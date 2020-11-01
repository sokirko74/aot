/* File: example.c */

#include "morphology_module.h"
#include "../common/utilit.h"
#include "../common/argparse.h"
#include "../AgramtabLib/RusGramTab.h"
#include "../AgramtabLib/EngGramTab.h"
#include "../AgramtabLib/GerGramTab.h"
#include "../LemmatizerLib/Lemmatizers.h"
#include "../LemmatizerLib/Paradigm.h"
#include "../LemmatizerLib/Morphan.h"
#include "../LemmatizerLib/MorphologyHolder.h"

CMorphologyHolder Holder;

int fact(int n) {
    if (n < 0){ /* This should probably return an error, but this is simpler */
        return 0;
    }
    if (n == 0) {
        return 1;
    }
    else {
        /* testing for overflow would be a good idea here */
        return n * fact(n-1);
    }
}

bool load_morphology(int language) {
    if (!Holder.LoadLemmatizer((MorphLanguageEnum)language)) {
        std::cerr << "Cannot load morphology\n";
        return false;
    }
    return true;
}

std::string lemmatize_json(std::string word_form, bool allForms) {
    word_form = convert_from_utf8(word_form.c_str(), Holder.m_CurrentLanguage);
    return LemmatizeJson(word_form.c_str(), &Holder, allForms, true, true);
}

std::string synthesize(std::string word_form, std::string part_of_speech_and_grammems) {
    word_form = convert_from_utf8(word_form.c_str(), Holder.m_CurrentLanguage);
    part_of_speech_and_grammems = convert_from_utf8(part_of_speech_and_grammems.c_str(), Holder.m_CurrentLanguage);
    part_of_speech_t partOfSpeech;
    grammems_mask_t grammems;
    if (!Holder.m_pGramTab->ProcessPOSAndGrammemsIfCan(part_of_speech_and_grammems.c_str(), &partOfSpeech, &grammems)) {
        return "{'forms':'', 'error': 'cannot process part_of_speech_and_grammems'}";
    }
    std::vector<CFormInfo>	paradigms;
    Holder.m_pLemmatizer->CreateParadigmCollection(false, 
                        word_form, 
                        is_upper_alpha((unsigned char)word_form[0], 
                        Holder.m_CurrentLanguage), true, paradigms);
    if (paradigms.empty()) {
        return "{'forms':'', 'error': 'word form not found'}";
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
                found_forms.push_back(convert_to_utf8(p.GetWordForm(i), Holder.m_CurrentLanguage));
            }
        }
    }
    return std::string("{'forms':'") + join_string(found_forms,";") + std::string("'}");

}