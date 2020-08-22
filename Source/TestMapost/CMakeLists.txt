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
        NAME Mapost_${lang}_${base_name} 
        COMMAND TestMapost --language ${lang} --input-file ${filename} --output-file ${filename}.mapost
        WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}
    )

    add_test(
        NAME Mapost_${lang}_${base_name}_cmp 
        COMMAND ${CMAKE_COMMAND} -E compare_files  ${filename}.mapost ${filename}.mapost.canon
        WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}
    )
endmacro(TestMapost)

TestMapost(Russian test/Russian/fio.txt "")
TestMapost(Russian test/Russian/lle.txt "")
TestMapost(Russian test/Russian/num.txt "")
TestMapost(Russian test/Russian/hom.txt "")
TestMapost(Russian test/Russian/fixed_collocs.txt "")
TestMapost(Russian test/Russian/seminouns.txt "")
TestMapost(Russian test/Russian/redub.txt "")
TestMapost(Russian test/Russian/other.txt "")
TestMapost(Russian test/Russian/indecline.txt "")


if (BUILD_DICTS)
    add_dependencies (TestLem MorphDicts)
endif()

