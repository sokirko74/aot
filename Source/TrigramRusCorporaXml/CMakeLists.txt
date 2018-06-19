cmake_minimum_required(VERSION 3.6)

project(TrigramRusCorporaXml)

add_executable (TrigramRusCorporaXml 
main.cpp
RusCorpXml.cpp
)

target_link_libraries(TrigramRusCorporaXml 
    TrigramLib
    tinyxml
)
