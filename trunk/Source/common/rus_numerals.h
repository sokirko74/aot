#ifndef __rus_numerals
#define __rus_numerals

#include  "utilit.h"

struct CNumeralToNumber
{
	char				m_Cardinal[25]; // "два"
	char				m_Ordinal[25]; // "второй"
	char				m_Adverb[25]; // "ВДВОЕМ"
	char				m_CoollectiveNumber[25]; //  "ДВОЕ"
	char				m_RomanNumber[25]; // II
	double				m_Number; // 2
    bool                m_bNoun; // false
	char				m_GenitForm[25]; // "двух" (двухламповый)
};

const size_t NumeralToNumberCount = 42;
extern const CNumeralToNumber NumeralToNumber[];

extern string IntToStr (double i);
extern string FindByNumber(QWORD Number);
extern int IsAdverbRule(const string& Lemma);
extern double GetCardinalNumeral(const string& word);
extern QWORD GetOrdinalNumeral(const string& word);
extern QWORD GetNounNumeral(const string& word);
extern string spellout_number_ru(string NumberStr, bool IsCardinal);
extern string spellout_ordinal_number_ru(QWORD x, BYTE IsOrdinal); // 0 - no , 1 - yes, 10 - yes, for thousands

#endif


