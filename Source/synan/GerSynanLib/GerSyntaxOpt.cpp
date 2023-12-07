#include  "GerSynan.h"
#include  "GerSentence.h"
#include  "GerSyntaxOpt.h"
#include  "GerThesaurus.h"
#include  "dicts/StructDictLib/TempArticle.h"
#include "synan/SimpleGrammarLib/SimpleGrammar.h"


const int gSyntaxGroupTypesCount = 32;
const char gSyntaxGroupTypes [gSyntaxGroupTypesCount][30] = 
{
	"DET_ADJ_SUB", "WWW", "KEYB", "PREP_NOUN", "MODIF_ADJ", 
	"SIMILAR_ADJ", "SIMILAR_NOMEN",	"SIMILAR_ADV", "GEN_NOUN",
	"DIR_OBJ", "NP", "NOUN_ADJ", "VERB_MODIF", 
	"ADJ_SUB",	"FAMIL", "BERUF_NP", "GENIT_NP", 
	"SIMILAR_NP", "APPOSITION", "DISKONT_KONJ", "DISKONT_KONJ_REL", 
	"EXPR",	"SUBJ", 
	"SPNAME", "TITLE", "PNAME", 
	"PERSON", "APPOS", "ADDITION", 
	"MODIF_NUMER", "GEOGR",	"GENIT_PRE"
};

CSentence* CGerSyntaxOpt::NewSentence() const {
	return new CGerSentence(this);
};


CGerSyntaxOpt :: CGerSyntaxOpt (MorphLanguageEnum langua) : CSyntaxOpt(langua)
{
	m_piGramTab = new CGerGramTab();

	m_PluralMask = _QM(gPlural);
	m_SingularMask = _QM(gSingular);
	m_IndeclinableMask = 0;
	m_GenNounGroupType =  gGEN_NOUN;
	m_PrepNounGroupType=    gPREP_NOUN;
	m_DirObjGroupType=  gDIR_OBJ;
	m_NPGroupType=  gNOUN_GR;
	m_NounAdjGroupType=  gNOUN_ADJ;
	m_NameGroupType = gFAMILIE_GROUP;
	m_OborotGroupType = gOBOROTS;
	m_WWWGroupType = gWEB_ADDR;
	m_KEYBGroupType = gKEYB;
	m_SubjRelation = gSUBJ;
	m_RusParenthesis = UnknownPartOfSpeech;
	m_Preposition = gPRP;
	m_Conjunction = gKON;


	m_DisruptConjRelation = gDISRUPT_CONJ_RELATION;
	m_DisruptConjGroupType = gDISKONT_KONJ;
	m_InfintiveClauseType = INFINITIVSATZ_T;


	for (size_t i=0; i < gSyntaxGroupTypesCount; i++)
		m_SyntaxGroupTypes[i] = gSyntaxGroupTypes[i];
	assert(m_SyntaxGroupTypes[gGENIT_PRE] == "GENIT_PRE");
	m_SimilarNPGroupType = GetSyntaxGroupOrRegister("NP_SIMIL");

}

CLemmatizer *CGerSyntaxOpt::NewLemmatizer() const {
	return new CLemmatizerGerman();
};

COborDic * CGerSyntaxOpt::NewOborDic(const CSyntaxOpt* opt)  {
	return new CGerOborDic(opt);
};

CThesaurusForSyntax* CGerSyntaxOpt::NewThesaurus(const CSyntaxOpt* opt) {
	return new CGerThesaurusForSyntax(opt);
};


void CGerSyntaxOpt::DestroyOptions ()
{
	CSyntaxOpt::DestroyOptions();
};

void CGerSyntaxOpt :: InitOptionsLanguageSpecific()
{
	m_piGramTab->LoadFromRegistry();

	auto synan_directory = GetRegistryString("GerSynan");

	//  reading adjektives
	std::string strFileName = (fs::path(synan_directory) / "other_lists/adj_prp.txt").string();
	ReadListFile(strFileName, m_AdjPrp);

	strFileName = MakePath(synan_directory, "synan.grm");
	m_FormatsGrammar.InitalizeGrammar(morphGerman, strFileName);
	m_FormatsGrammar.LoadGrammarForGLR(true);
}




bool CGerSyntaxOpt::is_firm_group(int GroupType) const
{
	return (		(GroupType == gWEB_ADDR ) 
				||	(GroupType == gKEYB ) 
				||	(GroupType == gOBOROTS )
			);
}

bool CGerSyntaxOpt::IsGroupWithoutWeight(int GroupType, const char* cause) const
{
	return false;
};

bool CGerSyntaxOpt::IsSimilarGroup (int type) const
{
	return false;
};
