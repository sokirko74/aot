declare_cmake_min_version()

project(SimpleGrammarPrecompiled)

add_executable (${PROJECT_NAME} SimpleGrammarPrecompiled.cpp)

target_link_libraries(${PROJECT_NAME} 
    SimpleGrammarLib
)

if (BUILD_DICTS)
    add_dependencies (SimpleGrammarPrecompiled MorphDicts)
endif()

