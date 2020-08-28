    cmake_minimum_required(VERSION 3.5)

set(my_SOURCES
AncodePattern.cpp
BigramsReader.cpp
Homonym.cpp
LemWord.cpp
MorphXmlToken.cpp
PlmLine.cpp
gra_descr.cpp
rus_numerals.cpp
utilit.cpp
)

if (COMPILE_WITH_HTTP)
    set(my_SOURCES ${my_SOURCES} http_server.cpp)
endif()

add_library(aot_common  ${my_SOURCES})

target_link_libraries(aot_common 
    iconv_subset
)
