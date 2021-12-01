declare_cmake_min_version()
project(TestLemmatizerCOM)

add_executable(${PROJECT_NAME}
        TestLemmatizerCOM.cpp
)

add_dependencies(${PROJECT_NAME}
    Lemmatizer
)

target_link_libraries(${PROJECT_NAME}
    Lemmatizer
)

add_test(
    NAME Morphan_COM_Russian 
    COMMAND TestLemmatizerCOM test/Russian/text.txt
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_test(
    NAME Morphan_COM_Russian_cmp
    COMMAND ${CMAKE_COMMAND} -E compare_files test/Russian/canon.lemma test/Russian/text.txt.lemma
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
