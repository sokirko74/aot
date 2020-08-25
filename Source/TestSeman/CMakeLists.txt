cmake_minimum_required(VERSION 3.5)

project(TestSeman)

add_executable (TestSeman TestSeman.cpp)

target_link_libraries(TestSeman 
    SemanLib
)

macro (TestSemanMacro lang file_mask)
    add_test(
        NAME Seman_${lang} 
        COMMAND TestSeman --input-file-mask ${file_mask}
        WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}
    )

    file(GLOB seman_tests ${file_mask})
    foreach (input_file ${seman_tests})
        get_filename_component(base_name ${input_file} NAME)
        add_test(
            NAME Seman_${lang}_${base_name}_cmp 
            COMMAND ${CMAKE_COMMAND} -E compare_files  ${input_file}.seman ${input_file}.seman.canon
            WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}
        )
    endforeach(input_file)

endmacro()


TestSemanMacro(Russian ${PROJECT_SOURCE_DIR}/test/Russian/*.txt)



