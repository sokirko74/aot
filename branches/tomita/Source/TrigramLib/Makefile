cparams := $(cparams) -DUSE_TRIGRAM_LEMMATIZER

include ../common/common_mak

LibName = libTrigram$(ct)

objects =  TrigramModel.o$(ct) \
TagSet.o$(ct) \
NGramFile.o$(ct) \
ReadLexFile.o$(ct) \
TrigramSerialize.o$(ct) \
TrigramLemmatizer.o$(ct) \
ViterbiInfo.o$(ct) \
../common/MorphXmlToken.o$(ct) \
Viterbi.o$(ct)

include ../common/common_lib_mak


