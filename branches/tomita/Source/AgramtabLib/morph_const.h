// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko, Dmitry Pankratov

#ifndef __AGRAMTABLib_LIBRARY_DEFINED__

#ifndef morph_consts_h
 #define morph_consts_h



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
	  _DUMMY = 15,
	  PHRASE = 16,
	  PARTICLE = 17,
	  UNK_GRAMMEMS_TAG = 18,
	  NOUN_g  = 19,
	  ADJ_g = 20,
	  NOUN_n  = 21,
	  ADJ_SHORT = 22,
      PARTICIPLE = 23,
	  ADVERB_PARTICIPLE = 24,
	  PARTICIPLE_SHORT = 25,
	  INFINITIVE = 26,
	  NOUN_o  = 27,
	  ADJ_o = 28
};

 enum ePartOfSpeeches 
 {
	  eNOUN  = 0, 
	  eADJ = 1, 
	  eVERB = 2, 
	  eVBE = 3, 
	  eMOD = 4, 
	  eNUMERAL = 5,
	  eCONJ  = 6, 
	  eINTERJ = 7, 
	  ePREP = 8, 
	  ePARTICLE = 9, 
	  eART = 10, 
	  eADV = 11, 
	  ePN = 12, 
	  eORDNUM = 13,  
	  ePRON = 14,
	  ePOSS = 15,
	  ePN_ADJ = 16
};


enum gPartOfSpeeches 
{
	  gART  = 0, 
	  gADJ = 1, 
	  gADV = 2, 
	  gEIG = 3, 
	  gSUB = 4, 
	  gVER = 5,
	  gPA1  = 6, 
	  gPA2 = 7, 
	  gPRONOMEN = 8, 
	  gPRP = 9, 
	  gKON = 10, 
	  gNEG = 11, 
	  gINJ = 12, 
	  gZAL = 13,  
	  gZUS = 14, 
	  gPRO_BEG = 15
};

enum rGrammems {
		 Musculinum = 0,
		 Feminum    = 1,
		 Neutrum    = 2,
		 MuscFem    = 3,
		 Name	    = 4,
		 SurName	= 5,
		 AllGenders   = (1<<Musculinum) | (1<<Feminum) | (1<<Neutrum),

		 Plural     = 6,
		 Singular   = 7,
		 AllNumbers = (1<<Singular) | (1<<Plural),

		 Nominativ  = 8,
		 Genitiv    = 9,
		 Dativ      = 10,
		 Accusativ  = 11,
		 Instrumentalis = 12,
		 Vocativ    = 13,
		 AllCases   = (1<<Nominativ) | (1<<Genitiv) | (1<<Dativ) | (1<<Accusativ) | (1<<Instrumentalis) | (1<<Vocativ),

		 ShortForm = 14,

		 Animative = 15,
		 NonAnimative = 16,

		 Comparative = 17,

		 Perfective = 18,
		 NonPerfective = 19,

		 NonTransitive = 20,
		 Transitive = 21,

		 ActiveVoice = 22,
		 PassiveVoice = 23,

		 PresenceTime = 24,
		 FutureTime = 25,
		 PastTime = 26,
		 AllTimes   = (1<<PresenceTime) | (1<<FutureTime) | (1<<PastTime),

		 FirstPerson = 27,
		 SecondPerson = 28,
		 ThirdPerson = 29,
		 AllPersons = (1 << FirstPerson) | (1 << SecondPerson) | (1 << ThirdPerson),

		 Imperative = 30,

		 Indeclinable = 31,
		 Initialism = 32,
		 
		 AllGrammems  = 0xFFFFFFFF
    };

enum eGrammems {
		eSingular = 0,
		ePlural = 1,
		eMusculinum = 2,
		eFeminum = 3,
		eAnimative = 4, 
		ePerfective = 5, 
		eNominative = 6, 
		eObjectCase = 7, 
		eNarrative = 8, 
		eGeographics = 9, 
		eProper = 10, 
		ePersonalPronoun = 11, 
		ePossessive = 12, 
		ePredicative = 13, 
		eUncountable = 14, 
		eReflexivePronoun = 15, 
		eDemonstrativePronoun = 16, 
		eMass = 17, 
		eComparativ  = 18, 
		eSupremum = 19,
		eFirstPerson = 20, 
		eSecondPerson = 21, 
		eThirdPerson = 22, 
		ePresentIndef = 23, 
		eInfinitive = 24,
		ePastIndef = 25,
		ePastParticiple = 26,
		eGerund = 27,
		eFuturum = 28,
		eConditional = 29,

		eApostropheS = 30,
        eApostrophe = 31,

		eAllGrammems  = 0xFFFFFFFF

 };

enum gGrammems {
// unknown 0..3
gNoaUnk = 0, 
gPredikBenutz = 1, 
gProUnk = 2,
gTmpUnk = 3,
// verb clasess 4..5
gModal=4,
gAuxiliar=5,

// eigennamen 6..16
gNac=6,
gMou=7,
gCou=8,
gGeo=9,
gWasser=10,
gGeb=11,
gStd=12,
gLok=13,
gA_grammem=14,
gB_grammem=15,
gVor=16,  

// schwach verb 17..18
gSchwach=17,
gNichtSchwach=18,

// verb forms 19..26
gKonj1=19,
gKonj2=20,
gPartizip1=21,
gPartizip2=22,
gZuVerbForm=23,
gImperativ=24,
gPraeteritum=25,
gPrasens=26,

//adjective 27..29
gGrundform=27,
gKomparativ=28,
gSuperlativ=29,

// konjunk 30..34
gProportionalKonjunktion=30,
gInfinitiv=31, // used also for verbs
gVergleichsKonjunktion=32,
gNebenordnende=33,
gUnterordnende=34,



//pronouns 35..41
gPersonal=35,
gDemonstrativ=36,
gInterrogativ=37,
gPossessiv=38,
gReflexiv=39,
gRinPronomen=40,
gAlgPronomen=41,

//adjective's articles 42.44
gAdjektiveOhneArtikel=42,
gAdjektiveMitUnbestimmte=43,
gAdjektiveMitBestimmte=44,



//persons 44..47
gErstePerson=45,
gZweitePerson=46,
gDrittePerson=47,  

//genus 48..51
gFeminin=48,
gMaskulin=49,
gNeutrum=50,
gOhneGenus=51,



// number 52..53
gPlural=52,
gSingular=53,


//cases 54..57
gNominativ=54,
gGenitiv=55,
gDativ=56,
gAkkusativ=57,

// abbreviation
gAbbreviation=58

};


#endif


#endif
