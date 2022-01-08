declare_cmake_min_version()

project(TestSynan)

add_executable (${PROJECT_NAME} TestSynan.cpp)

target_link_libraries(${PROJECT_NAME} 
    SynCommonLib
    SynanLib
)

macro (TestSynanMacro lang test_directory)
    set (test_directory_in_project "${PROJECT_SOURCE_DIR}/${test_directory}")
    get_filename_component(test_name ${test_directory} NAME)
    set (test_directory_in_build "${CMAKE_CURRENT_BINARY_DIR}/test")
    file(COPY ${test_directory_in_project} DESTINATION ${test_directory_in_build})
    set (file_mask ${test_directory_in_build}/${test_name}/*.txt )
    add_test(
        NAME Synan_${lang} 
        COMMAND TestSynan --language ${lang} --input-file-mask ${file_mask}
    )
    file(GLOB synan_tests ${file_mask} )
    foreach (input_file ${synan_tests})
        get_filename_component(base_name ${input_file} NAME)
        add_test(
            NAME Synan_${lang}_${base_name}_cmp 
            COMMAND ${CMAKE_COMMAND} -E compare_files ${test_directory_in_project}/${base_name}.synan ${input_file}.synan
        )
    endforeach(input_file)

endmacro(TestSynanMacro)


TestSynanMacro(Russian test/Russian)
TestSynanMacro(German test/German)


