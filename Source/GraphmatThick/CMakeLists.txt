cmake_minimum_required(VERSION 3.5)

project(GraphmatThick)

add_executable (GraphmatThick GraphmatThick.cpp)

target_link_libraries(GraphmatThick 
    GraphanLib
)

macro (GraphTest lang filename)
    get_filename_component(base_name ${filename} NAME)
    add_test(
        NAME Graphan_${lang}_${base_name} 
        COMMAND GraphmatThick --language ${lang} --input-file ${filename} --output-file ${filename}.gra
        WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}
    )
    add_test(
        NAME Graphan_${lang}_${base_name}_cmp
        COMMAND ${CMAKE_COMMAND} -E compare_files  ${filename}.gra ${filename}.gra.canon
        WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}
    )
endmacro(GraphTest)

GraphTest (Russian test/Russian/text.txt)
GraphTest (Russian test/Russian/force_to_rus.txt)
GraphTest (English test/English/text.txt)
GraphTest (German test/German/text.txt)
GraphTest (German test/German/sentbreaker.txt)

file(GLOB grapan_tests ${PROJECT_SOURCE_DIR}/test/RussianMany/*.txt)
foreach (input_file ${grapan_tests})
    GraphTest (Russian ${input_file})
endforeach(loop_var)
