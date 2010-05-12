run()
{
    $@;
    if [ "$?" -ne "0" ]; then
	  echo "Fail running $@ !";
	  exit;
    fi
}


if [ -z $RML ]; then
  RML=$(dirname $(realpath $0))
  echo "RML=$RML"
fi;

if [ -z $RML_PCRE_INCLUDE ]; then
  if [ -e "/usr/include/pcre.h" ]; then
    RML_PCRE_INCLUDE="/usr/include"
    echo "RML_PCRE_INCLUDE=$RML_PCRE_INCLUDE"
  else
    echo "Please, set RML_PCRE_INCLUDE variable or install libpcre-dev!"
    exit;
  fi
fi

if [ -z $RML_PCRE_LIB ]; then
  if [ -e "/usr/lib/libpcre.so" ]; then
    RML_PCRE_LIB="/usr/lib"
    echo "RML_PCRE_LIB=$RML_PCRE_LIB"
  else
    echo "Please, set RML_PCRE_LIB variable or install libpcre-dev!"
    exit;
  fi
fi


make_tool=gmake


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
run $make_tool -C $RML/Source/MorphGen mode=release
run $make_tool -C $RML/Source/TestLem mode=release
run $make_tool -C $RML/Source/FileLem mode=release
run $RML/Source/MorphGen/MorphGen Dicts/SrcMorph/Rus.mwz Dicts/Morph/Rus 5 3
run $RML/Source/MorphGen/MorphGen Dicts/SrcMorph/Ger.mwz Dicts/Morph/Ger 5 3
run $RML/Source/MorphGen/MorphGen Dicts/SrcMorph/Eng.mwz Dicts/Morph/Eng 5 3
run Bin/TestLem Russian <Test/Morph/Rus/test.txt  | cmp - Test/Morph/Rus/result.txt
run Bin/TestLem German <Test/Morph/Ger/test.txt  | cmp - Test/Morph/Ger/result.txt

run Bin/TestLem English <Test/Morph/Eng/test.txt  | cmp - Test/Morph/Eng/result.txt


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
run Bin/SimpleGrammarPrecompiled German Dicts/GerSynan/gformats.txt
run Bin/SimpleGrammarPrecompiled German Dicts/SimpleGrammar/example.grm
run Bin/SimpleGrammarPrecompiled German Dicts/SimpleGrammar/person.grm
run Bin/TestSynan German Test/Synan/Rus/test.txt | cmp - 	

#compile seman
run $make_tool -C $RML/Source/TestSeman mode=release
run $make_tool -C $RML/Source/AprDictGen mode=release
run $make_tool -C $RML/Source/asp_read mode=release
run $make_tool -C $RML/Source/deriv_read mode=release
run $make_tool -C $RML/Source/GenFreqDict mode=release
run $make_tool -C $RML/Source/StatDatBin  mode=release
run $make_tool -C $RML/Source/WordDatBin mode=release
run $make_tool -C $RML/Source/GenSynDict mode=release
#run $make_tool -C $RML/Source/SemanDaemon mode=release_thread libmode=shared 
#run $make_tool -C $RML/Source/translate mode=release_thread libmode=shared
run Bin/AprDictGen Dicts/SrcBinDict/dict2809.txt Dicts/BinDict/dict.bin > /dev/null
run Bin/asp_read Dicts/SrcBinDict/ASP.txt Dicts/BinDict/asp_dict.bin >/dev/null 2>/dev/null
run Bin/deriv_read Dicts/SrcBinDict/troiki_suff Dicts/BinDict/deriv_suff.bin  >/dev/null 2>/dev/null
run Bin/deriv_read Dicts/SrcBinDict/troiki_pref Dicts/BinDict/deriv_pref.bin  >/dev/null 2>/dev/null
run Bin/GenFreqDict Texts/freq_comp.txt Dicts/BinDict/freq_comp.bin Dicts/SrcBinDict/shira.txt > /dev/null 2> /dev/null
run Bin/GenFreqDict Texts/freq_fin.txt  Dicts/BinDict/freq_fin.bin  Dicts/SrcBinDict/shira.txt > /dev/null 2> /dev/null 
run Bin/GenFreqDict Texts/freq_hud.txt  Dicts/BinDict/freq_hud.bin  Dicts/SrcBinDict/shira.txt > /dev/null 2> /dev/null  
for d in L C F; do 
  run Bin/StatDatBin  Russian  Dicts/SrcBinDict/StatData.rus -$d Dicts/Morph/Rus/;
  run Bin/StatDatBin  German   Dicts/SrcBinDict/StatData.ger -$d Dicts/Morph/Ger;
done;
run Bin/WordDatBin  Dicts/SrcBinDict/WordData.txt -L Dicts/Morph/Rus/ >/dev/null
run Bin/WordDatBin  Dicts/SrcBinDict/WordData.txt -F Dicts/Morph/Rus/ >/dev/null 
run Bin/WordDatBin  Dicts/SrcBinDict/WordData.txt -C Dicts/Morph/Rus/ >/dev/null  
run Bin/GenSynDict  Dicts/SrcBinDict/synonyms.txt Dicts/BinDict/synonyms.bin 2>/dev/null  
