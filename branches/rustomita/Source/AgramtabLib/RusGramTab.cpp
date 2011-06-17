#include "StdGramtab.h"

// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko

#include "RusGramTab.h"




CRusGramTab :: CRusGramTab()
{
	size_t i=0;

	for (; i<GetMaxGrmCount(); i++) 
	    GetLine(i) = NULL;

}

CRusGramTab :: ~CRusGramTab()
{
	for (size_t i=0; i<GetMaxGrmCount(); i++) 
		if (GetLine(i) != NULL) 
			delete GetLine(i);

}
const char* CRusGramTab::GetRegistryString() const 
{
	return "Software\\Dialing\\Lemmatizer\\Russian\\Rgramtab";
};



BYTE	CRusGramTab::GetPartOfSpeechesCount () const 
{
	return rPartOfSpeechCount;
};

size_t CRusGramTab::GetMaxGrmCount() const 
{
	return MaxGrmCount;
}

const char*	CRusGramTab::GetPartOfSpeechStr(BYTE i) const 
{
	return rPartOfSpeeches[i];
};
size_t			CRusGramTab::GetGrammemsCount()  const 
{
	return GrammemsCount;
};
const char*	CRusGramTab::GetGrammemStr(size_t i) const 
{
	return Grammems[i];
};
CAgramtabLine*& CRusGramTab::GetLine(size_t LineNo) 
{
	return Lines[LineNo];
}
const CAgramtabLine* CRusGramTab:: GetLine(size_t LineNo) const 
{
	return Lines[LineNo];
}

size_t CRusGramTab::s2i(const char * s )  const
{ 
	return  (unsigned char) s[0]*0x100+(unsigned char) s[1] - StartUp;
};

string CRusGramTab::i2s(WORD i) const
{ 
	i += StartUp;
	char res[3];

	res[0] = (i >> 8);
	res[1] = (0xFF & i);
	res[2] = 0;
	return  res;
};



bool CRusGramTab :: ProcessPOSAndGrammems (const char* tab_str, BYTE& PartOfSpeech,  QWORD& grammems) const
{
	if (!CAgramtab::ProcessPOSAndGrammems(tab_str, PartOfSpeech, grammems)) return false;

    
    
   // ������������ ����� ��� ����� ����������� ���� �������
   if (			(_QM(rIndeclinable) & grammems)
		&&		(PartOfSpeech != PREDK )
	   )
           grammems |= rAllCases;

   if ( (_QM(rIndeclinable) & grammems) 
	    && (PartOfSpeech == PRONOUN_P )
	  )
   grammems |= rAllGenders | rAllNumbers;


    // ����� ������ ���� ('������') �����  ������������ ��� 
	// ����� �.�., ��� � ��� ����� �.�.
   if (_QM(rMascFem) & grammems)
		 grammems |=   _QM(rMasculinum) | _QM(rFeminum);
		   
   // ����� '������' �� ���������� �� ������, ������� �����
   // ���� ������������ � ����� ������
   if (PartOfSpeech != PREDK )
	   if ( (_QM(rIndeclinable) & grammems) &&  !(_QM(rSingular) & grammems))
			grammems |=   _QM(rPlural) | _QM(rSingular);

   return true;
};






// ����������� ������������ ����� ����� �������  ��  ����� � ������
inline bool CaseNumber (const CAgramtabLine* l1, const CAgramtabLine* l2) 
  {
      return ((rAllCases  & l1->m_Grammems & l2->m_Grammems) > 0) &&
		     ((rAllNumbers & l1->m_Grammems & l2->m_Grammems) > 0) ;
  };

// ����������� ������������ ����� ����� �������  ��  ������, ������ ������ ��� ������
// ����� �������� �������������� ����� 
inline bool CaseFirstPlural (const CAgramtabLine* l1, const CAgramtabLine* l2) 
  {
      return (     ( (rAllCases  & l1->m_Grammems & l2->m_Grammems) > 0) 
		       &&  ( ((1<<rPlural)  & l1->m_Grammems)  > 0)
			 );
  };


// ����������� ������������ ����� ����� �������  �� ���� � ����� 
inline bool GenderNumber(const CAgramtabLine* l1, const CAgramtabLine* l2)
	{
		 
		return 	((rAllNumbers & l1->m_Grammems & l2->m_Grammems) > 0) 
			 && (    ((l1->m_Grammems & l2->m_Grammems & _QM(rPlural)) > 0 ) 
				  || ((rAllGenders & l1->m_Grammems & l2->m_Grammems) > 0)
				);
	}

// ����������� ������������ �� ���� � ����� (��� 1 � 2 ����)
inline bool PersonNumber(const CAgramtabLine* l1, const CAgramtabLine* l2)
{
	size_t t1 = (rAllNumbers & l1->m_Grammems & l2->m_Grammems) > 0;
	size_t t2 = ((rAllPersons & l1->m_Grammems & l2->m_Grammems) > 0);
	return t1 && t2;
}

// ����������� ������������ ����� ���������� � ���������
inline bool SubjectPredicate(const CAgramtabLine* subj_l, const CAgramtabLine* verb_l)
{
    const QWORD& subj =  subj_l->m_Grammems;
    const QWORD& verb =  verb_l->m_Grammems;

	if(    !( subj & _QM(rNominativ)) 
	  )
	return false;


	if  (        ( ( verb & _QM(rPastTense)) > 0)
            ||  ( verb_l->m_PartOfSpeech == ADJ_SHORT)
            ||  ( verb_l->m_PartOfSpeech == PARTICIPLE_SHORT)
        )
	{
		// �� ����� 
		// � �����
		// �� ���
		// � �������
		// �� ���
		// �� ���
		if(subj & (_QM(rFirstPerson) | _QM(rSecondPerson)))
			return		(verb & subj & _QM(rPlural) )
					||	(		(verb & ( _QM(rMasculinum) | _QM(rFeminum) ) )
						  &&	(verb & subj & _QM(rSingular) )
						);	
		else
			// �� �����
			// ����� ����
			// ������� �������
			// ������� ������� 	
			// ������� ������
			return GenderNumber(subj_l, verb_l);
	}
	else 
	if(		( verb & _QM(rPresentTense)) 
		||	( verb & _QM(rFutureTense )) )
	{
		//  � �����
		//  �� �������
		//  �� ������
		if (	( subj & (_QM(rFirstPerson) | _QM(rSecondPerson)))
			||	(verb & (_QM(rFirstPerson) | _QM(rSecondPerson))) )
			return PersonNumber(subj_l, verb_l);
		else
			return (rAllNumbers & subj & verb) > 0;
	}
	else 
	if( verb & _QM(rImperative))
			return		((subj & _QM(rSecondPerson)) > 0)
					&&	((rAllNumbers & subj & verb) > 0);

	return false;
}


// ����������� ������������ ����� ����� �������  ��  ������ 
inline bool Case(const CAgramtabLine* l1, const CAgramtabLine* l2)
{
	 
	return 	((rAllCases & l1->m_Grammems & l2->m_Grammems) > 0); 
}


// ����������� ������������ ����� ��������������� � ���������������� ����, ����� � ������, ���� 
// ���� ��������������� ������������
bool GenderNumberCaseAnimRussian (const CAgramtabLine* l1, const CAgramtabLine* l2) 
{	
    const QWORD& gram_noun = l1->m_Grammems;
    const QWORD& gram_adj =  l2->m_Grammems;

	return			((rAllCases  & gram_noun & gram_adj) > 0) 
		    	 &&	((rAllNumbers & gram_noun & gram_adj) > 0) 
				 &&	(		((_QM(rAnimative) & gram_adj ) > 0) 
						||	((rAllAnimative& gram_adj) == 0)
					)
		         && (    ((rAllGenders & gram_noun & gram_adj) > 0)
					  || ((rAllGenders & gram_noun) == 0)
					  || ((rAllGenders & gram_adj) == 0)
					);
};

// ����������� ������������ ����� ��������������� � ���������������� ����, ����� � ������, ���� 
// ���� ��������������� ��������������
bool GenderNumberCaseNotAnimRussian (const CAgramtabLine* l1, const CAgramtabLine* l2) 
{	
    const QWORD& gram_noun = l1->m_Grammems;
    const QWORD& gram_adj =  l2->m_Grammems;

	return			((rAllCases  & gram_noun & gram_adj) > 0) 
		    	 &&	((rAllNumbers & gram_noun & gram_adj) > 0) 
				 &&	(		((_QM(rNonAnimative) & gram_adj ) > 0) 
						||	((rAllAnimative& gram_adj) == 0)
					)
		         && (    ((rAllGenders & gram_noun & gram_adj) > 0)
					  || ((rAllGenders & gram_noun) == 0)
					  || ((rAllGenders & gram_adj) == 0)
					);
};

// ����������� ������������ ����� ��������������� � ���������������� ����, ����� � ������, ���� 
// ���� ��������������� �� �������������� � �� ������������
//  (��� �����������, ��������, "��� ��� ���� ������")
bool GenderNumberCaseRussian (const CAgramtabLine* l1, const CAgramtabLine* l2) 
{	
    const QWORD& gram_noun = l1->m_Grammems;
    const QWORD& gram_adj =  l2->m_Grammems;

	return			(( rAllCases  & gram_noun & gram_adj) > 0) 
		    	 &&	((rAllNumbers & gram_noun & gram_adj) > 0) 
		         && (    ((rAllGenders & gram_noun & gram_adj) > 0)
					  || ((rAllGenders & gram_noun) == 0)
					  || ((rAllGenders & gram_adj) == 0)
					);
};

bool FiniteFormCoordRussian (const CAgramtabLine* l1, const CAgramtabLine* l2) 
{	
    const QWORD& verb1 = l1->m_Grammems;
    const QWORD& verb2 = l2->m_Grammems;

    // ��� � ���
	if (verb1 &  verb2 & rAllNumbers)
	    if (verb1 &  verb2 & _QM(rPastTense))
	    {

		    if	(		(verb1 &  verb2 & rAllGenders) 
				    ||	( (verb1 &  verb2 & rAllNumbers) == _QM(rPlural))
			    )
			    return true;
	    }
	    else
		    if (verb1 &  verb2 & rAllPersons)
			    return true;

    return false;

};




/*
������� ������ ������� GleicheGenderNumberCase:
- ��������� ����;
- �������� �����;
- �������� ������;
- �������� �������;
- ��������� �����;
- ��������� �������;
- ������� �������
+ �� ���� ���;
+ �������� ����;
+ � ��������  ���;
+ � ������� ������;
+ ��������� ��� �������;
+ ��� ��� 
*/
QWORD CRusGramTab::GleicheGenderNumberCase(const char* common_gram_code_noun, const char* gram_code_noun, const char* gram_code_adj) const
{
	if	(		(common_gram_code_noun == 0)
			||	!strcmp(common_gram_code_noun, "??")
			||	(strlen (common_gram_code_noun) == 0)
		)
			// ��� �������� �� ��������������
		return Gleiche(GenderNumberCaseRussian, gram_code_noun, gram_code_adj);
	else
		if ((GetLine(s2i(common_gram_code_noun))->m_Grammems&_QM(rNonAnimative)) >0)
				// ��������������
			return Gleiche(GenderNumberCaseNotAnimRussian, gram_code_noun, gram_code_adj);
		else
			if ((GetLine(s2i(common_gram_code_noun))->m_Grammems&_QM(rAnimative)) >0)
				// ������������
				return Gleiche(GenderNumberCaseAnimRussian, gram_code_noun, gram_code_adj);
			else
				// ��� �������� �� ��������������
				return Gleiche(GenderNumberCaseRussian, gram_code_noun, gram_code_adj);

}



bool CRusGramTab::GleicheCaseNumber(const char* gram_code1, const char* gram_code2) const
{
	return  Gleiche(CaseNumber, gram_code1, gram_code2) != 0;
}
bool CRusGramTab::GleicheGenderNumber(const char* gram_code1, const char* gram_code2) const
{
	return  Gleiche(GenderNumber, gram_code1, gram_code2) != 0;
}
bool CRusGramTab::GleicheSubjectPredicate(const char* gram_code1, const char* gram_code2) const
{
	return  Gleiche(SubjectPredicate, gram_code1, gram_code2) != 0;
}

bool CRusGramTab::GleicheCase(const char* gram_code1, const char* gram_code2) const
{
	return  Gleiche(Case, gram_code1, gram_code2) != 0;
}


const size_t CRusGramTab::GetClauseTypesCount() const 
{
	return rClauseTypesCount;
};

long CRusGramTab::GetClauseTypeByName(const char* TypeName) const
{
	for(int i = 0 ; i < rClauseTypesCount ; i++ )
	{
		if( !strcmp(rClauseTypes[i], TypeName) )
		{
			return i;
		}
	}

	return -1;
}


const char* CRusGramTab::GetClauseNameByType(long type) const
{
	if (type >= rClauseTypesCount) return 0;
	if (type < 0) return 0;

	return rClauseTypes[type];	
}










/*
	������ ��� ������������� ����� �����.
*/
bool CRusGramTab::IsStrongClauseRoot(const DWORD poses) const
{
	return		(poses & (1<<VERB))
			||	(poses & (1<<ADVERB_PARTICIPLE)) // ������� ������������  ���������� � ���������
												 // �������  ������ 
			||	(poses & (1<<PARTICIPLE_SHORT))
			||	(poses & (1<<ADJ_SHORT))
			||	(poses & (1<<PREDK));
};





char months[12][10] = {"������","�������","����",
		       "������","���","����",
		       "����","������","��������"
		       ,"�������","������","�������"};

bool CRusGramTab::is_month (const char* lemma) const
{
   if( !lemma ) return false;
   for(size_t i = 0 ; i < 12 ; i++ )
     if( !strcmp(lemma,months[i]))
        return true;
   return false;
}

char SmallNumbers[5][10] = {"���","���","������","���","�������"};

bool CRusGramTab::is_small_number (const char* lemma) const  
{
   if( !lemma ) return false;
   for(size_t i = 0 ; i < 5 ; i++ )
     if( !strcmp(lemma,SmallNumbers[i]))
        return true;
   return false;
}


bool CRusGramTab::IsMorphNoun (size_t poses)  const
{
	return  ( poses & ( 1 << NOUN)   );
};




bool CRusGramTab::is_morph_adj (size_t poses) const
{
	return		( poses & (1 <<ADJ_FULL  ))
			|| ( poses & (1 << ADJ_SHORT ) );
};

bool CRusGramTab::is_morph_participle (size_t poses) const
{
	return  ( poses & (1 <<PARTICIPLE  ))
			|| ( poses & (1 << PARTICIPLE_SHORT   ));
};
 
bool CRusGramTab::is_morph_pronoun (size_t poses) const
{
	return  ( poses & (1 <<PRONOUN  )) != 0;
};


bool CRusGramTab::is_morph_pronoun_adjective(size_t poses) const
{
	return  ( poses & (1 <<PRONOUN_P )) != 0;
};


bool CRusGramTab::is_left_noun_modifier  (size_t poses, QWORD grammems) const
{
	return     ( poses & (1 << ADJ_FULL  ))
			|| ( poses & (1 << NUMERAL_P ))
			|| ( poses & (1 << PRONOUN_P ))
			|| ( poses & (1 << PARTICIPLE  )) ;
}


bool CRusGramTab::is_numeral (size_t poses) const
{ 
	return  ( poses & (1 << NUMERAL ) ) != 0;
};

bool CRusGramTab::is_verb_form (size_t poses) const
{
	return     is_morph_participle(poses) 
			||  (poses & ( 1 << VERB )) 
			||  (poses & ( 1 << INFINITIVE ))
			||  (poses & ( 1 << ADVERB_PARTICIPLE ));
};



bool CRusGramTab::is_infinitive(size_t poses) const
{
	return (poses & (1 << INFINITIVE )) != 0; 
}

bool CRusGramTab::is_morph_predk(size_t poses) const
{
	return (poses & (1 << PREDK )) || 
		(poses & (1 << PRONOUN_PREDK )); 
}

bool CRusGramTab::is_morph_adv(size_t poses) const
{
	return (poses & (1 << ADV )) != 0; 
}


bool CRusGramTab::is_morph_personal_pronoun (size_t poses, QWORD grammems) const
{
	return		 ( poses & (1 <<PRONOUN  )) != 0 
			&&   ( grammems &( _QM(rFirstPerson) | _QM(rSecondPerson) | _QM(rThirdPerson)));
};



const size_t  ParticleCount = 8;
const char Particles[ParticleCount][20] = {"��","��","��","��","������", "�", "�", "��"};

bool CRusGramTab::IsSimpleParticle(const char* lemma, size_t poses) const
{
	if (!lemma) return false;
	if (!(poses & (1 << PARTICLE))) return false;
	for(int i = 0 ; i < ParticleCount ; i++ )
		if( !strcmp(lemma,Particles[i]) )
			return true;

	return false;
}









/*
	���� �����  ����. ��������������, ������� ����� ��������� � ���� ���������������:
	"������", "����", "������","���","�������".
	���  ��� �� ����� ������ ������ ����������� �����������,���������  ��� ��� ����� 
	���� ������������ �� ���� �����. ��� �������������� ����� ����� ���� ��� ��, ���
	���������������. ��������,
	� ����� ��� ������, � ������ ��������� ����� ��������
	� ���� ��, ������� ���� �����
	���� ������, ������ ����.
	� ���� ���, ������� ����������.
	������������� ����. �������������� ����������, ��������, �� ����������� "����" � "������", ���������	
	������ ���� ����������� ���-�� �����:
	"��� ������"
	"������ ������"
	�� ��������:
	"������ ������"
	"��� ������"
	����� "������" � "���" - ������� ����������� �� ����.

	!! ����� �������, � �� ���� ����������� � ���������� ��-�, ���� ��� ����� ������������
	!! �� ���� ����� � �������� ����������������.

	1 ����� 2001 ����, �������
*/
bool CRusGramTab::IsSynNoun(size_t poses, const char* Lemma) const
{
	return   IsMorphNoun (poses) 
			||( poses & (1 <<PRONOUN ))
			||(    (poses & (1 <<PRONOUN_P ) )
				&& (	  !strcmp(Lemma, "������") 
					||!strcmp(Lemma, "����") 
					||!strcmp(Lemma, "������") 
					||!strcmp(Lemma, "���") 
					||!strcmp(Lemma, "�������") 
					)
			);
};


const int StandardParamAbbrCount = 8;
const int StandardParamAbbrLength = 4;

const char StandardParamAbbr[StandardParamAbbrCount][StandardParamAbbrLength] =  
{ "��", "��", "��","��", "��","��", "���", "��"};

bool CRusGramTab::IsStandardParamAbbr (const char* WordStrUpper) const
{
	if (strlen(WordStrUpper) > StandardParamAbbrLength) return false;
	for (long  i = 0;  i < StandardParamAbbrCount; i++)
		if ( !strcmp(WordStrUpper,StandardParamAbbr[i]) )
			return true;

	return false;
}


bool CRusGramTab::is_morph_article(size_t poses)  const 
{
	return  false;
};


string RussianCaseNumberGender(const CAgramtab* pGramTab,  const string& adj, const string& common_noun_grm, const string& noun)
{
	assert (false);
	//return pGramTab->GleicheGenderNumberCase(common_noun_grm.c_str(), noun.c_str(), adj.c_str());
	return "";
};

string AgreeCaseNumberGender(const CAgramtab* pGramTab,  const string& adj1, const string& adj2)
{
	return pGramTab->GleicheAncode1(GenderNumberCaseRussian, adj1.c_str(), adj2.c_str());
};

string AgreeCaseNumberGenderAnim(const CAgramtab* pGramTab,  const string& adj1, const string& adj2)
{
    return pGramTab->GleicheAncode1(GenderNumberCaseAnimRussian, adj1.c_str(), adj2.c_str());
};

string AgreeCaseNumberGenderNonAnim(const CAgramtab* pGramTab,  const string& adj1, const string& adj2)
{
    return pGramTab->GleicheAncode1(GenderNumberCaseNotAnimRussian, adj1.c_str(), adj2.c_str());
};

string AgreeCaseNumber(const CAgramtab* pGramTab,  const string& adj1, const string& adj2)
{
    return pGramTab->GleicheAncode1(CaseNumber, adj1.c_str(), adj2.c_str());
};

string AgreeCase(const CAgramtab* pGramTab,  const string& adj1, const string& adj2)
{
    return pGramTab->GleicheAncode1(Case, adj1.c_str(), adj2.c_str());
};

string AgreeNumberGender(const CAgramtab* pGramTab,  const string& adj1, const string& adj2)
{
	return pGramTab->GleicheAncode1(GenderNumber, adj1.c_str(), adj2.c_str());
};


string SetNomAccPlural(const CAgramtab* pGramTab,  const string& s)
{
    assert ((s.length() % 2) == 0);
	string Result;
    QWORD q;
	for (size_t i=0; i < s.length();i+=2)
	{
		const CAgramtabLine* L = pGramTab->GetLine(pGramTab->s2i(s.c_str()+i));
		if (!L) continue;
		//q = (L->m_Grammems & ~_QM(rSingular) & ~rAllCases);
        q = L->m_Grammems | _QM(rPlural) | _QM(rNominativ) | _QM(rAccusativ);
        //q |= (q & ~rAllCases) | _QM(rNominativ) | _QM(rAccusativ);
        Result += pGramTab->GetAllPossibleAncodes(L->m_PartOfSpeech, q);
	};
    return Result;
};

string SetAllCases(const CAgramtab* pGramTab,  const string& s)
{
    assert ((s.length() % 2) == 0);
	string Result;
    QWORD q;
    if (s.empty()) 
    {
        Result = pGramTab->GetAllPossibleAncodes(UnknownPartOfSpeech, rAllCases);
    } 
    else
    {
        for (size_t i=0; i < s.length();i+=2)
        {
            const CAgramtabLine* L = pGramTab->GetLine(pGramTab->s2i(s.c_str()+i));
            if (!L) continue;
            q = L->m_Grammems; 
            q |= rAllCases;      
            Result += pGramTab->GetAllPossibleAncodes(L->m_PartOfSpeech, q);
        }
    }
    return Result;
};
