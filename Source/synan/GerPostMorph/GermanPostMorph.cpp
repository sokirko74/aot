#include "GermanPostMorph.h"
#include "morph_dict/lemmatizer_base_lib/MorphanHolder.h"

CPostMorphInteface* NewGermanPostMorph()
{
	CGermanPostMorph* M = new CGermanPostMorph;
	if (!M) return 0;
	auto& h = GetMHolder(morphGerman);
	if (!M->Init(h.m_pGramTab))
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
