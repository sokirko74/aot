cmake_minimum_required(VERSION 3.5)

project(TestSynan)

add_executable (${PROJECT_NAME} TestSynan.cpp)

target_link_libraries(${PROJECT_NAME} 
    SynCommonLib
    SynanLib
)

macro (TestSynanMacro lang file_mask)
    add_test(
        NAME Synan_${lang} 
        COMMAND TestSynan --language ${lang} --input-file-mask ${file_mask}
        WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}
    )

    file(GLOB synan_tests ${file_mask})
    foreach (input_file ${synan_tests})
        get_filename_component(base_name ${input_file} NAME)
        add_test(
            NAME Synan_${lang}_${base_name}_cmp 
            COMMAND ${CMAKE_COMMAND} -E compare_files  ${input_file}.synan ${input_file}.synan.canon
            WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}
        )
    endforeach(input_file)

endmacro(TestSynanMacro)


TestSynanMacro(Russian ${PROJECT_SOURCE_DIR}/test/Russian/*.txt)
TestSynanMacro(German ${PROJECT_SOURCE_DIR}/test/German/*.txt)

#CopyExe(${PROJECT_NAME})
