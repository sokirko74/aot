declare_cmake_min_version()

project(MorphGen)

add_executable (${PROJECT_NAME} MorphGen.cpp)

target_link_libraries(${PROJECT_NAME} 
    LemmatizerLib
)

add_dependencies (${PROJECT_NAME} TestLem)

macro (TestMorphGen lang path)
    get_filename_component(base_name ${path} NAME)
    get_filename_component(folder ${path} DIRECTORY)
    message (${folder})
    add_test(
        NAME MorphGen_${lang}_${base_name}_0 
        COMMAND MorphGen --input ${base_name} --output-folder . --postfix-len 5 --min-freq 3
        WORKING_DIRECTORY  ${folder}
    )
                            

    add_test(
        NAME MorphGen_${lang}_${base_name}_1
        COMMAND TestLem --RML ${folder} --language ${lang} --input test.txt --output test.txt.morph
        WORKING_DIRECTORY  ${folder}
    )

    add_test(
        NAME MorphGen_${lang}_${base_name}_2
        COMMAND ${CMAKE_COMMAND} -E compare_files  test.txt.morph test.txt.morph.canon
        WORKING_DIRECTORY  ${folder}
    )
endmacro(TestMorphGen)

TestMorphGen( Russian   ${PROJECT_SOURCE_DIR}/tests/Russian/project.mwz)
