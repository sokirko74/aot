declare_cmake_min_version()

project(TestMapost)

add_executable (${PROJECT_NAME} TestMapost.cpp)

target_link_libraries(${PROJECT_NAME} 
    LemmatizerLib
    MAPostLib
    SynanLib
)

macro (TestMapostMacro lang test_directory)
    set (test_directory_in_project "${PROJECT_SOURCE_DIR}/${test_directory}")
    get_filename_component(test_name ${test_directory} NAME)
    set (test_directory_in_build "${CMAKE_CURRENT_BINARY_DIR}/test")
    file(COPY ${test_directory_in_project} DESTINATION ${test_directory_in_build})
    set (file_mask ${test_directory_in_build}/${test_name}/*.txt )

    add_test(
        NAME Mapost_${lang} 
        COMMAND TestMapost --language ${lang} --input-file-mask ${file_mask}
    )

    file(GLOB tests ${file_mask})
    foreach (input_file ${tests})
        get_filename_component(base_name ${input_file} NAME)
        add_test(
            NAME Mapost_${lang}_${base_name}_cmp 
            COMMAND ${CMAKE_COMMAND} -E compare_files  ${test_directory_in_project}/${base_name}.mapost ${input_file}.mapost
        )
    endforeach()
endmacro()

TestMapostMacro(Russian test/Russian)


if (BUILD_DICTS)
    add_dependencies (TestLem MorphDicts)
endif()

