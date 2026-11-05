declare_cmake_min_version()

# http servers use the header-only cpp-httplib (Source/contrib/httplib)
find_package(Threads REQUIRED)

if ($JAVA_INCLUDES)  # not tested under cmake
    add_subdirectory (www/JNIMorphAPI)
    add_subdirectory (www/JNIMorphAPITest)
endif()

add_subdirectory (SynanDaemon)
add_subdirectory (SemanDaemon)
