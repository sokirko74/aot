cmake_minimum_required(VERSION 3.5)

project(TestSynan)

add_executable (TestSynan TestSynan.cpp)

target_link_libraries(TestSynan 
    SynCommonLib
    SynanLib
)

macro (TestSynanLang lang)
    add_test(NAME ${lang}Synan COMMAND TestSynan --language ${lang} --input test/${lang}/test.txt --output test/${lang}/out.txt)
    add_test(NAME ${lang}SynanCompare COMMAND ${CMAKE_COMMAND} -E compare_files  test/${lang}/canon.txt test/${lang}/out.txt)
endmacro(TestSynanLang)

TestSynanLang(Russian)
TestSynanLang(German)

