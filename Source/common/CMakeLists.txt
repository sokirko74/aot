declare_cmake_min_version()

set(my_SOURCES
        Chunk.h
        http_server.h
        test_corpus.h
        test_corpus.cpp
)

add_library(aot_common ${my_SOURCES})

target_link_libraries(aot_common
        morphology_common
        )
