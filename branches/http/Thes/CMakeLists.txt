set(CMAKE_LEGACY_CYGWIN_WIN32 0)
cmake_minimum_required(VERSION 3.5)

add_subdirectory (Comp/Ross)
add_subdirectory (Loc/Ross)
add_subdirectory (Fin/Ross)
add_subdirectory (Omni/Ross)

add_custom_target ( 
    ThesRosses
    DEPENDS OmniRoss FinRoss CompRoss LocRoss
)
