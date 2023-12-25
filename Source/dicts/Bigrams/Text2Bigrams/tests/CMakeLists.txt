project(test_merge_files)

add_executable (test_merge_files
        ../MergeFiles.cpp
        test_merge_files.cpp
)

add_test(
        NAME MergeFilesTest
        COMMAND  test_merge_files
)

target_link_libraries(test_merge_files
        aot_common
)

