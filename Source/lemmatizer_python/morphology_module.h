#include "../common/utilit.h"
int fact(int n);
bool load_morphology(int language);
std::string lemmatize_json(std::string word_form, bool allForms);
std::string synthesize(std::string word_form, std::string part_of_speech_and_grammems);