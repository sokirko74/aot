project(tests)

add_subdirectory("init_tcl_tk")

add_test(
    NAME test_tcl_work 
    COMMAND test_tcl
    WORKING_DIRECTORY  ${CMAKE_SOURCE_DIR}/Bin
)

