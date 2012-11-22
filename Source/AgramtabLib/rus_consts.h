// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko, Dmitry Pankratov

#ifndef __SYNANLib_LIBRARY_DEFINED__
#ifndef __AGRAMTABLib_LIBRARY_DEFINED__

#ifndef rus_consts_h
 #define rus_consts_h



enum rPartOfSpeeches 
{
	  NOUN  = 0, 
	  ADJ_FULL = 1, 
	  VERB = 2, 
	  PRONOUN = 3, 
	  PRONOUN_P = 4, 
	  PRONOUN_PREDK = 5,
	  NUMERAL  = 6, 
	  NUMERAL_P = 7, 
	  ADV = 8, 
	  PREDK  = 9, 
	  PREP = 10,
	  POSL = 11,
	  CONJ = 12,
	  INTERJ = 13,
	  INP = 14,
	  PHRASE = 15,
	  PARTICLE = 16,
	  ADJ_SHORT = 17,
      PARTICIPLE = 18,
	  ADVERB_PARTICIPLE = 19,
	  PARTICIPLE_SHORT = 20,
	  INFINITIVE = 21,
};

 

enum rGrammems {
		 rPlural     = 0,
		 rSingular   = 1,
		 rAllNumbers = ((1<<rSingular) | (1<<rPlural)),

		 rNominativ  = 2,
		 rGenitiv    = 3,
		 rDativ      = 4,
		 rAccusativ  = 5,
		 rInstrumentalis = 6,
		 rLocativ    = 7,
		 rVocativ    = 8,
		 rAllCases   = ((1<<rNominativ) | (1<<rGenitiv) | (1<<rDativ) | (1<<rAccusativ) | (1<<rInstrumentalis) | (1<<rVocativ) | (1<<rLocativ)),

		 rMasculinum = 9,
		 rFeminum    = 10,
		 rNeutrum    = 11,
		 rMascFem    = 12,
		 rAllGenders   = ((1<<rMasculinum) | (1<<rFeminum) | (1<<rNeutrum)),


		 rPresentTense = 13,
		 rFutureTense = 14,
		 rPastTense = 15,
		 rAllTimes   = ((1<<rPresentTense) | (1<<rFutureTense) | (1<<rPastTense)),

		 rFirstPerson = 16,
		 rSecondPerson = 17,
		 rThirdPerson = 18,
		 rAllPersons = ((1 << rFirstPerson) | (1 << rSecondPerson) | (1 << rThirdPerson)),

		 rImperative = 19,

		 rAnimative = 20,
		 rNonAnimative = 21,
		 rAllAnimative   = ((1<<rAnimative) | (1<<rNonAnimative)),

		 rComparative = 22,

		 rPerfective = 23,//ñâ
		 rNonPerfective = 24,//íñ

		 rNonTransitive = 25,//íï
		 rTransitive = 26,//ïå

		 rActiveVoice = 27,//äñò
		 rPassiveVoice = 28,//ñòð


		 rIndeclinable = 29, //"0"
	     rInitialism = 30,	//"àááð"

		 rPatronymic = 31, // "îò÷"

		 rToponym = 32,//ëîê
		 rOrganisation = 33,//îðã

		 rQualitative = 34,//êà÷
		 rDeFactoSingTantum = 35,//äôñò

		 rInterrogative = 36,//âîïð
		 rDemonstrative = 37,//óêàçàò

		 rName	    = 38,
		 rSurName	= 39,
		 rImpersonal = 40,//áåçë
		 rSlang	= 41,//æàðã
		 rMisprint = 42,//îï÷
		 rColloquial = 43,//ðàçã
		 rPossessive = 44,//ïðèòÿæ
		 rArchaism = 45,//àðõ
		 rSecondCase = 46,
		 rPoetry = 47,
		 rProfession = 48,
		 rSuperlative = 49,//ïðåâ
		 rPositive = 50//ïîëîæ
    };




typedef enum {
	VERB_PERS_T=0,//ÃË_ËÈ×Í
	ADVERB_PARTICIPLE_T=1, //ÄÏÐ
	PARTICIPLE_SHORT_T=2,	//ÊÐ_ÏÐ×
	ADJ_SHORT_T=3,//ÊÐ_ÏÐÈË
	PREDK_T=4, //ÏÐÅÄÊ
	PARTICIPLE_T=5,	//ÏÐ×
	INFINITIVE_T=6,	//ÈÍÔ
	INP_T=7, //ÂÂÎÄ
	DASH_T=8,//ÒÈÐÅ
	UNDETACHED_ADJ_PATIC=9,//ÍÑÎ
	COMPARATIVE_T=10,//ÑÐÀÂÍ
	COPUL_T=11//ÊÎÏÓË
} rClauseTypeEnum ;


#endif


#endif
#endif
