#!/bin/sh
source setrml.sh

make_tool=make
echo $* | grep -vq "\-\-VS" || make_tool="echo Skipping: make "
cd $RML

#compile struct dicts
run $make_tool -C $RML/Source/StructDictLoader mode=release
run $make_tool -C $RML/Source/GraphmatThick mode=release

#compile morphology
MORPHOLOGY_COMPONENTS="MorphGen TestLem FileLem"
for component in $MORPHOLOGY_COMPONENTS
do
  run $make_tool -C $RML/Source/$component mode=release
done

#compile synan
run $make_tool -C $RML/Source/TestSynan mode=release
run $make_tool -C $RML/Source/ConvertTrigramBinary mode=release
run $make_tool -C $RML/Source/SimpleGrammarPrecompiled mode=release
run $make_tool -C $RML/Source/SynanDaemon mode=release_thread
run $make_tool -C $RML/Source/SynanCGI  mode=release_thread

#compile seman
SEMAN="TestSeman AprDictGen asp_read deriv_read GenFreqDict StatDatBin WordDatBin GenSynDict"
for component in $SEMAN
do
  run $make_tool -C $RML/Source/$component mode=release
done
#run $make_tool -C $RML/Source/SemanDaemon mode=release_thread libmode=shared 
#run $make_tool -C $RML/Source/translate mode=release_thread libmode=shared

echo All done.