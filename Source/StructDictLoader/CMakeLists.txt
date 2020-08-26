cmake_minimum_required(VERSION 3.5)

project(StructDictLoader)

add_executable (${PROJECT_NAME} Main.cpp)

target_link_libraries(${PROJECT_NAME} 
    StructDictLib
)



macro (ImportExportMacro folder)
    file(REMOVE [${folder}/units.bin, ${folder}/cortege.bin, ${folder}/comments.bin])
    add_test(
        NAME StructDictLoader_fromtxt_${folder} 
        COMMAND ${PROJECT_NAME} FromTxt ross.txt .
        WORKING_DIRECTORY  ${folder}
    )

    add_test(
        NAME StructDictLoader_totxt_${folder} 
        COMMAND ${PROJECT_NAME} ToTxt ross.txt.new .
        WORKING_DIRECTORY ${folder}
    )

    add_test(
        NAME StructDictLoader_${folder}_cmp 
        COMMAND ${CMAKE_COMMAND} -E compare_files  ross.txt ross.txt.new
        WORKING_DIRECTORY  ${folder}
    )

endmacro(ImportExportMacro)

ImportExportMacro(${CMAKE_CURRENT_LIST_DIR}/tests/test1)
