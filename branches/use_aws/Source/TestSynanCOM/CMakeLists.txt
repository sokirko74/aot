project(TestSynanCOM)

add_executable (${PROJECT_NAME} 
    TestSynanCOM.cpp
     ../common/COM/COMSyntaxHolder.cpp
)

add_dependencies(${PROJECT_NAME}
    SynAn
)


target_link_libraries(${PROJECT_NAME}
    aot_common
)
