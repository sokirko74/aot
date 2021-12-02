declare_cmake_min_version()

project(MorphGenTest)

add_executable (${PROJECT_NAME}
   TestMorphGen.cpp
)

target_link_libraries(${PROJECT_NAME} 
    LemmatizerBaseLib
)

# test morphgen for a simple dictionary
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
            COMMAND MorphGenTest ${folder} "дума"
            WORKING_DIRECTORY  ${folder}
    )
endmacro(TestMorphGen)

TestMorphGen( Russian   ${PROJECT_SOURCE_DIR}/Russian/project.mwz)

