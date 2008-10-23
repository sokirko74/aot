cparams := $(cparams) -DUSE_TRIGRAM_LEMMATIZER
common_parms := -I.
include ../common/common_mak
LibName = libMAPost$(ct)
objects = \
MAPostInit.o$(ct) \
MAPostMain.o$(ct) \
PostLemWord.o$(ct) \
../common/AncodePattern.o$(ct) \
../common/Homonym.o$(ct) \
../common/LemWord.o$(ct) \
Fio.o$(ct) \
../common/rus_numerals.o$(ct)

include ../common/common_lib_mak

