add_executable (TestMergeFiles 
    ../MergeFiles.cpp
    TestMergeFiles.cpp
   )

   
message( "======= add test for ${PROJECT_SOURCE_DIR} =========")
add_test(
        NAME MergeFiles
        COMMAND  TestMergeFiles a1.txt a2.txt a3.txt out.txt
        WORKING_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR}
)
