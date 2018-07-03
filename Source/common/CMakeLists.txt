cmake_minimum_required(VERSION 3.5)

set(my_SOURCES
AncodePattern.cpp
BigramsReader.cpp
Homonym.cpp
LemWord.cpp
MorphXmlToken.cpp
PlmLine.cpp
cgic.cpp
gra_descr.cpp
rus_numerals.cpp
string_socket.cpp
utilit.cpp
)

add_library(aot_common  ${my_SOURCES})
