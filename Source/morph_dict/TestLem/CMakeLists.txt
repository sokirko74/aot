declare_cmake_min_version()

project(TestLem)

add_executable (TestLem TestLem.cpp)

target_link_libraries(TestLem
        LemmatizerBaseLib
)

macro (TestLemLang lang filename optional_morphan_arg)
    get_filename_component(base_name ${filename} NAME)
    add_test(
        NAME Morphan_${lang}_${base_name} 
        COMMAND TestLem --echo  ${optional_morphan_arg} --no-ids --forms --language ${lang} --input ${filename} --output ${filename}.morph
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )

    add_test(
        NAME Morphan_${lang}_${base_name}_cmp
        COMMAND ${CMAKE_COMMAND} -E compare_files  ${filename}.morph ${filename}.morph.canon
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )
endmacro(TestLemLang)

TestLemLang(Russian test/Russian/test.txt "")
TestLemLang(Russian test/Russian/morphan.txt --morphan)
TestLemLang(Russian test/Russian/predict.txt "")

TestLemLang(German test/German/test.txt "")
TestLemLang(German test/German/morphan.txt --morphan)

TestLemLang(English test/English/test.txt "")

if (BUILD_DICTS)
    add_dependencies (TestLem MorphDicts)
endif()

install (TARGETS TestLem 
	RUNTIME DESTINATION ${INSTALL_BIN_FOLDER})
