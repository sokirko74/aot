cmake_minimum_required(VERSION 3.6)

project(tinyxml)

add_library(tinyxml
action_store.cpp
htmlutil.cpp
lex_util.cpp
main.cpp
node_set.cpp
tinyxml.cpp
tinyxmlerror.cpp
tinyxmlparser.cpp
tokenlist.cpp
xml_util.cpp
xpath_expression.cpp
xpath_processor.cpp
xpath_stack.cpp
xpath_static.cpp
xpath_stream.cpp
xpath_syntax.cpp
)

target_link_libraries(tinyxml
    aot_common
)

