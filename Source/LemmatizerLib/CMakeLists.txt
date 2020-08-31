declare_cmake_min_version()

project(LemmatizerLib)

add_library(LemmatizerLib
CreatePredictionBase.cpp
Lemmatizers.cpp
MorphAutomBuilder.cpp
MorphAutomat.cpp
MorphDict.cpp
MorphDictBuilder.cpp
PLMLineCollection.cpp
Paradigm.cpp
Predict.cpp
Statistic.cpp
StdMorph.cpp
MorhologyHolder.cpp
Morphan.cpp
)

target_link_libraries(LemmatizerLib
    AgramtabLib
    PCRE
    GraphanLib
    MorphWizardLib
)

