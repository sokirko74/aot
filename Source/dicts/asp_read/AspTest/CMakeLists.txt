declare_cmake_min_version()

project(AspTest)

add_executable (${PROJECT_NAME} Main.cpp)

target_link_libraries(${PROJECT_NAME} 
    BinaryDictsLib
)
