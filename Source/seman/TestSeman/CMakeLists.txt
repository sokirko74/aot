declare_cmake_min_version()

project(TestSeman)

add_executable (TestSeman TestSeman.cpp)

target_link_libraries(TestSeman 
    SemanLib
)

macro (TestSemanMacro lang test_directory mode)
    set (test_directory_in_project "${PROJECT_SOURCE_DIR}/${test_directory}")
    get_filename_component(test_name ${test_directory} NAME)
    set (test_directory_in_build "${CMAKE_CURRENT_BINARY_DIR}/test")
    file(COPY ${test_directory_in_project} DESTINATION ${test_directory_in_build})
    set (file_mask ${test_directory_in_build}/${test_name}/*.txt )
    get_filename_component(base_name ${test_directory} NAME)
    add_test(
        NAME Seman_${lang}__${base_name} 
        COMMAND TestSeman --input-file-mask ${file_mask} ${mode}
    )
    file(GLOB tests ${file_mask} )
    foreach (input_file ${tests})
        get_filename_component(base_name ${input_file} NAME)
        add_test(
            NAME Seman_${lang}_${base_name}_cmp 
            COMMAND ${CMAKE_COMMAND} -E compare_files  ${test_directory_in_project}/${base_name}.seman ${input_file}.seman
        )
    endforeach()

endmacro()


macro (TestSemanMacroold lang folder mode)
    get_filename_component(base_name ${folder} NAME)
    set (file_mask ${folder}/*.txt)
    add_test(
        NAME Seman_${lang}__${base_name} 
        COMMAND TestSeman --input-file-mask ${file_mask} ${mode}
        WORKING_DIRECTORY ../../..
    )

    file(GLOB seman_tests ${file_mask})
    foreach (input_file ${seman_tests})
        get_filename_component(base_name ${input_file} NAME)
        add_test(
            NAME Seman_${lang}_${base_name}_cmp 
            COMMAND ${CMAKE_COMMAND} -E compare_files  ${input_file}.seman ${input_file}.seman.canon
            WORKING_DIRECTORY ../../..
        )
    endforeach(input_file)

endmacro()


TestSemanMacro(Russian test/seman "")
TestSemanMacro(Russian test/translate --translate)
TestSemanMacro(Russian test/long --translate)



