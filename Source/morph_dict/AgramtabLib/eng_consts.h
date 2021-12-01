// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko, Dmitry Pankratov

#pragma once

// this file is included in a WINDOWS windows IDL,that's why we have to reuse generated constants
#ifndef __AGRAMTABLib_LIBRARY_DEFINED__
#ifndef __SYNANLib_LIBRARY_DEFINED__


 enum EnglishPartOfSpeechEnum
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
	  ePN_ADJ = 16,
	  ENGLISH_PART_OF_SPEECH_COUNT = 17
};


enum EnglishGrammemsEnum {
		eSingular = 0,
		ePlural = 1,
		eMasculinum = 2,
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
		eNames = 32,
        eOrganisation = 33,
		EnglishGrammemsCount = 34
 };

#endif
#endif