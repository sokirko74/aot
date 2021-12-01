declare_cmake_min_version()

project(JNIMorphAPITest)

add_executable (JNIMorphAPITest JNIMorphTest.cpp)

target_link_libraries(JNIMorphAPITest
    JNIMorphAPI
)
