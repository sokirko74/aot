declare_cmake_min_version()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
find_package(Libevent REQUIRED)
message ("LIBEVENT_INCLUDE =  ${LIBEVENT_INCLUDE_DIR}" )
message ("LIBEVENT_LIBRARY =  ${LIBEVENT_LIBRARY}" )

if ($JAVA_INCLUDES)  # not tested under cmake
    add_subdirectory (www/JNIMorphAPI)
    add_subdirectory (www/JNIMorphAPITest)
endif()

add_subdirectory (SynanDaemon)
add_subdirectory (SemanDaemon)
