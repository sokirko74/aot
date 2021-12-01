// #pragma once  is not enough since this file is included from agramtab.idl

#pragma once 

// this file is included in a WINDOWS windows IDL,that's why we have to also reuse generated ifdef
#ifndef __AGRAMTABLib_LIBRARY_DEFINED__
#ifndef __SYNANLib_LIBRARY_DEFINED__

enum GermanPartOfSpeechEnum
{
	gART = 0,
	gADJ = 1,
	gADV = 2,
	gEIG = 3,
	gSUB = 4,
	gVER = 5,
	gPA1 = 6,
	gPA2 = 7,
	gPRONOMEN = 8,
	gPRP = 9,
	gKON = 10,
	gNEG = 11,
	gINJ = 12,
	gZAL = 13,
	gZUS = 14,
	gPRO_BEG = 15,
	gZU_INFINITIV = 16,
	GERMAN_PART_OF_SPEECH_COUNT = 17
};


enum GermanGrammemsEnum {
	// unknown 0..3
	gNoaUnk = 0,
	gPredikBenutz = 1,
	gProUnk = 2,
	gTmpUnk = 3,


	// eigennamen 4..14
	gNac = 4,
	gMou = 5,
	gCou = 6,
	gGeo = 7,
	gWasser = 8,
	gGeb = 9,
	gStd = 10,
	gLok = 11,
	gVor = 12,

	//  reflexive Verben
	gSichAcc = 13,
	gSichDat = 14,



	// verb clasess 15..18
	gSchwach = 15,
	gNichtSchwach = 16,
	gModal = 17,
	gAuxiliar = 18,


	// verb forms 19..26
	gKonj1 = 19,
	gKonj2 = 20,
	gPartizip1 = 21,
	gPartizip2 = 22,
	gZuVerbForm = 23,
	gImperativ = 24,
	gPraeteritum = 25,
	gPrasens = 26,

	//adjective 27..29
	gGrundform = 27,
	gKomparativ = 28,
	gSuperlativ = 29,

	// konjunk 30..34
	gProportionalKonjunktion = 30,
	gInfinitiv = 31, // used also for verbs
	gVergleichsKonjunktion = 32,
	gNebenordnende = 33,
	gUnterordnende = 34,



	//pronouns 35..41
	gPersonal = 35,
	gDemonstrativ = 36,
	gInterrogativ = 37,
	gPossessiv = 38,
	gReflexiv = 39,
	gRinPronomen = 40,
	gAlgPronomen = 41,

	//adjective's articles 42.44
	gAdjektiveOhneArtikel = 42,
	gAdjektiveMitUnbestimmte = 43,
	gAdjektiveMitBestimmte = 44,



	//persons 44..47
	gErstePerson = 45,
	gZweitePerson = 46,
	gDrittePerson = 47,

	//genus 48..50
	gFeminin = 48,
	gMaskulin = 49,
	gNeutrum = 50,



	// number 51..52
	gPlural = 51,
	gSingular = 52,


	//cases 53..56
	gNominativ = 53,
	gGenitiv = 54,
	gDativ = 55,
	gAkkusativ = 56,

	// abbreviation
	gAbbreviation = 57,

	//Einwohnerbezeichnung
	gEinwohner = 58,

	//
	gTransitiv = 59,
	gIntransitiv = 60,
	gImpersonal = 61,
	GermanGrammemsCount = 62

};

enum GermanClauseTypeEnum {
	VERBSATZ_T = 0,
	PARTIZIPIALSATZ_T = 1,
	INFINITIVSATZ_T = 2,
	GERMAN_CLAUSE_TYPE_COUNT = 3
};

#endif
#endif