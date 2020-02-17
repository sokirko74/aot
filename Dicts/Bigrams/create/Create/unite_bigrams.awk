BEGIN {
 FS = "\t";
 Prev1 = "";
 Prev2 = "";
 Count = 0;
}
{
	if ( ($1 == "") || ($2 == "") || ($3 == ""))
	{
		print "bad format at line ", $0 >> "/dev/stderr"
		next;
	};

	if (Prev1 != "")
		if ( (("word" Prev1) != ("word" $1))  || (("word" Prev2) != ("word" $2)) )
		{
			printf ("%s\t%s\t%i\n",Prev1, Prev2, Count);
			Count = 0;
			
		}
	Prev1 = $1;
	Prev2 = $2;
	Count += $3;
}
END {
	printf ("%s\t%s\t%i\n",Prev1, Prev2, Count);
}