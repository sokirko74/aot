cmake_minimum_required(VERSION 3.5)

project(TestMapost)

add_executable (TestMapost TestMapost.cpp)

target_link_libraries(TestMapost 
    LemmatizerLib
    MAPostLib
    SynanLib
)

macro (TestMapost lang filename)
    get_filename_component(base_name ${filename} NAME)
    add_test(
        NAME ${lang}_${base_name}_TestMapost 
        COMMAND TestMapost --language ${lang} --input ${filename} --output ${filename}.mapost
        WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}
    )

    add_test(
        NAME ${lang}_${base_name}_TestMapostCompare 
        COMMAND ${CMAKE_COMMAND} -E compare_files  ${filename}.mapost ${filename}.mapost.canon
        WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}
    )
endmacro(TestMapost)

TestMapost(Russian test/Russian/test.txt "")


if (BUILD_DICTS)
    add_dependencies (TestLem MorphDicts)
endif()

