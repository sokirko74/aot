set(CMAKE_LEGACY_CYGWIN_WIN32 0)
cmake_minimum_required(VERSION 3.5)

add_custom_target(
    TrigramData
    SOURCES full.rev/buckets.bin
)

add_custom_command(
    OUTPUT full.rev/buckets.bin
    COMMAND ConvertTrigramBinary full.rev.config
    DEPENDS ConvertTrigramBinary full.rev/base.ngram full.rev.config full.rev/base.lex
    COMMENT "Convert Trigram data (Russian)"
    VERBATIM
)

add_custom_command(
    OUTPUT full.rev/base.lex
    DEPENDS full.rev/base.lex.gz
    COMMAND gzip -cd full.rev/base.lex.gz > full.rev/base.lex
)

add_custom_command(
    OUTPUT full.rev/base.ngram
    DEPENDS full.rev/base.ngram.gz
    COMMAND gzip -cd full.rev/base.ngram.gz > full.rev/base.ngram
)


