rm $RML/Bin/result.gra
for f in $RML/Bin/GraTest/*.txt; do 
	./GraphmatThick Russian $f -gra  test.gra
	tr  "\n\r" " " <$f | tr "\000" " " 	>>$RML/Bin/result.gra
	echo "" 			>>$RML/Bin/result.gra
	sed -r "s/(.*)/	\1/" <test.gra 	>>$RML/Bin/result.gra
done;
rm test.gra