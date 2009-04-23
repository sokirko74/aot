
// =====================================================
// Синтез английских числительных по числу. Файл создан Алексеем Сокирко 10  ноября 2000 года.
// Документация - в "l:\documents\Перевод арабских цифр в английские числительные.lnk"

#if !defined(eng_numerals)
	#define eng_numerals

		
inline string spellout_ordinal_number(double y) 
{
	const char ones[][10] = { "zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine" };
	const char teens[][10] ={ "ten", "eleven", "twelve", "thirteen", "fourteen", "fifteen","sixteen", "seventeen", "eighteen", "nineteen" };
	const char tens[][10] ={ "ten", "twenty", "thirty", "forty", "fifty", "sixty", "seventy","eighty", "ninety" };
	const char thousands[][10] = { " thousand", " million", " billion", " trillion" };
	const char hundred[] = " hundred";
	QWORD x = (QWORD)y;
     

	if (x < 10)
		return ones[x];
	if (x < 20)
		return teens[x - 10];
	if (x < 100) {
		if (x % 10 == 0)
			return tens[(x / 10)  - 1];
		else
			return string(tens[((x / 10)) - 1]) + string("-") + string(ones[x % 10]);
	}
	if (x < 1000) {
		if (x % 100 == 0)
			return string(ones[x / 100]) + string(hundred);
		else
			return string(ones[x / 100]) + string(hundred) + string(" ") +
						spellout_ordinal_number(x % 100);
	}
	if (x < 1000000) {
		if (x % 1000 == 0)
			return spellout_ordinal_number(x / 1000) + string(thousands[0]);
		else
			return spellout_ordinal_number(x / 1000) + thousands[0] + " " +
						spellout_ordinal_number(x % 1000);
	}
	if (x < 1000000000) {
		if (x % 1000000 == 0)
			return spellout_ordinal_number(x / 1000000) + thousands[1];
		else
			return spellout_ordinal_number(x / 1000000) + thousands[1] + " " +
						spellout_ordinal_number(x % 1000000);
	}
	#ifdef WIN32
		if (x < 1000000000000) {
	#else
		if (x < 1000000000000ULL) {
	#endif
		if (x % 1000000000 == 0)
			return spellout_ordinal_number(x / 1000000000) + thousands[2];
		else
			return spellout_ordinal_number(x / 1000000000) + thousands[2] + " " +
						spellout_ordinal_number(x % 1000000000);
	}

	
	#ifdef WIN32
		return Format("%I64i", x);
	#else
		return Format("%lli", x);
	#endif
}



inline string spellout_number(string NumberStr, bool IsCardinal) 
{
	if (NumberStr.length() ==0 ) return "";

	int pos = NumberStr.find ("(");
	if (    (pos != -1) 
	 	 && (NumberStr[NumberStr.length() - 1] == ')')
	   )
	{
		string Result = spellout_number(NumberStr.substr(0, pos), IsCardinal);
		Result += " or ";
		Result += spellout_number(NumberStr.substr(pos+1, NumberStr.length() - 2 - pos), IsCardinal);
		return Result;
	};

	

	double x = atof (NumberStr.c_str());
	if (x == 1.5)
		return "one and a half";

	const char ordinal_ones[][10] = { "first", "second", "third" };
	if  ((x > 0) && (x < 4) && !IsCardinal)  
		return  ordinal_ones [int(x)-1];


	string  result =  spellout_ordinal_number (x);

	if ( (result.length() > 0) && !IsCardinal)
		if ( result[result.length() - 1] == 'y' )
		{
			result.erase (result.length() - 1);
			result += "ieth";
		}
		else
		{
			result += "th";
		};
    return result;
};		


#endif
