
./StatDatBin $1 $3 -L ../Dicts/Morph/$2/
if [ "$?" -ne "0" ]; then
	echo "cannot compile homonyms statistics (fiction)";
	exit 1;
fi


./StatDatBin  $1 $3 -F ../Dicts/Morph/$2/
if [ "$?" -ne "0" ]; then
	echo "cannot compile homonyms statistics (finance)";
	exit 1;
fi


./StatDatBin  $1 $3 -C ../Dicts/Morph/$2/
if [ "$?" -ne "0" ]; then
	echo "cannot compile homonyms statistics (computer)";
	exit 1;
fi

exit 0;