cmake_minimum_required(VERSION 3.5)

project(MorphWizardLib)

add_library(MorphWizardLib
wizard.cpp
)

target_link_libraries(MorphWizardLib
    PCRE
    AgramtabLib
)

