cmake_minimum_required(VERSION 3.5)

project(GraphmatThick)

add_executable (GraphmatThick GraphmatThick.cpp)

target_link_libraries(GraphmatThick 
    GraphanLib
)

macro (GraphTest lang)
    add_test(
        NAME ${lang}Graphan 
        COMMAND GraphmatThick --language ${lang} --input-file test/${lang}/text.txt --output-file test/${lang}/out.gra
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

file(GLOB grapan_tests ${PROJECT_SOURCE_DIR}/test/RussianMany/*.txt)

foreach (input_file ${grapan_tests})
    get_filename_component(test_name ${input_file} NAME)
    message (${input_file})
    add_test(
        NAME ${test_name}Graphan 
        COMMAND GraphmatThick --language Russian --input-file ${input_file} --output-file ${input_file}.gra
        WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}
    )
    add_test(
        NAME ${test_name}GraphanCompare 
        COMMAND ${CMAKE_COMMAND} -E compare_files  ${input_file}.gra ${input_file}.gra.canon
        WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}
    )
endforeach(loop_var)
