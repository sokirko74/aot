#include "rus_numerals.h"


const CNumeralToNumber NumeralToNumber[NumeralToNumberCount+1] = 
{
{"ндхм","оепбши","","","I",1,false,"ндмн"}, 
{"онкрнпю","","","","",1.5,false,"онксрнпн"}, 
{"дбю","брнпни","бдбнел","дбне","II",2,false,"дбсу"}, 
{"рпх","рперхи","брпнел","рпне","III",3,false,"рпеу"}, 
{"вершпе","вербепрши","бвербепнл","вербепн","IV",4,false,"вершпеу"}, 
{"оърэ","оърши","боърепнл","оърепн","V",5,false,"оърх"}, 
{"ьеярэ","ьеярни","бьеярепнл","ьеярепн","VI",6,false,"ьеярх"}, 
{"яелэ","яедэлни","бяелепнл","яелепн","VII",7,false,"яелх"}, 
{"бняелэ","бняэлни","ббняэлепнл","бняэлепн","VIII",8,false,"бняелх"}, 
{"дебърэ","дебърши","бдебърепнл","дебърепн","IX",9 ,false,"дебърх"}, 
{"деяърэ","деяърши","бдеяърепнл","деяърепн","X",10,false,"деяърх"}, 
{"ндхммюджюрэ","ндхммюджюрши","бндхммюджюрепнл","","XI",11,false,"ндхммюджюрх"}, 
{"дбемюджюрэ","дбемюджюрши", "бдбемюджюрепнл","","XII",12,false,"дбемюджюрх"}, 
{"рпхмюджюрэ","рпхмюджюрши", "брпхмюджюрепнл","","XIII",13,false,"рпхмюджюрх"}, 
{"вершпмюджюрэ","вершпмюджюрши","бвершпмюджюрепнл","","XIV",14,false,"вершпмюджюрх"}, 
{"оърмюджюрэ","оърмюджюрши", "боърмюджюрепнл","","XV",15,false,"оърмюджюрх"}, 
{"ьеярмюджюрэ","ьеярмюджюрши", "бьеярмюджюрепнл","","XVI",16,false,"ьеярмюджюрх"}, 
{"яелмюджюрэ","яелмюджюрши", "бяелмюджюрепнл","","XVII",17,false,"яелмюджюрх"},  
{"бняелмюджюрэ","бняелмюджюрши", "ббняелмюджюрепнл","","XIII",18,false,"бняелмюджюрх"},  
{"дебърмюджюрэ","дебърмюджюрши", "бдебърмюджюрепнл","","XIX",19,false,"дебърмюджюрх"}, 
{"дбюджюрэ","дбюджюрши", "бдбюджюрепнл","","XX",20,false,"дбюджюрх"}, 
{"рпхджюрэ","рпхджюрши", "брпхджюрепнл","","XXX",30,false,"рпхджюрх"}, 
{"янпнй","янпнйнбни", "","","XL",40,false,"янпнйю"}, 
{"оърэдеяър","оърхдеяърши", "боърхдеяърепнл","","L",50,false,"оърхдеяърх"},  
{"ьеярэдеяър","ьеярхдеяърши", "бьеярхдеяърепнл","","LX",60,false,"ьеярхдеяърх"}, 
{"яелэдеяър","яелхдеяърши", "бяелхдеяърепнл","","LXX",70,false,"яелхдеяърх"},  
{"бняелэдеяър","бняэлхдеяърши", "ббняэлхдеяърепнл","","LXXX",80,false,"бняэлхдеяърх"}, 
{"дебъмнярн","дебъмнярши", "","","XC",90,false,"дебъмнярн"}, 
{"ярн","янрши","","","C",100,false,"ярн"}, 
{"дбеярх","дбсуянрши","","","CC",200,false,"дбсуянр"}, 
{"рпхярю","рпеуянрши","","","CCC",300,false,"рпеуянр"}, 
{"вершпеярю","вершпеуянрши","","CD","I",400,false,"вершпеуянр"}, 
{"оърэянр","оърхянрши", "","","D",500,false,"оърхянр"}, 
{"ьеярэянр","ьеярхянрши", "","","DC",600,false,"ьеярхянр"}, 
{"яелэянр","яелхянрши", "","","DCC",700,false,"яелхянр"}, 
{"бняелэянр","бняэлхянрши", "","","DCCC",800,false,"бняэлхянр"}, 
{"дебърэянр","дебърхянрши","","","CM",900,false,"дебърхянр"}, 
{"ршяъвю","ршяъвмши","","","M",1000,true,""}, 
{"лхккхнм","лхккхнммши","","","",1000000,true,""},
{"лхккхюпд","лхккхюпдмши","","","",1000000000,true,""},
{"рпхккхнм","рпхккхнммши","","","",1000000000000.0,true,""},
{"йбюдпхккхнм","йбюдпхккхнммши","","","",1000000000000000.0,true,""},
{"мнкэ","мскебни","","","",0,true,""} //МЕ ДНКФЕМ АШРЭ БЙКЧВЕМ Б NumeralToNumberCount
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
	if (x == 0) return IsOrdinal ? "мскебни" : "мнкэ";
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
				+ (IsOrdinal==0 ? "ндмю ршяъвю" : "ндмнршяъвмши");
			else
			if (IsOrdinal==0)
				if ( x % 10000 != 0 && x % 10000 < 5000 && (x % 100000)/10000 != 1) 
					return (x % 10000 == 2000 ? (x % 1000000 != 2000 ? spellout_number_ru((x / 10000)*10, IsOrdinal) + " " : "") + "дбе" : 
					spellout_number_ru(x / 1000, IsOrdinal)) + " ршяъвх";
				else return spellout_number_ru(x / 1000, IsOrdinal) + " ршяъв";
			else return spellout_number_ru(x / 1000 , IsOrdinal*10) + "ршяъвмши";
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
				(IsOrdinal==0 ? " " + m : m + "мши");
			else
			if (IsOrdinal==0)
				if ( x % (10*Q) != 0 && x % (10*Q) < 5*Q && (x % (100*Q))/(10*Q) != 1) 
					return  spellout_number_ru(x / Q, IsOrdinal) + " " + m + "ю"; //лхккхнмю
				else return spellout_number_ru(x / Q, IsOrdinal) + " " + m + "нб"; //лхккхнмнб
			else return spellout_number_ru(x / Q , IsOrdinal*10) + m + "мши"; //лхккхнмши
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

