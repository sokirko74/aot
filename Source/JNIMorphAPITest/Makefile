link_pcre = 1

VPATH = ../

include ../common/common_mak

common_parms := -I. $(JAVA_INCLUDES)

include ../common/common_mak

ExeName = JNIMorphTest$(ExeExten)

objects = JNIMorphTest.o$(ct) ../JNIMorphAPI/JNIMorphAPI.o$(ct) ../common/utilit.o$(ct)

libs = -ldl ../LemmatizerLib/libLemmatizer$(ct).$(lib_ext) \
../AgramtabLib/libAgramtab$(ct).$(lib_ext) \
../GraphanLib/libGraphan$(ct).$(lib_ext) \
../StructDictLib/libStructDict$(ct).$(lib_ext) \
../MorphWizardLib/libMorphWizard$(ct).$(lib_ext) 

include ../common/common_exe_mak
