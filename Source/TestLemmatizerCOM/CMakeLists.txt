cmake_minimum_required(VERSION 3.6)


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
    WORKING_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR}
)

add_test(
    NAME Morphan_COM_Russian_cmp
    COMMAND ${CMAKE_COMMAND} -E compare_files  test/Russian/canon.lemma test/Russian/text.txt.lemma
    WORKING_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR}
)
