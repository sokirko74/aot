cmake_minimum_required(VERSION 3.6)

project(TestLem)

add_executable (TestLem TestLem.cpp)

target_link_libraries(TestLem 
    LemmatizerLib
)

macro (TestLemLang lang)
    add_test(NAME ${lang}Morphan COMMAND TestLem --echo  --no-ids --forms --language ${lang} --input test/${lang}/test.txt --output test/${lang}/out.txt)
    add_test(NAME ${lang}MorphanCompare COMMAND ${CMAKE_COMMAND} -E compare_files  test/${lang}/canon.txt test/${lang}/out.txt)
endmacro(TestLemLang)

TestLemLang(Russian)
TestLemLang(German)
TestLemLang(English)

add_dependencies (TestLem MorphDicts)