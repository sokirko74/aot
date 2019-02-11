cmake_minimum_required(VERSION 3.5)

project(TestLem)

add_executable (TestLem TestLem.cpp)

target_link_libraries(TestLem 
    LemmatizerLib
)

macro (TestLemLang lang)
    add_test(
        NAME ${lang}Morphan 
        COMMAND TestLem --echo  --no-ids --forms --language ${lang} --input test/${lang}/test.txt --output test/${lang}/out.txt
        WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}
    )

    add_test(
        NAME ${lang}MorphanCompare 
        COMMAND ${CMAKE_COMMAND} -E compare_files  test/${lang}/canon.txt test/${lang}/out.txt
        WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}
    )
endmacro(TestLemLang)

TestLemLang(Russian)
TestLemLang(German)
TestLemLang(English)

if (BUILD_DICTS)
    add_dependencies (TestLem MorphDicts)
endif()

install (TARGETS TestLem 
	RUNTIME DESTINATION ${INSTALL_BIN_FOLDER})
