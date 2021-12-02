declare_cmake_min_version()

project(LemmatizerBaseLib)

add_library(LemmatizerBaseLib
        CreatePredictionBase.cpp
        Lemmatizers.cpp
        MorphAutomBuilder.cpp
        MorphAutomat.cpp
        MorphDict.cpp
        MorphDictBuilder.cpp
        Paradigm.cpp
        Predict.cpp
        Statistic.cpp
        MorphanHolder.cpp
        rus_numerals.cpp
        )

target_link_libraries(LemmatizerBaseLib
        AgramtabLib
        MorphWizardLib
        )

