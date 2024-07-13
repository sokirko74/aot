declare_cmake_min_version()
project(BigramsLib)

add_library(${PROJECT_NAME}
  BigramsReader.cpp 
  BigramsReader.h
)

target_link_libraries(${PROJECT_NAME}
    aot_common
)




file(COPY test_data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

#build bigrams.txt
add_test(
        NAME Bigrams.1.Builder
        COMMAND Text2Bigrams --language English --input-file-list file.con --only-words --output-folder .
        WORKING_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/test_data
)

add_test(
        NAME Bigrams.2.Index
        COMMAND BigramsIndex ${CMAKE_CURRENT_BINARY_DIR}/test_data
)

add_test(
        NAME Bigrams.3.Test
        COMMAND BigramsTest --data-folder ${CMAKE_CURRENT_BINARY_DIR}/test_data --input-word father --language English --output ${CMAKE_CURRENT_BINARY_DIR}/output.txt
)

add_test(
        NAME Bigrams.4.Compare
        COMMAND ${CMAKE_COMMAND} -E compare_files  ${CMAKE_CURRENT_BINARY_DIR}/test_data/output.canon  ${CMAKE_CURRENT_BINARY_DIR}/output.txt
)

add_subdirectory (Text2Bigrams)
add_subdirectory (BigramsIndex)
add_subdirectory (BigramsTest)
