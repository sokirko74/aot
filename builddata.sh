#!/bin/sh
source setrml.sh

cd $RML

for f in Obor Ross Aoss Collocs EngCollocs EngObor GerObor TimeRoss; do 
  run Bin/StructDictLoader FromTxt Dicts/$f/ross.txt  Dicts/$f
done;

run Bin/GraphmatThick Russian Test/Graphan/Rus/text.txt -gra gra
run cmp gra Test/Graphan/Rus/result.gra
run Bin/GraphmatThick German Test/Graphan/Ger/text1.txt -gra gra
run cmp gra Test/Graphan/Ger/result.gra

for f in Comp Fin Omni Loc ; do 
  run Bin/StructDictLoader FromTxt Thes/$f/Ross/ross.txt  Thes/$f/Ross
done;

MORPHOLOGY_LANGUAGES="Rus Ger Eng"
for language in $MORPHOLOGY_LANGUAGES
do
  run Bin/MorphGen Dicts/SrcMorph/$language.mwz Dicts/Morph/$language 5 3
done
run Bin/TestLem -echo -noids -forms Russian <Test/Morph/Rus/test.txt | cmp - Test/Morph/Rus/result.txt
run Bin/TestLem -echo -noids -forms German <Test/Morph/Ger/test.txt  | cmp - Test/Morph/Ger/result.txt
run Bin/TestLem -echo -noids -forms English <Test/Morph/Eng/test.txt | cmp - Test/Morph/Eng/result.txt

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
Bin/TestSeman <Test/Seman/Rus/test.txt | cmp - Test/Seman/Rus/result.txt

echo All done.