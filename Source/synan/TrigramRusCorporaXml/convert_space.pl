$bAttr =  0;
$nl =0;
while (<STDIN>)
{
	$nl++;
	$line = $_;
	if ($nl == 1)
	{
		print;
		next;
	}
	for ($i=0; $i < length ($line); $i++)
	{
		$ch = substr($line, $i, 1);
		if ($ch  eq "<")
		{
			$bAttr =  1;
		}
		elsif ($ch eq ">")
		{
			$bAttr =  0;
		}

		if ( ( $bAttr  == 0) &&  ($ch eq " "))
		{
			print "<trig_nbsp/>";
				
		}
		elsif ( ( $bAttr  == 0) &&  ($ch eq "\n"))
		{
			print "<trig_br/>\n";
		
		}
		elsif ( ($ch ne "\r") && ($ch ne "\t"))
		{
			print $ch;
		}
	}
}