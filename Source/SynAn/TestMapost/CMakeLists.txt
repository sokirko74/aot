declare_cmake_min_version()

project(TestMapost)

add_executable (TestMapost TestMapost.cpp)

target_link_libraries(TestMapost 
    LemmatizerLib
    MAPostLib
    SynanLib
)

macro (TestMapostMacro lang file_mask)
    add_test(
        NAME Mapost_${lang} 
        COMMAND TestMapost --language ${lang} --input-file-mask ${file_mask}
        WORKING_DIRECTORY ../../..
    )

    file(GLOB tests ${file_mask})
    foreach (input_file ${tests})
        get_filename_component(base_name ${input_file} NAME)
        add_test(
            NAME Mapost_${lang}_${base_name}_cmp 
            COMMAND ${CMAKE_COMMAND} -E compare_files  ${input_file}.mapost ${input_file}.mapost.canon
            WORKING_DIRECTORY ../../..
        )
    endforeach(input_file)
endmacro(TestMapostMacro)

TestMapostMacro(Russian test/Russian/*.txt)


if (BUILD_DICTS)
    add_dependencies (TestLem MorphDicts)
endif()

