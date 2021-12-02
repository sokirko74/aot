declare_cmake_min_version()

project(MorphGen)

add_executable (${PROJECT_NAME}
        MorphGen.cpp
)

target_link_libraries(${PROJECT_NAME} 
    LemmatizerBaseLib
)


# test morphgen for a simple dictionary
macro (TestMorphGen lang folder)
    get_filename_component(base_name ${folder} NAME)
    add_test(
            NAME MorphGen_${lang}_${base_name}_0
            COMMAND  MorphGen --input project.mwz --output-folder . --postfix-len 5 --min-freq 3  --test-word дума
            WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}/${folder}
    )
    add_test(
            NAME MorphGen_${lang}_${base_name}_1
            COMMAND ${CMAKE_COMMAND} -E compare_files  test_word.morph test_word.morph.canon
            WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}/${folder}
    )
endmacro(TestMorphGen)

TestMorphGen( Russian   tests/Russian)

