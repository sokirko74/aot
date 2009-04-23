#include "rus_numerals.h"


const CNumeralToNumber NumeralToNumber[NumeralToNumberCount] = 
{
{"����","������","","","I",1,false}, 
{"�������","","","","",1.5,false}, 
{"���","������","������","����","II",2,false}, 
{"���","������","������","����","III",3,false}, 
{"������","���������","���������","�������","IV",4,false}, 
{"����","�����","��������","������","V",5,false}, 
{"�����","������","���������","�������","VI",6,false}, 
{"����","�������","��������","������","VII",7,false}, 
{"������","�������","����������","��������","VIII",8,false}, 
{"������","�������","����������","��������","IX",9 ,false}, 
{"������","�������","����������","��������","X",10,false}, 
{"�����������","������������","���������������","","XI",11,false}, 
{"����������","�����������", "��������������","","XII",12,false}, 
{"����������","�����������", "��������������","","XIII",13,false}, 
{"������������","�������������","����������������","","XIV",14,false}, 
{"����������","�����������", "��������������","","XV",15,false}, 
{"�����������","������������", "���������������","","XVI",16,false}, 
{"����������","�����������", "��������������","","XVII",17,false},  
{"������������","�������������", "����������������","","XIII",18,false},  
{"������������","�������������", "����������������","","XIX",19,false}, 
{"��������","���������", "������������","","XX",20,false}, 
{"��������","���������", "������������","","XXX",30,false}, 
{"�����","���������", "","","XL",40,false}, 
{"���������","�����������", "��������������","","L",50,false},  
{"����������","������������", "���������������","","LX",60,false}, 
{"���������","�����������", "��������������","","LXX",70,false},  
{"�����������","�������������", "����������������","","LXXX",80,false}, 
{"���������","����������", "","","XC",90,false}, 
{"���","�����","","","C",100,false}, 
{"������","���������","","","CC",200,false}, 
{"������","���������","","","CCC",300,false}, 
{"���������","������������","","CD","I",400,false}, 
{"�������","���������", "","","D",500,false}, 
{"��������","����������", "","","DC",600,false}, 
{"�������","���������", "","","DCC",700,false}, 
{"���������","�����������", "","","DCCC",800,false}, 
{"���������","�����������","","","CM",900,false}, 
{"������","��������","","","M",1000,true}, 
{"�������","����������","","","",1000000,true},
{"��������","�����������","","","",1000000000,true}
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

