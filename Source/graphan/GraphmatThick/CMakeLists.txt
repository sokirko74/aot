declare_cmake_min_version()

project(GraphmatThick)

add_executable (${PROJECT_NAME} GraphmatThick.cpp)

target_link_libraries(${PROJECT_NAME} 
    GraphanLib
)

