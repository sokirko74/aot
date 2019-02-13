cmake_minimum_required(VERSION 3.5)

project(TestLem)

add_executable (TestLem TestLem.cpp)

target_link_libraries(TestLem 
    LemmatizerLib
)

macro (TestLemLang lang)
    add_test(
        NAME ${lang}TestLem 
        COMMAND TestLem --echo  --no-ids --forms --language ${lang} --input test/${lang}/test.txt --output test/${lang}/out.txt
        WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}
    )

    add_test(
        NAME ${lang}TestLemCompare 
        COMMAND ${CMAKE_COMMAND} -E compare_files  test/${lang}/canon.txt test/${lang}/out.txt
        WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}
    )
endmacro(TestLemLang)

TestLemLang(Russian)
TestLemLang(German)
TestLemLang(English)

macro (MorphanLang lang)
    add_test(
        NAME ${lang}Morphan 
        COMMAND TestLem --echo  --forms --morphan --language ${lang} --input test/${lang}_morphan/test.txt --output test/${lang}_morphan/out.txt
        WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}
    )

    add_test(
        NAME ${lang}MorphanCompare 
        COMMAND ${CMAKE_COMMAND} -E compare_files  test/${lang}_morphan/canon.txt test/${lang}_morphan/out.txt
        WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}
    )
endmacro()
MorphanLang(Russian)
MorphanLang(German)

if (BUILD_DICTS)
    add_dependencies (TestLem MorphDicts)
endif()

install (TARGETS TestLem 
	RUNTIME DESTINATION ${INSTALL_BIN_FOLDER})
