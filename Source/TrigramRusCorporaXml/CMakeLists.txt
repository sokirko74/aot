declare_cmake_min_version()

project(TrigramRusCorporaXml)

add_executable (TrigramRusCorporaXml 
main.cpp
RusCorpXml.cpp
)

target_link_libraries(TrigramRusCorporaXml 
    TrigramLib
    tinyxml
)
