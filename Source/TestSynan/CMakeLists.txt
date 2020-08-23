cmake_minimum_required(VERSION 3.5)

project(TestSynan)

add_executable (TestSynan TestSynan.cpp)

target_link_libraries(TestSynan 
    SynCommonLib
    SynanLib
)

macro (TestSynanMacro lang filename)
    get_filename_component(base_name ${filename} NAME)
    add_test(
        NAME Synan_${lang}_${base_name} 
        COMMAND TestSynan --language ${lang} --input ${filename} --output ${filename}.synan
        WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}
    )
    add_test(
        NAME Synan_${lang}_${base_name}_cmp 
        COMMAND ${CMAKE_COMMAND} -E compare_files  ${filename}.synan ${filename}.synan.canon
        WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}
    )
endmacro(TestSynanMacro)


file(GLOB synan_tests ${PROJECT_SOURCE_DIR}/test/Russian/*.txt)
foreach (input_file ${synan_tests})
    TestSynanMacro(Russian ${input_file})
endforeach(input_file)

TestSynanMacro(German test/German/test.txt)

