// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko, Dmitry Pankratov

#pragma once

// this file is included in a WINDOWS windows IDL,that's why we have to reuse generated constants
#ifndef __AGRAMTABLib_LIBRARY_DEFINED__
#ifndef __SYNANLib_LIBRARY_DEFINED__

enum RussianPartOfSpeechEnum
{
	NOUN = 0,
	ADJ_FULL = 1,
	VERB = 2,
	PRONOUN = 3,
	PRONOUN_P = 4,
	PRONOUN_PREDK = 5,
	NUMERAL = 6,
	NUMERAL_P = 7,
	ADV = 8,
	PREDK = 9,
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
	RUSSIAN_PART_OF_SPEECH_COUNT = 22
};



enum RussianGrammemsEnum {
	rPlural = 0,
	rSingular = 1,
	rAllNumbers = ((1 << rSingular) | (1 << rPlural)),

	rNominativ = 2,
	rGenitiv = 3,
	rDativ = 4,
	rAccusativ = 5,
	rInstrumentalis = 6,//тв,
	rLocativ = 7,	//пр,
	rVocativ = 8,	//зв,
	rAllCases = ((1 << rNominativ) | (1 << rGenitiv) | (1 << rDativ) | (1 << rAccusativ) | (1 << rInstrumentalis) | (1 << rVocativ) | (1 << rLocativ)),

	rMasculinum = 9,
	rFeminum = 10,
	rNeutrum = 11,
	rMascFem = 12,
	rAllGenders = ((1 << rMasculinum) | (1 << rFeminum) | (1 << rNeutrum)),


	rPresentTense = 13,
	rFutureTense = 14,
	rPastTense = 15,
	rAllTimes = ((1 << rPresentTense) | (1 << rFutureTense) | (1 << rPastTense)),

	rFirstPerson = 16,
	rSecondPerson = 17,
	rThirdPerson = 18,
	rAllPersons = ((1 << rFirstPerson) | (1 << rSecondPerson) | (1 << rThirdPerson)),

	rImperative = 19,

	rAnimative = 20,
	rNonAnimative = 21,
	rAllAnimative = ((1 << rAnimative) | (1 << rNonAnimative)),

	rComparative = 22,

	rPerfective = 23,//св
	rNonPerfective = 24,//нс

	rNonTransitive = 25,//нп
	rTransitive = 26,//пе

	rActiveVoice = 27,//дст
	rPassiveVoice = 28,//стр


	rIndeclinable = 29, //"0"
	rInitialism = 30,	//"аббр"

	rPatronymic = 31, // "отч"

	rToponym = 32,//лок
	rOrganisation = 33,//орг

	rQualitative = 34,//кач
	rDeFactoSingTantum = 35,//дфст

	rInterrogative = 36,//вопр
	rDemonstrative = 37,//указат

	rName = 38,
	rSurName = 39,
	rImpersonal = 40,//безл
	rSlang = 41,//жарг
	rMisprint = 42,//опч
	rColloquial = 43,//разг
	rPossessive = 44,//притяж
	rArchaism = 45,//арх
	rSecondCase = 46,
	rPoetry = 47,
	rProfession = 48,
	rSuperlative = 49,
	rPositive = 50,
	RussianGrammemsCount = 51
};




enum RussianClauseTypeEnum  {
	VERB_PERS_T = 0,//ГЛ_ЛИЧН
	ADVERB_PARTICIPLE_T = 1, //ДПР
	PARTICIPLE_SHORT_T = 2,	//КР_ПРЧ
	ADJ_SHORT_T = 3,//КР_ПРИЛ
	PREDK_T = 4, //ПРЕДК
	PARTICIPLE_T = 5,	//ПРЧ
	INFINITIVE_T = 6,	//ИНФ
	INP_T = 7, //ВВОД
	DASH_T = 8,//ТИРЕ
	UNDETACHED_ADJ_PATIC = 9,//НСО
	COMPARATIVE_T = 10,//СРАВН
	COPUL_T = 11,
	RUSSIAN_CLAUSE_TYPE_COUNT = 12
};

#endif	
#endif