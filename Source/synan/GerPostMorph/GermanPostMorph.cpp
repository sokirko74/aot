
#include "GermanPostMorph.h"
#include <filesystem>
using namespace

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
	m_Language = morphGerman;
	m_GerGramTab = 0;
	m_bCanChangeInputText = false;
    m_bHumanFriendlyOutput = false;
};

CGermanPostMorph::~CGermanPostMorph()
{
};

bool	CGermanPostMorph::Init(const CAgramtab* GerGramTab)
{	std::string path = MakePath(GetRegistryString("GerSynan"), "postmorph.grm");
	m_PersonGrammar.InitalizeGrammar (GerGramTab, path)
	m_PersonGrammar.LoadGrammarForGLR( true);
	m_GerGramTab = GerGramTab;
	return true;
};

bool	CGermanPostMorph::ProcessData(const CLemmatizedText *piInTextItems)
{
	std::string ErrorMsg;
	m_PersonGrammar.CreateTokenList();
	m_ResultLemWords = m_PersonGrammar.FilterHomonymsByGrammar(GLRRestartParsing, *piInTextItems);
	return !m_ResultLemWords.empty();
};
