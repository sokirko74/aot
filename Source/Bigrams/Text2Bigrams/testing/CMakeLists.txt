add_executable (TestMergeFiles 
    ../MergeFiles.cpp
    TestMergeFiles.cpp
   )

   
add_test(
        NAME MergeFiles
        COMMAND  TestMergeFiles a1.txt a2.txt a3.txt out.txt
)
