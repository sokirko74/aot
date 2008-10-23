cparams := $(cparams) -DUSE_TRIGRAM_LEMMATIZER
link_pcre = 1
include ../common/common_mak

ExeName = ConvertTrigramBinary$(ExeExten)
objects = main.o$(ct)  \
../common/utilit.o$(ct) 

libs = ../TrigramLib/libTrigram$(ct).$(lib_ext) \
../AgramtabLib/libAgramtab$(ct).$(lib_ext) \
../GraphanLib/libGraphan$(ct).$(lib_ext) \
../MorphWizardLib/libMorphWizard$(ct).$(lib_ext) \
../StructDictLib/libStructDict$(ct).$(lib_ext) \
../LemmatizerLib/libLemmatizer$(ct).$(lib_ext)


include ../common/common_exe_mak
