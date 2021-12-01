declare_cmake_min_version()

add_executable(TestLemmatizerCOM
        TestLemmatizerCOM.cpp
)

add_dependencies(TestLemmatizerCOM
    Lemmatizer
)

target_link_libraries(TestLemmatizerCOM
    Lemmatizer
)

add_test(
    NAME Morphan_COM_Russian 
    COMMAND TestLemmatizerCOM test/Russian/text.txt
    WORKING_DIRECTORY .
)

add_test(
    NAME Morphan_COM_Russian_cmp
    COMMAND ${CMAKE_COMMAND} -E compare_files test/Russian/canon.lemma test/Russian/text.txt.lemma
    WORKING_DIRECTORY .
)
