#a subset of iconv library for cp1251 and cp1252
# https://ftp.gnu.org/pub/gnu/libiconv/libiconv-1.16.tar.gz

declare_cmake_min_version()

add_library(iconv_subset
    iconv.cpp
)
