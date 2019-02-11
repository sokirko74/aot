cmake_minimum_required(VERSION 3.5)

project(GraphmatThick)

add_executable (GraphmatThick GraphmatThick.cpp)

target_link_libraries(GraphmatThick 
    GraphanLib
)

macro (GraphTest lang)
    add_test(
        NAME ${lang}Graphan 
        COMMAND GraphmatThick ${lang} test/${lang}/text.txt -gra test/${lang}/out.gra
        WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}
    )
    add_test(
        NAME ${lang}GraphanCompare 
        COMMAND ${CMAKE_COMMAND} -E compare_files  test/${lang}/canon.gra test/${lang}/out.gra
        WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}
    )
endmacro(GraphTest)

GraphTest (Russian)
GraphTest (English)
GraphTest (German)
