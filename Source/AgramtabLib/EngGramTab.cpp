// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko
#include "EngGramTab.h"
#include "eng_consts.h"

const static std::string EnglishPartOfSpeech[] =
{
	"NOUN",
	"ADJECTIVE",
	"VERB",
	"VBE",
	"MOD",
	"NUMERAL",
	"CONJ",
	"INT",
	"PREP",
	"PART",
	"ARTICLE",
	"ADVERB",
	"PN",
	"ORDNUM",
	"PRON",
	"POSS",
	"PN_ADJ"
};

const static std::string EnglishGrammems[] =
{
	"sg", "pl", "m", "f", "anim", "perf", "nom", "obj", "narr","geo",
	"prop" ,"pers", "poss", "pred", "uncount", "ref", "dem", "mass", "comp", "sup",
	"1", "2", "3", "prsa", "inf", "pasa", "pp", "ing", "fut", "if", "plsq", "plsgs", "name","org" 
};


CEngGramTab :: CEngGramTab()
{
	assert(sizeof(EnglishGrammems) / sizeof(std::string) == EnglishGrammemsCount);
	assert(sizeof(EnglishPartOfSpeech) / sizeof(std::string) == ENGLISH_PART_OF_SPEECH_COUNT);

	for (size_t i = 0; i<GetMaxGrmCount(); i++) 
	    GetLine(i) = NULL;
	m_Language = morphEnglish;
}

CEngGramTab :: ~CEngGramTab()
{
	int Num = 0;
	for (size_t i=0; i<GetMaxGrmCount(); i++) 
		if (GetLine(i) != NULL) 
		{
			delete GetLine(i);
			Num++;
		};
};

BYTE CEngGramTab::GetPartOfSpeechesCount() const { 
	return ENGLISH_PART_OF_SPEECH_COUNT;
};

const char* CEngGramTab::GetPartOfSpeechStr(BYTE i) const { 
	return EnglishPartOfSpeech[i].c_str();
};

size_t CEngGramTab::GetGrammemsCount()  const { 
	return EnglishGrammemsCount;
};

const char* CEngGramTab::GetGrammemStr(size_t i) const { 
	return EnglishGrammems[i].c_str();
};

size_t CEngGramTab::GetMaxGrmCount() const { 
	return eMaxGrmCount; 
};


bool CEngGramTab::GleicheGenderNumber(const char* gram_code1, const char* gram_code2) const
{
	return  false;
}

bool CEngGramTab::GleicheSubjectPredicate(const char* gram_code1, const char* gram_code2) const
{
	return  false;
}


bool CEngGramTab::IsStrongClauseRoot(const poses_mask_t poses)  const
{
	return	false;
};


bool CEngGramTab::is_small_number (const char* lemma) const  
{
   return false;
}

bool CEngGramTab::is_month (const char* lemma) const
{
	return false;
};

bool CEngGramTab::IsMorphNoun (poses_mask_t poses)  const
{
	return  false;
};

bool CEngGramTab::is_morph_adj (poses_mask_t poses) const
{
	return  false;
};

bool CEngGramTab::is_morph_participle (poses_mask_t poses) const
{
	return  false;
};
 
bool CEngGramTab::is_morph_pronoun (poses_mask_t poses) const
{
	return  false;
};

bool CEngGramTab::is_morph_pronoun_adjective(poses_mask_t poses) const
{
	return  false;
};

bool CEngGramTab::is_left_noun_modifier (poses_mask_t poses, QWORD grammems) const
{
	return     false;
}


bool CEngGramTab::is_numeral (poses_mask_t poses) const
{ 
	return  false;
};

bool CEngGramTab::is_verb_form (poses_mask_t poses) const
{
	return     false;
};



bool CEngGramTab::is_infinitive(poses_mask_t poses) const
{
	return false; 
}

bool CEngGramTab::is_morph_predk(poses_mask_t poses) const
{
	return false;
}

bool CEngGramTab::is_morph_adv(poses_mask_t poses) const
{
	return false; 
}

bool CEngGramTab::is_morph_personal_pronoun (poses_mask_t poses, QWORD grammems) const
{
	return		false;
};

bool CEngGramTab::IsSimpleParticle(const char* lemma, poses_mask_t poses) const
{
	return false;
}







bool CEngGramTab::IsSynNoun(poses_mask_t poses, const char* Lemma) const
{
	return false;
};

bool CEngGramTab::IsStandardParamAbbr (const char* WordStrUpper) const
{
	return false;
};



bool CEngGramTab::GleicheCase(const char* gram_code_noun, const char* gram_code_adj) const
{
	return false;
};
bool CEngGramTab::GleicheCaseNumber(const char* gram_code1, const char* gram_code2) const
{
	return false;
};
QWORD CEngGramTab::GleicheGenderNumberCase(const char* common_gram_code_noun, const char* gram_code_noun, const char* gram_code_adj) const 
{
	return 0;
};

bool CEngGramTab::is_morph_article(poses_mask_t poses)  const 
{
	return false;
};


void TestMem1() 
{
	std::string Result;
	Result.append("test",2);
}
