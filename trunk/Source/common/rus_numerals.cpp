#include "rus_numerals.h"

string IntToStr (double i) 
{
	if (i == 1.5) return "1.5";
	char s[55];
	QWORD  _i = i;
	#ifdef WIN32
		_i64toa(_i, s, 10);
	#else
			sprintf (s, "%Ld", _i);
	#endif

	return s;
};

string FindByNumber(QWORD Number)
{
	for(int i = 0 ; i < NumeralToNumberCount ; i++ )
		if( Number == NumeralToNumber[i].m_Number)
			return NumeralToNumber[i].m_Cardinal;
	return "";
};

string FindByNumber(QWORD Number, BYTE IsOrdinal) // 0 - no , 1 - yes, 10 - yes, for thousands
{
	for(int i = 0 ; i < NumeralToNumberCount ; i++ )
		if( Number == NumeralToNumber[i].m_Number)
			return IsOrdinal == 0 ? NumeralToNumber[i].m_Cardinal :
				   IsOrdinal == 1 ? NumeralToNumber[i].m_Ordinal : NumeralToNumber[i].m_GenitForm;
	return "";
};

int IsAdverbRule(const string& Lemma)
{
	
	for(int i = 0 ; i < NumeralToNumberCount ; i++ )
		if (!strcmp (Lemma.c_str(), NumeralToNumber[i].m_Adverb))
			return NumeralToNumber[i].m_Number;

   return -1;
};


double GetCardinalNumeral(const string& word)
{
	for(int i = 0 ; i < NumeralToNumberCount ; i++ )
		if( word == NumeralToNumber[i].m_Cardinal )
			return NumeralToNumber[i].m_Number;
	return -1;
};

QWORD GetOrdinalNumeral(const string& word)
{
	for(int i = 0 ; i < NumeralToNumberCount; i++ )
		if( word == NumeralToNumber[i].m_Ordinal )
			return NumeralToNumber[i].m_Number;
	return -1;
};

QWORD GetNounNumeral(const string& word)
{
	for(int i = 0 ; i < NumeralToNumberCount; i++ )
        if( word == NumeralToNumber[i].m_CoollectiveNumber )
			return NumeralToNumber[i].m_Number;
	return -1;
};

string spellout_number_ru(QWORD x, BYTE IsOrdinal) // 0 - no , 1 - yes, 10 - yes, for thousands
{	 
	if (x == 0) return IsOrdinal ? "ÍÓËÅÂÎÉ" : "ÍÎËÜ";
	if (x < 20)
		return FindByNumber(x, IsOrdinal);
	if (x < 100) {
		if (x % 10 == 0)
			return FindByNumber(x, IsOrdinal);
		else
			return FindByNumber((x / 10)*10) + " " + FindByNumber(x % 10, IsOrdinal);
	}
	if (x < 1000) {
		if (x % 100 == 0)
			return FindByNumber(x, IsOrdinal);
		else
			return FindByNumber(x / 100 * 100) + " " + spellout_number_ru(x % 100, IsOrdinal);
	}
	if (x < 1000000) {
		if (x % 1000 == 0)
			if ( x % 10000 == 1000 && (x % 100000)/10000 != 1)
				return (x % 1000000 != 1000 ? spellout_number_ru((x / 10000)*10, IsOrdinal*10) + " " : "") +
				+ (IsOrdinal==0 ? "ÎÄÍÀ ÒÛÑß×À" : "ÎÄÍÎÒÛÑß×ÍÛÉ");
			else
			if (IsOrdinal==0)
				if ( x % 10000 != 0 && x % 10000 < 5000 && (x % 100000)/10000 != 1) 
					return (x % 10000 == 2000 ? (x % 1000000 != 2000 ? spellout_number_ru((x / 10000)*10, IsOrdinal) + " " : "") + "ÄÂÅ" : 
					spellout_number_ru(x / 1000, IsOrdinal)) + " ÒÛÑß×È";
				else return spellout_number_ru(x / 1000, IsOrdinal) + " ÒÛÑß×";
			else return spellout_number_ru(x / 1000 , IsOrdinal*10) + "ÒÛÑß×ÍÛÉ";
		else
			return spellout_number_ru((x / 1000) * 1000, 0) + " " +
						spellout_number_ru(x % 1000, IsOrdinal);
	}
	for(int p=2; p<6; p++)
	if (x < 1000*pow(1000,p)) {
		QWORD Q = pow(1000,p);
		string m = FindByNumber(Q);
		if (x % Q == 0)
			if ( x % (10*Q) == Q ) return spellout_number_ru(x / Q , IsOrdinal*10) + 
				(IsOrdinal==0 ? " " + m : m + "ÍÛÉ");
			else
			if (IsOrdinal==0)
				if ( x % (10*Q) != 0 && x % (10*Q) < 5*Q && (x % (100*Q))/(10*Q) != 1) 
					return  spellout_number_ru(x / Q, IsOrdinal) + " " + m + "À"; //ÌÈËËÈÎÍÀ
				else return spellout_number_ru(x / Q, IsOrdinal) + " " + m + "ÎÂ"; //ÌÈËËÈÎÍÎÂ
			else return spellout_number_ru(x / Q , IsOrdinal*10) + m + "ÍÛÉ"; //ÌÈËËÈÎÍÛÉ
		else
			return spellout_number_ru((x / Q) * Q, 0) + " " +
						spellout_number_ru(x % Q, IsOrdinal);
	}
	#ifdef WIN32
		return Format("%I64i", x);
	#else
		return Format("%lli", x);
	#endif
}

