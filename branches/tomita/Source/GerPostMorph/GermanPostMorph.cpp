
#include "GermanPostMorph.h"


CPostMorphInteface* NewGermanPostMorph(const CLemmatizer* GerLemmatizer, const CAgramtab* GerGramTab)
{
	CGermanPostMorph* M = new CGermanPostMorph;
	if (!M) return 0;
	if (!M->Init(GerGramTab))
	{

		delete M;
		return 0;
	};
	return M;
}



CGermanPostMorph::CGermanPostMorph()
{
	m_GerGramTab = 0;
	m_bCanChangeInputText = false;
    m_bHumanFriendlyOutput = false;
};

CGermanPostMorph::~CGermanPostMorph()
{
};

bool	CGermanPostMorph::Init(const CAgramtab* GerGramTab)
{
	string FileName = GetRegistryString("SimpleGrammar") +"/"+"person.grm";
	if (access(FileName.c_str(), 04) != 0)
		FileName = GetRegistryString("SimpleGrammar") +"/"+"example.grm";

	m_PersonGrammar.m_Language = morphGerman;
	m_PersonGrammar.m_pGramTab = GerGramTab;
	m_PersonGrammar.m_SourceGrammarFile  = FileName;

	if (!LoadGrammarForGLR( m_PersonGrammar, true, false))
		return false;


	m_GerGramTab = GerGramTab;

	return true;
};

bool	CGermanPostMorph::ProcessData(const CPlmLineCollection *piInTextItems, CPlmLineCollection& piOutTextItems)
{
	
	//if (!m_PersonGrammar.ParseFile(TrieParsingMethod, *piInTextItems, m_GerGramTab, piOutTextItems))
	//	return false;

	// call CreateTokenList to refresh file lists from the disk (only modified files will be refreshed)
	string ErrorMsg;
	if (!m_PersonGrammar.CreateTokenList(ErrorMsg))
	{
		ErrorMessage (ErrorMsg);
		return false;
	};

	if (!m_PersonGrammar.ParseFile(GLRRestartParsing, *piInTextItems, m_GerGramTab, piOutTextItems, false))
		return false;

	return true;
};
