#include "dicts/BinaryDictsLib/BinaryDictionary.h"
#include "morph_dict/lemmatizer_base_lib/MorphanHolder.h"


int main(int argc, char **argv) {
    try {
        assert(argc == 3);
        GlobalLoadMorphHolder(morphEnglish);
        GlobalLoadMorphHolder(morphRussian);
        BuildBinaryDict(&GetMHolder(morphRussian), &GetMHolder(morphEnglish), argv[1], argv[2]);
        return 0;
    }
    catch (std::exception& e) {
        fprintf(stderr, "exception occurred: %s!\n", e.what());
        return 1;
    }
    catch (...) {
        fprintf(stderr, "some exception occurred!\n");
        return 1;
    }
}
