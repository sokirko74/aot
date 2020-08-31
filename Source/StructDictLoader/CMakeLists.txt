declare_cmake_min_version()

project(StructDictLoader)

add_executable (${PROJECT_NAME} Main.cpp)

target_link_libraries(${PROJECT_NAME} 
    StructDictLib
)



macro (ImportExportMacro folder)
    #file(REMOVE [${folder}/units.bin, ${folder}/cortege.bin, ${folder}/comments.bin])
    message (${folder}/units.bin)
    add_test(
        NAME StructDictLoader_1remove_${folder} 
        COMMAND ${CMAKE_COMMAND} -E remove -f  units.bin cortege.bin comments.bin ross.txt.new
        WORKING_DIRECTORY  ${folder}
    )

    add_test(
        NAME StructDictLoader_2fromtxt_${folder} 
        COMMAND ${PROJECT_NAME} FromTxt ross.txt .
        WORKING_DIRECTORY  ${folder}
    )

    add_test(
        NAME StructDictLoader_3totxt_${folder} 
        COMMAND ${PROJECT_NAME} ToTxt ross.txt.new .
        WORKING_DIRECTORY ${folder}
    )

    add_test(
        NAME StructDictLoader_4cmp_${folder} 
        COMMAND ${CMAKE_COMMAND} -E compare_files  ross.txt ross.txt.new
        WORKING_DIRECTORY  ${folder}
    )

endmacro(ImportExportMacro)

ImportExportMacro(${CMAKE_CURRENT_LIST_DIR}/tests/test1)
