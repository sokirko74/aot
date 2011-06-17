#!/bin/sh

run()
{
    $@;
    if [ "$?" -ne "0" ]; then
	  echo "Fail running $@ !";
	  exit 1;
    fi
}


if [ -z $RML ]; then
  RML=$(dirname $(realpath $0))
  if [ -z $RML ]; then
   # still empty: try to detect absolute path using Perl
   # (Cwd is in Perl's core distribution, this should not fail)
   RML=`perl '-MCwd qw(abs_path)' -e 'print abs_path'`
  fi;
  export RML=$RML
fi;

if [ -z $RML_PCRE_INCLUDE ]; then
  if [ -e "/usr/include/pcre.h" ]; then
    RML_PCRE_INCLUDE="/usr/include"
    export RML_PCRE_INCLUDE=$RML_PCRE_INCLUDE
  else
    echo "Please, set RML_PCRE_INCLUDE variable or install libpcre-dev!"
    exit;
  fi
fi

if [ -z $RML_PCRE_LIB ]; then
  if [ -e "/usr/lib/libpcre.so" ]; then
    RML_PCRE_LIB="/usr/lib"
    export RML_PCRE_LIB=$RML_PCRE_LIB
  else
    echo "Please, set RML_PCRE_LIB variable or install libpcre-dev!"
    exit;
  fi
fi

echo "RML=$RML"
echo "RML_PCRE_LIB=$RML_PCRE_LIB"
echo "RML_PCRE_INCLUDE=$RML_PCRE_INCLUDE"

make_tool=gmake
cd $RML

#compile struct dicts
run $make_tool -C $RML/Source/StructDictLoader mode=release

for f in Obor Ross Aoss Collocs EngCollocs EngObor GerObor TimeRoss; do 
  run Bin/StructDictLoader FromTxt Dicts/$f/ross.txt  Dicts/$f
done;


for f in Comp Fin Omni Loc ; do 
  run Bin/StructDictLoader FromTxt Thes/$f/Ross/ross.txt  Thes/$f/Ross
done;

#compile graphematics
run $make_tool -C $RML/Source/GraphmatThick mode=release
run Bin/GraphmatThick Russian Test/Graphan/Rus/text.txt -gra gra
run cmp gra Test/Graphan/Rus/result.gra
run Bin/GraphmatThick German Test/Graphan/Ger/text1.txt -gra gra
run cmp gra Test/Graphan/Ger/result.gra


#compile morphology
MORPHOLOGY_COMPONENTS="MorphGen TestLem FileLem"
for component in $MORPHOLOGY_COMPONENTS
do
  run $make_tool -C $RML/Source/$component mode=release
done

MORPHOLOGY_LANGUAGES="Rus Ger Eng"
for language in $MORPHOLOGY_LANGUAGES
do
  run $RML/Source/MorphGen/MorphGen Dicts/SrcMorph/$language.mwz Dicts/Morph/$language 5 3
done
run Bin/TestLem Russian <Test/Morph/Rus/test.txt | cmp - Test/Morph/Rus/result.txt
run Bin/TestLem German <Test/Morph/Ger/test.txt  | cmp - Test/Morph/Ger/result.txt
run Bin/TestLem English <Test/Morph/Eng/test.txt | cmp - Test/Morph/Eng/result.txt


#compile synan
run $make_tool -C $RML/Source/TestSynan mode=release
run $make_tool -C $RML/Source/ConvertTrigramBinary mode=release
run $make_tool -C $RML/Source/SimpleGrammarPrecompiled mode=release
run $make_tool -C $RML/Source/SynanDaemon mode=release_thread
run $make_tool -C $RML/Source/SynanCGI  mode=release_thread
d=Dicts/Trigram/full.rev/
run gzip -cd $d/base.lex.gz >$d/base.lex
run gzip -cd $d/base.ngram.gz >$d/base.ngram
run Bin/ConvertTrigramBinary  Dicts/Trigram/full.rev.config
run Bin/TestSynan Russian Test/Synan/Rus/test.txt | cmp - Test/Synan/Rus/result.txt

#german syntax
GERMAN_SYNTAX="GerSynan/gformats.txt
SimpleGrammar/example.grm
SimpleGrammar/person.grm
"
for component in $GERMAN_SYNTAX
do
  run Bin/SimpleGrammarPrecompiled German Dicts/$component
done
run Bin/TestSynan German Test/Synan/Rus/test.txt | cmp - Test/Synan/Ger/result.txt 	

#compile seman
SEMAN="TestSeman AprDictGen asp_read deriv_read GenFreqDict StatDatBin WordDatBin GenSynDict"
for component in $SEMAN
do
  run $make_tool -C $RML/Source/$component mode=release
done
#run $make_tool -C $RML/Source/SemanDaemon mode=release_thread libmode=shared 
#run $make_tool -C $RML/Source/translate mode=release_thread libmode=shared
run Bin/AprDictGen Dicts/SrcBinDict/dict2809.txt Dicts/BinDict/dict.bin > /dev/null
run Bin/asp_read Dicts/SrcBinDict/ASP.txt Dicts/BinDict/asp_dict.bin >/dev/null 2>/dev/null
run Bin/deriv_read Dicts/SrcBinDict/troiki_suff Dicts/BinDict/deriv_suff.bin  >/dev/null 2>/dev/null
run Bin/deriv_read Dicts/SrcBinDict/troiki_pref Dicts/BinDict/deriv_pref.bin  >/dev/null 2>/dev/null
for freq_dict in freq_comp freq_fin freq_hud; do
  run Bin/GenFreqDict Texts/$freq_dict.txt Dicts/BinDict/$freq_dict.bin Dicts/SrcBinDict/shira.txt > /dev/null 2> /dev/null
done;
for d in L C F; do 
  run Bin/StatDatBin  Russian  Dicts/SrcBinDict/StatData.rus -$d Dicts/Morph/Rus/;
  run Bin/StatDatBin  German   Dicts/SrcBinDict/StatData.ger -$d Dicts/Morph/Ger;
done;
for d in L C F; do
  run Bin/WordDatBin  Dicts/SrcBinDict/WordData.txt -$d Dicts/Morph/Rus/ >/dev/null
done;
run Bin/GenSynDict  Dicts/SrcBinDict/synonyms.txt Dicts/BinDict/synonyms.bin 2>/dev/null

