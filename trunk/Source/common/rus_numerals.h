#ifndef __rus_numerals
#define __rus_numerals

#include  "utilit.h"

struct CNumeralToNumber
{
	char				m_Cardinal[25]; // "äâà"
	char				m_Ordinal[25]; // "âòîğîé"
	char				m_Adverb[25]; // "ÂÄÂÎÅÌ"
	char				m_CoollectiveNumber[25]; //  "ÄÂÎÅ"
	char				m_RomanNumber[25]; // II
	double				m_Number; // 2
    bool                m_bNoun; // false
};

const size_t NumeralToNumberCount = 40;
extern const CNumeralToNumber NumeralToNumber[];

extern string IntToStr (double i);
extern string FindByNumber(QWORD Number);
extern int IsAdverbRule(const string& Lemma);
extern double GetCardinalNumeral(const string& word);
extern QWORD GetOrdinalNumeral(const string& word);
extern QWORD GetNounNumeral(const string& word);


#endif


