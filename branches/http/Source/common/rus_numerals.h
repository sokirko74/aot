#ifndef __rus_numerals
#define __rus_numerals

#include  "utilit.h"

struct CNumeralToNumber
{
	char				m_Cardinal[25]; // "���"
	char				m_Ordinal[25]; // "������"
	char				m_Adverb[25]; // "������"
	char				m_CoollectiveNumber[25]; //  "����"
	char				m_RomanNumber[25]; // II
	double				m_Number; // 2
    bool                m_bNoun; // false
	char				m_GenitForm[25]; // "����" (������������)
};

const size_t NumeralToNumberCount = 42;
const CNumeralToNumber NumeralToNumber[NumeralToNumberCount+1] = 
{
{"����","������","","","I",1,false,"����"}, 
{"�������","","","","",1.5,false,"��������"}, 
{"���","������","������","����","II",2,false,"����"}, 
{"���","������","������","����","III",3,false,"����"}, 
{"������","���������","���������","�������","IV",4,false,"�������"}, 
{"����","�����","��������","������","V",5,false,"����"}, 
{"�����","������","���������","�������","VI",6,false,"�����"}, 
{"����","�������","��������","������","VII",7,false,"����"}, 
{"������","�������","����������","��������","VIII",8,false,"������"}, 
{"������","�������","����������","��������","IX",9 ,false,"������"}, 
{"������","�������","����������","��������","X",10,false,"������"}, 
{"�����������","������������","���������������","","XI",11,false,"�����������"}, 
{"����������","�����������", "��������������","","XII",12,false,"����������"}, 
{"����������","�����������", "��������������","","XIII",13,false,"����������"}, 
{"������������","�������������","����������������","","XIV",14,false,"������������"}, 
{"����������","�����������", "��������������","","XV",15,false,"����������"}, 
{"�����������","������������", "���������������","","XVI",16,false,"�����������"}, 
{"����������","�����������", "��������������","","XVII",17,false,"����������"},  
{"������������","�������������", "����������������","","XIII",18,false,"������������"},  
{"������������","�������������", "����������������","","XIX",19,false,"������������"}, 
{"��������","���������", "������������","","XX",20,false,"��������"}, 
{"��������","���������", "������������","","XXX",30,false,"��������"}, 
{"�����","���������", "","","XL",40,false,"������"}, 
{"���������","�����������", "��������������","","L",50,false,"����������"},  
{"����������","������������", "���������������","","LX",60,false,"�����������"}, 
{"���������","�����������", "��������������","","LXX",70,false,"����������"},  
{"�����������","�������������", "����������������","","LXXX",80,false,"������������"}, 
{"���������","����������", "","","XC",90,false,"���������"}, 
{"���","�����","","","C",100,false,"���"}, 
{"������","���������","","","CC",200,false,"�������"}, 
{"������","���������","","","CCC",300,false,"�������"}, 
{"���������","������������","","CD","I",400,false,"����������"}, 
{"�������","���������", "","","D",500,false,"�������"}, 
{"��������","����������", "","","DC",600,false,"��������"}, 
{"�������","���������", "","","DCC",700,false,"�������"}, 
{"���������","�����������", "","","DCCC",800,false,"���������"}, 
{"���������","�����������","","","CM",900,false,"���������"}, 
{"������","��������","","","M",1000,true,""}, 
{"�������","����������","","","",1000000,true,""},
{"��������","�����������","","","",1000000000,true,""},
{"��������","�����������","","","",1000000000000.0,true,""},
{"�����������","��������������","","","",1000000000000000.0,true,""},
{"����","�������","","","",0,true,""} //�� ������ ���� ������� � NumeralToNumberCount
};


extern const CNumeralToNumber NumeralToNumber[];

extern string IntToStr (double i);
extern string FindByNumber(QWORD Number);
extern int IsAdverbRule(const string& Lemma);
extern double GetCardinalNumeral(const string& word);
extern QWORD GetOrdinalNumeral(const string& word);
extern QWORD GetNounNumeral(const string& word);
extern string spellout_number_ru(QWORD x, BYTE IsOrdinal); // 0 - no , 1 - yes, 10 - yes, for thousands

#endif

