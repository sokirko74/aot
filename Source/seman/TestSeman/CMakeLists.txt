declare_cmake_min_version()

project(TestSeman)

add_executable (TestSeman TestSeman.cpp)

target_link_libraries(TestSeman 
    SemanLib
)

macro (TestSemanMacro lang folder mode)
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


TestSemanMacro(Russian ${PROJECT_SOURCE_DIR}/test/seman "")
TestSemanMacro(Russian ${PROJECT_SOURCE_DIR}/test/translate --translate)
TestSemanMacro(Russian ${PROJECT_SOURCE_DIR}/test/long --translate)



