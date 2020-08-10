// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko
#include "StdGramtab.h"
#include "EngGramTab.h"


CEngGramTab :: CEngGramTab()
{
	for (size_t i = 0; i<GetMaxGrmCount(); i++) 
	    GetLine(i) = NULL;

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
