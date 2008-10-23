# common_parms := -I. -I../AgramtabLib -I../GraphanLib -I../StructDictLib

VPATH = ../

include ../common/common_mak


common_parms := -I.
include ../common/common_mak
LibName = libLemmatizer$(ct)


objects = Lemmatizers.o$(ct) \
Paradigm.o$(ct) \
CreatePredictionBase.o$(ct) \
PLMLineCollection.o$(ct) \
Statistic.o$(ct) \
MorphAutomat.o$(ct) \
MorphAutomBuilder.o$(ct) \
MorphDict.o$(ct) \
MorphDictBuilder.o$(ct) \
../common/PlmLine.o$(ct) \
Predict.o$(ct) \
../common/MorhologyHolder.o$(ct)

include ../common/common_lib_mak
