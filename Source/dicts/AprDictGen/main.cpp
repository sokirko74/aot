#include "dicts/BinaryDictsLib/BinaryDictionary.h"
#include "dicts/BinaryDictsLib/BinaryDictionary.h"
#include "morph_dict/common/util_classes.h"
#include "morph_dict/lemmatizer_base_lib/MorphanHolder.h"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>




int main(int argc, char **argv) {
    try {
        assert(argc == 3);
        CMorphanHolder rus;
        CMorphanHolder eng;
        rus.LoadMorphology(morphRussian);
        eng.LoadMorphology(morphEnglish);
        BuildBinaryDict(&rus, &eng, argv[1], argv[2]);
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
