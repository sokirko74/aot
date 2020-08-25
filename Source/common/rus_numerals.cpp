#include "rus_numerals.h"

std::string IntToStr (double i) 
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

std::string FindByNumber(QWORD Number)
{
	for(int i = 0 ; i < NumeralToNumberCount ; i++ )
		if( Number == NumeralToNumber[i].m_Number)
			return NumeralToNumber[i].m_Cardinal;
	return "";
};

std::string FindByNumber(QWORD Number, BYTE IsOrdinal) // 0 - no , 1 - yes, 10 - yes, for thousands
{
	for(int i = 0 ; i < NumeralToNumberCount ; i++ )
		if( Number == NumeralToNumber[i].m_Number)
			return IsOrdinal == 0 ? NumeralToNumber[i].m_Cardinal :
				   IsOrdinal == 1 ? NumeralToNumber[i].m_Ordinal : NumeralToNumber[i].m_GenitForm;
	return "";
};

int IsAdverbRule(const std::string& Lemma)
{
	for(int i = 0 ; i < NumeralToNumberCount ; i++ )
		if (Lemma == NumeralToNumber[i].m_Adverb)
			return NumeralToNumber[i].m_Number;

   return -1;
};


double GetCardinalNumeral(const std::string& word)
{
	for(int i = 0 ; i < NumeralToNumberCount ; i++ )
		if( word == NumeralToNumber[i].m_Cardinal )
			return NumeralToNumber[i].m_Number;
	return -1;
};

double GetOrdinalNumeral(const std::string& word)
{
	for(size_t i = 0 ; i < NumeralToNumberCount; i++ )
		if (word == NumeralToNumber[i].m_Ordinal) {
			return  NumeralToNumber[i].m_Number;
		}
	return -1;
};

double GetNounNumeral(const std::string& word)
{
	for(int i = 0 ; i < NumeralToNumberCount; i++ )
        if( word == NumeralToNumber[i].m_CoollectiveNumber )
			return NumeralToNumber[i].m_Number;
	return -1;
};

std::string spellout_number_ru(QWORD x, BYTE IsOrdinal) // 0 - no , 1 - yes, 10 - yes, for thousands
{	 
	if (x == 0) return IsOrdinal ? _R("НУЛЕВОЙ") : _R("НОЛЬ");
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
			if (x % 10000 == 1000 && (x % 100000) / 10000 != 1) {
				std::string s1 = (x % 1000000 != 1000 ? spellout_number_ru((x / 10000) * 10, IsOrdinal * 10) + " " : "");
				std::string s2 = (IsOrdinal == 0 ? _R("ОДНА ТЫСЯЧА") : _R("ОДНОТЫСЯЧНЫЙ"));
				return s1 + s2;
			}
			else
			if (IsOrdinal==0)
				if ( x % 10000 != 0 && x % 10000 < 5000 && (x % 100000)/10000 != 1) 
					return (x % 10000 == 2000 ? (x % 1000000 != 2000 ? spellout_number_ru((x / 10000)*10, IsOrdinal) + " " : "") + _R("ДВЕ") : 
					spellout_number_ru(x / 1000, IsOrdinal)) + _R(" ТЫСЯЧИ");
				else return spellout_number_ru(x / 1000, IsOrdinal) + _R(" ТЫСЯЧ");
			else return spellout_number_ru(x / 1000 , IsOrdinal*10) + _R("ТЫСЯЧНЫЙ");
		else
			return spellout_number_ru((x / 1000) * 1000, 0) + " " +
						spellout_number_ru(x % 1000, IsOrdinal);
	}
	for(int p=2; p<6; p++)
	if (x < 1000*pow(1000,p)) {
		QWORD Q = pow(1000,p);
		std::string m = FindByNumber(Q);
		if (x % Q == 0)
			if ( x % (10*Q) == Q ) return spellout_number_ru(x / Q , IsOrdinal*10) + 
				(IsOrdinal==0 ? " " + m : m + _R("НЫЙ"));
			else
			if (IsOrdinal==0)
				if ( x % (10*Q) != 0 && x % (10*Q) < 5*Q && (x % (100*Q))/(10*Q) != 1) 
					return  spellout_number_ru(x / Q, IsOrdinal) + " " + m + _R("А"); //МИЛЛИОНА
				else return spellout_number_ru(x / Q, IsOrdinal) + " " + m + _R("ОВ"); //МИЛЛИОНОВ
			else return spellout_number_ru(x / Q , IsOrdinal*10) + m + _R("НЫЙ"); //МИЛЛИОНЫЙ
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

