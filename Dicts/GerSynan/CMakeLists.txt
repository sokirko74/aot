cmake_minimum_required(VERSION 3.5)
project (GerNPGLRSynan)

CompileSimpleGrammar(GerSynanGrammar synan.grm ${PROJECT_SOURCE_DIR}/synan.table)
CompileSimpleGrammar(GetPostMorphGrammar postmorph.grm ${PROJECT_SOURCE_DIR}/postmorph.table)


