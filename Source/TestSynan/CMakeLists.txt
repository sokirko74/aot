cmake_minimum_required(VERSION 3.5)

project(TestSynan)

add_executable (TestSynan TestSynan.cpp)

target_link_libraries(TestSynan 
    SynCommonLib
    SynanLib
)

macro (TestSynanLang lang)
    add_test(
        NAME Synan_${lang} 
        COMMAND TestSynan --language ${lang} --input test/${lang}/test.txt --output test/${lang}/out.txt
        WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}
    )
    add_test(
        NAME Synan_${lang}_cmp 
        COMMAND ${CMAKE_COMMAND} -E compare_files  test/${lang}/canon.txt test/${lang}/out.txt
        WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}
    )
endmacro(TestSynanLang)

TestSynanLang(Russian)
TestSynanLang(German)

