
// CCOMSyntaxHolder is a class which is created to work with Dialing Shallow Syntax
// via COM-interfaces. The main methods of the class are the following:
// 1. 	BOOL LoadSyntax(MorphLanguageEnum langua) - loading all dictionaries 
// of the syntax system into the memory for the  given language
// 2. 	BOOL GetSentencesFromSynAn(string str)  - building a syntax structure for 
//    the given text ("str")

 
#ifndef COMSyntaxHolder
#define COMSyntaxHolder

#include "utilit.h"
#include <comdef.h>
#include <atlbase.h>
#import "../../bin/Lemmatizer.tlb"
#import "../../bin/MAPost.tlb"
#import "../../bin/Agramtab.tlb"
#import "../../bin/Graphan.tlb"
#import "../../bin/Synan.tlb"

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

	BOOL BuildBeforeSyntax(string str, BOOL bFile, BOOL bWriteIntermFiles, BOOL bSaveIntermResults);
	BOOL BuildSyntax(BOOL bSaveIntermResults);
	BOOL GetSentencesFromSynAn(string str, BOOL bFile, BOOL bWriteIntermFiles, BOOL bSaveIntermResults);
	
	
	
};
extern string  GetClauseTypeDescr(MorphLanguageEnum	Language, const SYNANLib::IClausePtr& piClause, int ClauseRootNo);
#endif