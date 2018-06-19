cmake_minimum_required(VERSION 3.5)

project(JNIMorphAPITest)

add_executable (JNIMorphAPITest JNIMorphTest.cpp)

target_link_libraries(JNIMorphAPITest
    JNIMorphAPI
)
