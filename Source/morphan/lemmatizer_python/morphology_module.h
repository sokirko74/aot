#include "morph_dict/common/utilit.h"
bool load_morphology(int language, bool use_prediction);
std::string lemmatize_json(std::string word_form, bool allForms);
std::string synthesize(std::string word_form, std::string part_of_speech_and_grammems);
bool is_in_dictionary(std::string word_form);