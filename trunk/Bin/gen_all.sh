cd $RML/Bin;

MorphGen ../Dicts/SrcMorph/rus.mwz ../Dicts/Morph/Rus 5 3 
MorphGen ../Dicts/SrcMorph/eng.mwz ../Dicts/Morph/Eng 5 3
MorphGen ../Dicts/SrcMorph/ger.mwz ../Dicts/Morph/Ger 5 3

if [ ! -x $RML/Dicts/BinDict ]; then
	mkdir $RML/Dicts/BinDict;
	if [ "$?" -ne "0" ]; then
		echo "cannot create $RML/Dicts/BinDict";
		exit 1;
	fi

fi;



./AprDictGen ../Dicts/SrcBinDict/dict2809.txt ../Dicts/BinDict/dict.bin > zzout 
if [ "$?" -ne "0" ]; then
	echo "cannot compile binary dictionary";
	exit 1;
fi

./asp_read ../Dicts/SrcBinDict/ASP.txt ../Dicts/BinDict/asp_dict.bin > zzout 2>zzerr
if [ "$?" -ne "0" ]; then
	echo "cannot aspect dictionary";
	exit 1;
fi



./deriv_read ../Dicts/SrcBinDict/troiki_suff ../Dicts/BinDict/deriv_suff.bin > zzout 2> zzerr
if [ "$?" -ne "0" ]; then
	echo "cannot suffix dictionary";
	exit 1;
fi

./deriv_read ../Dicts/SrcBinDict/troiki_pref ../Dicts/BinDict/deriv_pref.bin > zzout 2> zzerr
if [ "$?" -ne "0" ]; then
	echo "cannot prefix dictionary";
	exit 1;
fi

#==============  frequences of binary translations ==================

./GenFreqDict freq_comp.txt ../Dicts/BinDict/freq_comp.bin ../Dicts/SrcBinDict/shira.txt > freq_comp.dmp 2> zzerr
if [ "$?" -ne "0" ]; then
	echo "cannot computer frequence  dictionary";
	exit 1;
fi

./GenFreqDict freq_fin.txt ../Dicts/BinDict/freq_fin.bin ../Dicts/SrcBinDict/shira.txt > freq_fin.dmp 2> zzerr
if [ "$?" -ne "0" ]; then
	echo "cannot finance frequence  dictionary";
	exit 1;
fi


./GenFreqDict freq_hud.txt ../Dicts/BinDict/freq_hud.bin ../Dicts/SrcBinDict/shira.txt > freq_hud.dmp 2> zzerr
if [ "$?" -ne "0" ]; then
	echo "cannot fiction frequence  dictionary";
	exit 1;
fi


#=============== homonym statistics  ======================

langua=Russian
dir=Rus
fil=../Dicts/SrcBinDict/StatData.rus

./StatDatBin $langua $fil -L ../Dicts/Morph/$dir/
if [ "$?" -ne "0" ]; then
	echo "cannot compile homonyms statistics (fiction)";
	exit 1;
fi


./StatDatBin  $langua $fil -F ../Dicts/Morph/$dir/
if [ "$?" -ne "0" ]; then
	echo "cannot compile homonyms statistics (finance)";
	exit 1;
fi


./StatDatBin  $langua $fil -C ../Dicts/Morph/$dir/
if [ "$?" -ne "0" ]; then
	echo "cannot compile homonyms statistics (computer)";
	exit 1;
fi


# =============   word statistics   =============
./WordDatBin  ../Dicts/SrcBinDict/WordData.txt -L ../Dicts/Morph/Rus/ >zzerr
if [ "$?" -ne "0" ]; then
	echo "cannot compile word statistics (fiction)";
	exit 1;
fi

./WordDatBin  ../Dicts/SrcBinDict/WordData.txt -F ../Dicts/Morph/Rus/ >zzerr
if [ "$?" -ne "0" ]; then
	echo "cannot compile word statistics (finance)";
	exit 1;
fi

./WordDatBin  ../Dicts/SrcBinDict/WordData.txt -C ../Dicts/Morph/Rus/ >zzerr
if [ "$?" -ne "0" ]; then
	echo "cannot compile word statistics (computer)";
	exit 1;
fi


# ===============   synonyms    =========================
./GenSynDict ../Dicts/SrcBinDict/synonyms.txt ../Dicts/BinDict/synonyms.bin 2>zzerr
if [ "$?" -ne "0" ]; then
	echo "cannot synonyms dictionary";
	exit 1;
fi

sh gen_stat.sh German Ger ../Dicts/SrcBinDict/StatData.ger

exit 0;


