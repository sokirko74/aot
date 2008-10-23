#include "rus_numerals.h"


const CNumeralToNumber NumeralToNumber[NumeralToNumberCount] = 
{
{"ндхм","оепбши","","","I",1,false}, 
{"онкрнпю","","","","",1.5,false}, 
{"дбю","брнпни","бдбнел","дбне","II",2,false}, 
{"рпх","рперхи","брпнел","рпне","III",3,false}, 
{"вершпе","вербепрши","бвербепнл","вербепн","IV",4,false}, 
{"оърэ","оърши","боърепнл","оърепн","V",5,false}, 
{"ьеярэ","ьеярни","бьеярепнл","ьеярепн","VI",6,false}, 
{"яелэ","яедэлни","бяелепнл","яелепн","VII",7,false}, 
{"бняелэ","бняэлни","ббняэлепнл","бняэлепн","VIII",8,false}, 
{"дебърэ","дебърши","бдебърепнл","дебърепн","IX",9 ,false}, 
{"деяърэ","деяърши","бдеяърепнл","деяърепн","X",10,false}, 
{"ндхммюджюрэ","ндхммюджюрши","бндхммюджюрепнл","","XI",11,false}, 
{"дбемюджюрэ","дбемюджюрши", "бдбемюджюрепнл","","XII",12,false}, 
{"рпхмюджюрэ","рпхмюджюрши", "брпхмюджюрепнл","","XIII",13,false}, 
{"вершпмюджюрэ","вершпмюджюрши","бвершпмюджюрепнл","","XIV",14,false}, 
{"оърмюджюрэ","оърмюджюрши", "боърмюджюрепнл","","XV",15,false}, 
{"ьеярмюджюрэ","ьеярмюджюрши", "бьеярмюджюрепнл","","XVI",16,false}, 
{"яелмюджюрэ","яелмюджюрши", "бяелмюджюрепнл","","XVII",17,false},  
{"бняелмюджюрэ","бняелмюджюрши", "ббняелмюджюрепнл","","XIII",18,false},  
{"дебърмюджюрэ","дебърмюджюрши", "бдебърмюджюрепнл","","XIX",19,false}, 
{"дбюджюрэ","дбюджюрши", "бдбюджюрепнл","","XX",20,false}, 
{"рпхджюрэ","рпхджюрши", "брпхджюрепнл","","XXX",30,false}, 
{"янпнй","янпнйнбни", "","","XL",40,false}, 
{"оърэдеяър","оърхдеяърши", "боърхдеяърепнл","","L",50,false},  
{"ьеярэдеяър","ьеярхдеяърши", "бьеярхдеяърепнл","","LX",60,false}, 
{"яелэдеяър","яелхдеяърши", "бяелхдеяърепнл","","LXX",70,false},  
{"бняелэдеяър","бняэлхдеяърши", "ббняэлхдеяърепнл","","LXXX",80,false}, 
{"дебъмнярн","дебъмнярши", "","","XC",90,false}, 
{"ярн","янрши","","","C",100,false}, 
{"дбеярх","дбсуянрши","","","CC",200,false}, 
{"рпхярю","рпеуянрши","","","CCC",300,false}, 
{"вершпеярю","вершпеуянрши","","CD","I",400,false}, 
{"оърэянр","оърхянрши", "","","D",500,false}, 
{"ьеярэянр","ьеярхянрши", "","","DC",600,false}, 
{"яелэянр","яелхянрши", "","","DCC",700,false}, 
{"бняелэянр","бняэлхянрши", "","","DCCC",800,false}, 
{"дебърэянр","дебърхянрши","","","CM",900,false}, 
{"ршяъвю","ршяъвмши","","","M",1000,true}, 
{"лхккхнм","лхккхнммши","","","",1000000,true},
{"лхккхюпд","лхккхюпдмши","","","",1000000000,true}
};



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

