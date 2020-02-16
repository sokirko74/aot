cmake_minimum_required(VERSION 3.5)
project(Bigrams)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_test(
        NAME BigramsBuilder
        COMMAND  Text2Bigrams English file.con --only-words
        WORKING_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/test
)

add_test(
        NAME BigramsIndex
        COMMAND  BigramsIndex ${CMAKE_CURRENT_BINARY_DIR}/test/bigrams.txt
)

add_test(
        NAME BigramsTest
        COMMAND  BigramsTest --bigrams ${CMAKE_CURRENT_BINARY_DIR}/test/bigrams.txt --input-word father --language English --output ${CMAKE_CURRENT_BINARY_DIR}/test/output.txt
)

add_test(
        NAME BigramsCompare
        COMMAND ${CMAKE_COMMAND} -E compare_files  ${CMAKE_CURRENT_BINARY_DIR}/test/output.canon  ${CMAKE_CURRENT_BINARY_DIR}/test/output.txt
)

add_subdirectory (Text2Bigrams)
add_subdirectory (BigramsIndex)
add_subdirectory (BigramsTest)