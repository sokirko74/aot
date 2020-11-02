#include "rus_numerals.h"

std::string IntToStr (double i) 
{
	if (i == 1.5) return "1.5";
	char s[55];
	uint64_t  _i = i;
	#ifdef WIN32
		_i64toa(_i, s, 10);
	#else
			sprintf (s, "%ld", _i);
	#endif

	return s;
};

std::string FindByNumber(uint64_t Number)
{
	for(int i = 0 ; i < NumeralToNumberCount ; i++ )
		if( Number == NumeralToNumber[i].m_Number)
			return NumeralToNumber[i].m_Cardinal;
	return "";
};

std::string FindByNumber(uint64_t Number, NumberFormEnum formType)
{
	for(int i = 0 ; i < NumeralToNumberCount ; i++ )
		if (Number == NumeralToNumber[i].m_Number) {
			switch (formType) {
				case nfeCardinal: return NumeralToNumber[i].m_Cardinal;	
				case nfeOridnal: return NumeralToNumber[i].m_Ordinal;
				case nfeGenitiv: return NumeralToNumber[i].m_GenitForm;
			}
		}
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

// не используется пока
std::string spellout_number_ru(uint64_t x,  NumberFormEnum formType)
{	 
	if (x == 0) 
		return formType == nfeCardinal ? _R("НОЛЬ")  : _R("НУЛЕВОЙ");
	if (x < 20)
		return FindByNumber(x, formType);
	if (x < 100) {
		if (x % 10 == 0)
			return FindByNumber(x, formType);
		else
			return FindByNumber((x / 10)*10) + " " + FindByNumber(x % 10, formType);
	}
	if (x < 1000) {
		if (x % 100 == 0)
			return FindByNumber(x, formType);
		else
			return FindByNumber(x / 100 * 100) + " " + spellout_number_ru(x % 100, formType);
	}
	if (x < 1000000) {
		if (x % 1000 == 0)
			if (x % 10000 == 1000 && (x % 100000) / 10000 != 1) {
				std::string s1 = (x % 1000000 != 1000 ? spellout_number_ru((x / 10000) * 10, nfeGenitiv) + " " : "");
				std::string s2 = (formType == nfeCardinal ? _R("ОДНА ТЫСЯЧА") : _R("ОДНОТЫСЯЧНЫЙ"));
				return s1 + s2;
			}
			else
			if (formType == nfeCardinal)
				if ( x % 10000 != 0 && x % 10000 < 5000 && (x % 100000)/10000 != 1) 
					return (x % 10000 == 2000 ? (x % 1000000 != 2000 ? spellout_number_ru((x / 10000)*10, formType) + " " : "") + _R("ДВЕ") :
					spellout_number_ru(x / 1000, formType)) + _R(" ТЫСЯЧИ");
				else return spellout_number_ru(x / 1000, formType) + _R(" ТЫСЯЧ");
			else 
				return spellout_number_ru(x / 1000 , nfeGenitiv) + _R("ТЫСЯЧНЫЙ");
		else
			return spellout_number_ru((x / 1000) * 1000, nfeCardinal) + " " +
						spellout_number_ru(x % 1000, formType);
	}
	for(int p=2; p<6; p++)
	if (x < 1000*pow(1000,p)) {
		uint64_t Q = pow(1000,p);
		std::string m = FindByNumber(Q);
		if (x % Q == 0)
			if ( x % (10*Q) == Q ) return spellout_number_ru(x / Q , nfeGenitiv) +
				(formType == nfeCardinal ? " " + m : m + _R("НЫЙ"));
			else
			if (formType  == nfeCardinal)
				if ( x % (10*Q) != 0 && x % (10*Q) < 5*Q && (x % (100*Q))/(10*Q) != 1) 
					return  spellout_number_ru(x / Q, formType) + " " + m + _R("А"); //МИЛЛИОНА
				else return spellout_number_ru(x / Q, formType) + " " + m + _R("ОВ"); //МИЛЛИОНОВ
			else return spellout_number_ru(x / Q , nfeGenitiv) + m + _R("НЫЙ"); //МИЛЛИОНЫЙ
		else
			return spellout_number_ru((x / Q) * Q, nfeCardinal) + " " +
						spellout_number_ru(x % Q, formType);
	}
	#ifdef WIN32
		return Format("%I64i", x);
	#else
		return Format("%lli", x);
	#endif
}

