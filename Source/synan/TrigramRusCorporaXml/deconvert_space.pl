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
	$new_line = "";
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
			
				
		}
		elsif ( ( $bAttr  == 0) &&  ($ch eq "\n"))
		{
			
		
		}
		elsif ( ($ch ne "\r") && ($ch ne "\t"))
		{
			$new_line .= $ch;
		}
	}
	$new_line =~ s/<trig_nbsp *\/>/ /g;
	$new_line =~ s/<trig_br *\/>/\n/g;
	$new_line =~ s/&quot;/"/g;
	print $new_line;

}