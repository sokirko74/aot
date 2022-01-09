
// CCOMSyntaxHolder is a class which is created to work with Dialing Shallow Syntax
// via windows-interfaces. The main methods of the class are the following:
// 1. 	BOOL LoadSyntax(MorphLanguageEnum langua) - loading all dictionaries 
// of the syntax system into the memory for the  given language
// 2. 	BOOL GetSentencesFromSynAn(std::string str)  - building a syntax structure for 
//    the given text ("str")
#pragma once

#include "morph_dict/common/utilit.h"

#include <comdef.h>
#include <atlbase.h>
#import "../Lemmatizer/Lemmatizer.tlb"
#import "../MAPost/MAPost.tlb"
#import "../Agramtab/Agramtab.tlb"
#import "../Graphan/Graphan.tlb"
#import "../Synan/Synan.tlb"


class CCOMSyntaxHolder
{
public:
	MorphLanguageEnum				m_CurrentLanguage;

	GRAPHANLib::IGraphmatFilePtr	m_piGraphan;
	LEMMATIZERLib::ILemmatizerPtr	m_piLemmatizer;
	AGRAMTABLib::IGramTabPtr		m_pGramTab;
	MAPOSTLib::IMAPostPtr m_piMAPost;
	SYNANLib::ISentencesCollectionPtr		m_piSentCollection;
	LEMMATIZERLib::IPLMLineCollectionPtr	m_piAfterMorphPlmLines;
	LEMMATIZERLib::IPLMLineCollectionPtr	m_piBeforeSyntaxPlmLines;

	CCOMSyntaxHolder();
	~CCOMSyntaxHolder();
	BOOL LoadSyntax(MorphLanguageEnum langua);
	
	void DeleteProcessors();
	BOOL LoadSyntaxModule(MorphLanguageEnum langua);

	BOOL BuildBeforeSyntax(std::string str, BOOL bFile, BOOL bWriteIntermFiles, BOOL bSaveIntermResults);
	BOOL BuildSyntax(BOOL bSaveIntermResults);
	BOOL GetSentencesFromSynAn(std::string str, BOOL bFile, BOOL bWriteIntermFiles, BOOL bSaveIntermResults);
	
	
	
};

