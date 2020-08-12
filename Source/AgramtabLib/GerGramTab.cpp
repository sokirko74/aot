// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko
#include "GerGramTab.h"
#include "ger_consts.h"       


extern std::string CommonAncodeAssignFunction(const CAgramtab* pGramTab, const std::string& s1, const std::string& s2);

CGerGramTab :: CGerGramTab()
{
	for (size_t i = 0; i<GetMaxGrmCount(); i++)
	    GetLine(i) = NULL;

	m_Language = morphEnglish;
}


CGerGramTab :: ~CGerGramTab()
{
	for (size_t i=0; i<GetMaxGrmCount(); i++) 
		if (GetLine(i) != NULL) 
			delete GetLine(i);
}



long CGerGramTab::GetClauseTypeByName(const char* TypeName) const
{
	for(int i = 0 ; i < gClauseTypesCount ; i++ )
	{
		if( !strcmp(gClauseTypes[i], TypeName) )
		{
			return i;
		}
	}

	return -1;
}


const char* CGerGramTab::GetClauseNameByType(long type) const

{
	if (type >= gClauseTypesCount) return 0;
	if (type < 0) return 0;

	return gClauseTypes[type];	
}

bool GenderNumberGerman(const CAgramtabLine* l1, const CAgramtabLine* l2)
{
		
	return 		((gAllNumbers & l1->m_Grammems & l2->m_Grammems) > 0)
			&&	(    	( (l1->m_Grammems & l2->m_Grammems & _QM(gPlural) ) > 0 )
					||	((gAllGenders & l1->m_Grammems & l2->m_Grammems) > 0)
				);

	
}

bool CGerGramTab::GleicheGenderNumber(const char* gram_code1, const char* gram_code2) const
{
	return  Gleiche(GenderNumberGerman, gram_code1, gram_code2) != 0;
}



bool SubjectPredicateGerman(const CAgramtabLine* subj_l, const CAgramtabLine* verb_l)
{
    const QWORD& subj = subj_l->m_Grammems;
    const QWORD& verb = verb_l->m_Grammems;

	if(    !( subj & _QM(gNominativ)) 
	  )
	return false;

	if(  verb & _QM(gImperativ)  )  // otherwise "Dienste" is a subject in "die Dienste leistet"
	return false;
	


	QWORD Persons12 = _QM(gErstePerson) | _QM(gZweitePerson);
	QWORD gAllNumbers1 = ( _QM(gPlural) | _QM(gSingular) );
	if (		( subj & Persons12)
			||	(verb & Persons12)
		)
	{
		return		((gAllNumbers & subj & verb)>0) 
				&&	((Persons12 & subj & verb) > 0);
	}
	else
		return (gAllNumbers & subj & verb) > 0;

	return false;
}

bool CGerGramTab::GleicheSubjectPredicate(const char* subj, const char* pred) const
{
	return  Gleiche(SubjectPredicateGerman, subj, pred) != 0;
}




bool CGerGramTab::IsStrongClauseRoot(const poses_mask_t poses) const
{
	return		(poses & (1<<gVER)) != 0;
};



bool CGerGramTab::is_small_number (const char* lemma) const  
{
   return false;
}

bool CGerGramTab::is_month (const char* lemma) const
{
	return false;
};

bool CGerGramTab::IsMorphNoun (poses_mask_t poses)  const
{
	return  ( ( poses & ( 1 << gSUB)   ) >0)
		 || ( ( poses & ( 1 << gEIG)   ) >0);
};

bool CGerGramTab::is_morph_adj (poses_mask_t poses) const
{
	return  ( poses & (1 <<gADJ  )) >0 ;
};

bool CGerGramTab::is_morph_participle (poses_mask_t poses) const
{
	return  ( poses & (1 <<gPA1  ))
			|| ( poses & (1 << gPA2  ));
};
 
bool CGerGramTab::is_morph_pronoun (poses_mask_t poses) const
{
	return  ( poses & (1 <<gPRONOMEN  )) != 0;
};


bool CGerGramTab::is_morph_pronoun_adjective(poses_mask_t poses) const
{
	return	   ( poses & (1 <<gPRO_BEG  )) != 0	;
};

bool CGerGramTab::is_left_noun_modifier (poses_mask_t poses, QWORD grammems) const
{
	if (poses & (1 <<gZAL )) return true;
	if (!(grammems & gAllCases)) return false;
	if (!(grammems & gAllNumbers)) return false;
	return			is_morph_adj(poses) 
				||	is_morph_pronoun_adjective(poses)
				||	( poses & (1 <<gPA1 )) != 0	
				||	( poses & (1 <<gPA2 )) != 0;
}


bool CGerGramTab::is_numeral (poses_mask_t poses) const
{ 
	return  false;
};

bool CGerGramTab::is_verb_form (poses_mask_t poses) const
{
	return 
			is_morph_participle(poses) 
		|| ( poses & (1 <<gVER )) != 0;
};



bool CGerGramTab::is_infinitive(poses_mask_t poses) const
{
	return false; 
}

bool CGerGramTab::is_morph_predk(poses_mask_t poses) const
{
	return false;
}

bool CGerGramTab::is_morph_adv(poses_mask_t poses) const
{
	return  ( poses & (1 <<gADV )) != 0;
}

bool CGerGramTab::is_morph_personal_pronoun (poses_mask_t poses, QWORD grammems) const
{
	return		false;
};

bool CGerGramTab::IsSimpleParticle(const char* lemma, poses_mask_t poses) const
{
	return false;
}




bool CGerGramTab::IsSynNoun(poses_mask_t poses, const char* Lemma) const
{
	return			is_morph_pronoun(poses)
			||		IsMorphNoun(poses);
};


bool CGerGramTab::IsStandardParamAbbr (const char* WordStrUpper) const
{
	return false;
};


bool GleicheCasesGerman(const CAgramtabLine* l1, const CAgramtabLine* l2)
{
	return (l1->m_Grammems & l2->m_Grammems & gAllCases) != 0;
};

bool CGerGramTab::GleicheCase(const char* gram_code_noun, const char* gram_code_adj)  const
{
	return  Gleiche(GleicheCasesGerman, gram_code_noun, gram_code_adj) != 0;
};
bool CGerGramTab::GleicheCaseNumber(const char* gram_code1, const char* gram_code2) const
{
	return false;
};

bool GenderNumberCaseGerman (const CAgramtabLine* noun_l, const CAgramtabLine* adj_l ) 
{	
    const QWORD& noun = noun_l->m_Grammems;
    const QWORD& adj = adj_l->m_Grammems;
	return     ((gAllCases  & noun & adj) > 0) 
		    	 &&	((gAllNumbers & noun & adj) > 0) 
		         && (    ((gAllGenders & noun & adj) > 0)
					  || ((gAllGenders & noun) == 0)
					  || ((gAllGenders & adj) == 0)
					);
};


QWORD CGerGramTab::GleicheGenderNumberCase(const char* common_gram_code_noun,const char* gram_code_noun, const char* gram_code_adj) const 
{
	return  Gleiche(GenderNumberCaseGerman, gram_code_noun, gram_code_adj);
};

bool CGerGramTab::is_morph_article(poses_mask_t poses)  const 
{
	return  ( poses & (1 <<gART  )) != 0;
};


bool WeakGleiche (const CAgramtabLine* noun, const CAgramtabLine* adj) 
{	
    if ((adj->m_Grammems & _QM(gAdjektiveMitBestimmte)) == 0) 
		return false;

	return     GenderNumberCaseGerman(noun, adj);
};


std::string WeakDeclAssignFunction2(const CAgramtab* pGramTab,  const std::string& det, const std::string& noun)
{
	return pGramTab->GleicheAncode1(WeakGleiche, noun.c_str(), det.c_str());
};


std::string WeakDeclAssignFunction3(const CAgramtab* pGramTab,  const std::string& det, const std::string& adj, const std::string& noun )
{
	return		CommonAncodeAssignFunction(pGramTab,
					pGramTab->GleicheAncode1(WeakGleiche, noun.c_str(), det.c_str()),
					pGramTab->GleicheAncode1(WeakGleiche, noun.c_str(), adj.c_str()));
}

bool MixedGleiche (const CAgramtabLine* noun, const CAgramtabLine* adj) 
{	
    if (	((adj->m_Grammems & _QM(gAdjektiveMitUnbestimmte)) == 0) 
		&&	((adj->m_Grammems & _QM(gPossessiv)) == 0) 	
		)
		return false;

	return     GenderNumberCaseGerman(noun, adj);
};

std::string MixedDeclAssignFunction2(const CAgramtab* pGramTab,  const std::string& det, const std::string& noun)
{
	return pGramTab->GleicheAncode1(MixedGleiche, noun.c_str(), det.c_str());
};


std::string MixedDeclAssignFunction3(const CAgramtab* pGramTab,  const std::string& det, const std::string& adj, const std::string& noun )
{
	return		CommonAncodeAssignFunction(pGramTab,
					pGramTab->GleicheAncode1(MixedGleiche, noun.c_str(), det.c_str()),
					pGramTab->GleicheAncode1(MixedGleiche, noun.c_str(), adj.c_str()));
}


bool SoloGleiche (const CAgramtabLine* noun, const CAgramtabLine* adj) 
{	
    if ((adj->m_Grammems & _QM(gAdjektiveOhneArtikel)) == 0) 
		return false;

	return     GenderNumberCaseGerman(noun, adj);
};

std::string StrongDeclAssignFunction(const CAgramtab* pGramTab,  const std::string& det, const std::string& noun)
{
	return pGramTab->GleicheAncode1(SoloGleiche, noun.c_str(), det.c_str());
};



std::string CommonCase(const CAgramtab* pGramTab, const std::string& noun1, const std::string& noun2)
{
	// right now only for German
	return pGramTab->GleicheAncode1(GleicheCasesGerman, noun1.c_str(), noun2.c_str());
};


bool HasOnlyOneCase(const CAgramtab* pGramTab,  const std::string& Ancodes, const poses_mask_t& poses, const QWORD& Grammems)
{
	poses_mask_t oPoses;
	QWORD  oGrammems;
	pGramTab->GetPartOfSpeechAndGrammems((BYTE*)Ancodes.c_str(),  oPoses, oGrammems);
	return  (oGrammems & gAllCases) == Grammems;
};

bool HasGrammem(const CAgramtab* pGramTab,  const std::string& Ancodes, const poses_mask_t& poses, const QWORD& Grammems)
{
	poses_mask_t oPoses;
	QWORD  oGrammems;
	pGramTab->GetPartOfSpeechAndGrammems((BYTE*)Ancodes.c_str(),  oPoses, oGrammems);
	return		((oGrammems & Grammems) == Grammems)
		&&		((oPoses & poses) == poses);
};

bool HasOneGrammem(const CAgramtab* pGramTab,  const std::string& Ancodes, const poses_mask_t& poses, const QWORD& Grammems)
{
	poses_mask_t oPoses;
	QWORD  oGrammems;
	pGramTab->GetPartOfSpeechAndGrammems((BYTE*)Ancodes.c_str(),  oPoses, oGrammems);
	return		(oGrammems & Grammems) > 0;
};


std::string CommonCaseNumberGender(const CAgramtab* pGramTab,  const std::string& adj1, const std::string& adj2)
{
	return pGramTab->GleicheAncode1(GenderNumberCaseGerman, adj1.c_str(), adj2.c_str());
};



std::string ConvertToPlural(const CAgramtab* pGramTab,  const std::string& s)
{
	assert ((s.length() % 2) == 0);
	std::string Result;
	for (size_t i=0; i < s.length();i+=2)
	{
		const CAgramtabLine* L =   pGramTab->GetLine(pGramTab->s2i(s.c_str()+i));
		if (!L) continue;
		if (L->m_Grammems & _QM(gPlural) ) 
		{
			Result += s[i];
			Result += s[i+1];
			
		}
		else
			if (L->m_Grammems & _QM(gSingular) ) 
			{
				QWORD q = (L->m_Grammems & ~_QM(gSingular)) | _QM(gPlural);
				Result += pGramTab->GetAllPossibleAncodes(L->m_PartOfSpeech, q);
			};
	};
	return Result;

};

