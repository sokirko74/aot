declare_cmake_min_version()

project(CheckThesRead)

add_executable (${PROJECT_NAME}
    CheckThesRead.cpp ${PROJECT_SOURCE_DIR}/data
)

target_link_libraries(${PROJECT_NAME}
    AgramtabLib
    RmlThesLib
    StructDictLib
)
