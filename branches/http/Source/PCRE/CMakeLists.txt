cmake_minimum_required(VERSION 3.5)

project(PCRE)


add_library(PCRE
pcre_chartables.cpp
pcre_compile.cpp
pcre_config.cpp
pcre_dfa_exec.cpp
pcre_exec.cpp
pcre_fullinfo.cpp
pcre_get.cpp
pcre_globals.cpp
pcre_info.cpp
pcre_maketables.cpp
pcre_newline.cpp
pcre_ord2utf8.cpp
pcre_refcount.cpp
pcre_rml.cpp
pcre_study.cpp
pcre_tables.cpp
pcre_try_flipped.cpp
pcre_ucp_searchfuncs.cpp
pcre_valid_utf8.cpp
pcre_version.cpp
pcre_xclass.cpp
pcreposix.cpp
)

target_compile_definitions(PCRE PUBLIC HAVE_CONFIG_H=1)
