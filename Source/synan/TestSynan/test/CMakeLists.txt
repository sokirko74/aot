declare_cmake_min_version()

project(CTestSynan)

macro (TestSynanMacro lang test_directory)
    set (test_directory_in_project "${PROJECT_SOURCE_DIR}/${test_directory}")
    get_filename_component(test_name ${test_directory} NAME)

    set (test_directory_in_build "${CMAKE_CURRENT_BINARY_DIR}/${test_directory}")
    file(MAKE_DIRECTORY ${test_directory_in_build})
    #file(COPY ${test_directory_in_project} DESTINATION ${test_directory_in_build})
    set (file_mask ${test_directory_in_project}/*.txt )
    add_test(
        NAME Synan_${lang} 
        COMMAND TestSynan --language ${lang} --input-file-mask ${file_mask} --output-folder ${test_directory_in_build}
    )
    file(GLOB synan_tests ${file_mask} )
    foreach (input_file ${synan_tests})
        get_filename_component(base_name ${input_file} NAME)
        set (canon ${test_directory_in_project}/${base_name}.synan)
        set (res ${test_directory_in_build}/${base_name}.synan)
        #message("compare ${canon} to  ${res}")

        add_test(
            NAME Synan_${lang}_${base_name}_cmp
            COMMAND ${CMAKE_COMMAND} -E compare_files ${canon} ${res}
        )
    endforeach(input_file)

endmacro(TestSynanMacro)


TestSynanMacro(Russian synan/Russian)

TestSynanMacro(German synan/German)

add_subdirectory (thes)

add_test(
        NAME CheckThesRead 
        COMMAND CheckThesRead ${PROJECT_SOURCE_DIR}/thes/data
)



