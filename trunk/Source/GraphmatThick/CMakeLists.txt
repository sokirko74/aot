cmake_minimum_required(VERSION 3.5)

project(GraphmatThick)

add_executable (GraphmatThick GraphmatThick.cpp)

target_link_libraries(GraphmatThick 
    GraphanLib
)

add_test(NAME RusGraphan COMMAND GraphmatThick Russian test/Rus/text.txt -gra test/Rus/out.gra)
add_test(NAME RusGraphanCompare COMMAND ${CMAKE_COMMAND} -E compare_files  test/Rus/canon.gra test/Rus/out.gra)

add_test(NAME EngGraphan COMMAND GraphmatThick English test/Eng/text.txt -gra test/Eng/out.gra)
add_test(NAME EngGraphanCompare COMMAND ${CMAKE_COMMAND} -E compare_files  test/Eng/canon.gra test/Eng/out.gra)

add_test(NAME GerGraphan COMMAND GraphmatThick German test/Ger/text.txt -gra test/Ger/out.gra)
add_test(NAME GerGraphanCompare COMMAND ${CMAKE_COMMAND} -E compare_files  test/Ger/canon.gra test/Ger/out.gra)
