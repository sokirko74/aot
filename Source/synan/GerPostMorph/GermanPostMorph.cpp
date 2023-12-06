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
	m_Language = morphGerman;
	m_GerGramTab = 0;
	m_bCanChangeInputText = false;
    m_bHumanFriendlyOutput = false;
};

CGermanPostMorph::~CGermanPostMorph()
{
};

bool	CGermanPostMorph::Init(const CAgramtab* GerGramTab)
{	
	std::string path = MakePath(GetRegistryString("GerSynan"), "postmorph.grm");
	m_PersonGrammar.InitalizeGrammar(morphGerman, path);
	m_PersonGrammar.LoadGrammarForGLR( true);
	m_GerGramTab = GerGramTab;
	return true;
};

void CGermanPostMorph::ProcessData(CLemmatizedText& text)
{
	m_PersonGrammar.CreateTokenList();
	text = m_PersonGrammar.FilterHomonymsByGrammar(text);
};
