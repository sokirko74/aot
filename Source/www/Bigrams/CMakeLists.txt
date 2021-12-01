declare_cmake_min_version()
project(Bigrams)

file(COPY test DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

#build bigrams.txt
add_test(
        NAME Bigrams.1.Builder
        COMMAND Text2Bigrams English file.con --only-words
        WORKING_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/test
)

add_test(
        NAME Bigrams.2.Index
        COMMAND BigramsIndex ${CMAKE_CURRENT_BINARY_DIR}/test/bigrams.txt
)

add_test(
        NAME Bigrams.3.Test
        COMMAND BigramsTest --bigrams ${CMAKE_CURRENT_BINARY_DIR}/test/bigrams.txt --input-word father --language English --output ${CMAKE_CURRENT_BINARY_DIR}/test/output.txt
)

add_test(
        NAME Bigrams.4.Compare
        COMMAND ${CMAKE_COMMAND} -E compare_files  ${CMAKE_CURRENT_BINARY_DIR}/test/output.canon  ${CMAKE_CURRENT_BINARY_DIR}/test/output.txt
)

add_subdirectory (Text2Bigrams)
add_subdirectory (BigramsIndex)
add_subdirectory (BigramsTest)