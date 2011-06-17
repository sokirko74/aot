link_pcre = 1

VPATH = ../

include ../common/common_mak

common_parms := -I. $(JAVA_INCLUDES)

include ../common/common_mak

LibName = JNIMorphAPI


objects = JNIMorphAPI.o$(ct) ../common/utilit.o$(ct) \
../LemmatizerLib/Lemmatizers.o$(ct) \
../LemmatizerLib/Paradigm.o$(ct) \
../LemmatizerLib/CreatePredictionBase.o$(ct) \
../LemmatizerLib/PLMLineCollection.o$(ct) \
../LemmatizerLib/Statistic.o$(ct) \
../LemmatizerLib/MorphAutomat.o$(ct) \
../LemmatizerLib/MorphAutomBuilder.o$(ct) \
../LemmatizerLib/MorphDict.o$(ct) \
../LemmatizerLib/MorphDictBuilder.o$(ct) \
../LemmatizerLib/../common/PlmLine.o$(ct) \
../LemmatizerLib/Predict.o$(ct) \
../LemmatizerLib/../common/MorhologyHolder.o$(ct) \
../AgramtabLib/EngGramTab.o$(ct) \
../AgramtabLib/GerGramTab.o$(ct) \
../AgramtabLib/RusGramTab.o$(ct) \
../AgramtabLib/agramtab_.o$(ct) \
../MorphWizardLib/wizard.o$(ct) ../PCRE/pcre_rml.o$(ct) \
../StructDictLib/ItemsContainer.o$(ct) \
../StructDictLib/Ross.o$(ct) \
../StructDictLib/TempArticle.o$(ct) \
../GraphanLib/C_desc.o$(ct) \
../GraphanLib/Consent.o$(ct) \
../GraphanLib/../common/gra_descr.o$(ct) \
../GraphanLib/Cs_hiera.o$(ct) \
../GraphanLib/GraphmatFile.o$(ct) \
../GraphanLib/HtmlConv.o$(ct) \
../GraphanLib/Nc_desc.o$(ct) \
../GraphanLib/Writegra.o$(ct) \
../GraphanLib/abbrev.o$(ct) \
../GraphanLib/graline.o$(ct) \
../GraphanLib/Oborots.o$(ct) \
../GraphanLib/UnitHolder.o$(ct) \
../GraphanLib/GraphanDicts.o$(ct) \
../GraphanLib/Sentences.o$(ct) \
$(PCRE_OBJECTS_LIB)/pcre_chartables.o \
$(PCRE_OBJECTS_LIB)/pcre_compile.o \
$(PCRE_OBJECTS_LIB)/pcre_config.o \
$(PCRE_OBJECTS_LIB)/pcrecpp.o \
$(PCRE_OBJECTS_LIB)/pcre_dfa_exec.o \
$(PCRE_OBJECTS_LIB)/pcre_exec.o \
$(PCRE_OBJECTS_LIB)/pcre_fullinfo.o \
$(PCRE_OBJECTS_LIB)/pcre_get.o \
$(PCRE_OBJECTS_LIB)/pcre_globals.o \
$(PCRE_OBJECTS_LIB)/pcre_info.o \
$(PCRE_OBJECTS_LIB)/pcre_maketables.o \
$(PCRE_OBJECTS_LIB)/pcre_newline.o \
$(PCRE_OBJECTS_LIB)/pcre_ord2utf8.o \
$(PCRE_OBJECTS_LIB)/pcreposix.o \
$(PCRE_OBJECTS_LIB)/pcre_refcount.o \
$(PCRE_OBJECTS_LIB)/pcre_scanner.o \
$(PCRE_OBJECTS_LIB)/pcre_stringpiece.o \
$(PCRE_OBJECTS_LIB)/pcre_study.o \
$(PCRE_OBJECTS_LIB)/pcre_tables.o \
$(PCRE_OBJECTS_LIB)/pcre_try_flipped.o \
$(PCRE_OBJECTS_LIB)/pcre_ucd.o \
$(PCRE_OBJECTS_LIB)/pcre_valid_utf8.o \
$(PCRE_OBJECTS_LIB)/pcre_version.o \
$(PCRE_OBJECTS_LIB)/pcre_xclass.o

libs = 

include ../common/common_lib_mak
