// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko


#include "stdafx.h"
#include "SynPlmLine.h"
#include "Word.h"




CSynPlmLine::CSynPlmLine()
{
	reset();
}



void CSynPlmLine :: reset ()
{
    m_pActiveWord = 0;
    m_pActiveHomonym = 0;
	m_ClauseType = UnknownSyntaxElement;
    m_gramcodes = 0;
	m_Grammems = 0;
    poses = 0;
	m_Flags = 0;
	m_FoundPrepDependCases = 0;
	m_UnitType = EWord;
	m_DisruptOborotId = UnknownSyntaxElement;
	m_type_gram_code = 0;
}


void CSynPlmLine::SetFlag (PlmLineFlagEnum PlmLineFlag)
{
	//if (bStatus)
		m_Flags |= PlmLineFlag;
	//else
	//	m_Flags &=  ~PlmLineFlag;
};





QWORD CSynPlmLine :: GetGrammems () const
{
	return m_Grammems;
};

void CSynPlmLine :: SetGrammems (QWORD value) 
{
	m_Grammems = value;
};


void CSynPlmLine :: SetMorphByAncodePattern (const CAncodePattern & P) 
{
	m_Grammems = P.m_iGrammems | P.m_TypeGrammems;
	//m_TypeGrammems = P.m_TypeGrammems;
	poses = P.m_iPoses;
	m_gramcodes = P.m_GramCodes.c_str();
	m_type_gram_code = P.m_CommonGramCode.c_str();
};


size_t CSynPlmLine :: GetPoses () const
{
	return poses;
};

bool CSynPlmLine :: HasPOS (BYTE POS) const
{
	return (poses & (1<<POS)) > 0; 
};


bool	CSynPlmLine :: is_single_punct (BYTE c ) const
{
	return		get_word()
			&&	((BYTE)get_word()[0] == c) 
			&&	(get_word()[1] == 0);
	
}


bool CSynPlmLine :: has_lemma() const 
{
	return (get_lemma() != 0) &&  (get_lemma()[0] != 0);
};

bool CSynPlmLine :: is_lemma(const char* lemma) const 
{
	if (!has_lemma()) return false;
	if (lemma == 0) return false;
	return !strcmp(get_lemma(),lemma);

};

bool CSynPlmLine :: is_word_upper(const char* word_upper) const 
{
	if (get_upper_word() == 0) return false;
	if (word_upper == 0) return false;
	return !strcmp(get_upper_word(),word_upper);

};

bool CSynPlmLine :: has_grammem (BYTE g)  const
{
	return		(m_Grammems & _QM(g) )!= 0;
};

bool CSynPlmLine :: is_morph_noun () const
{
	return  HasFlag(fl_morph_noun);
};
bool CSynPlmLine :: is_syn_noun() const
{
	return   HasFlag(fl_syn_noun);
};

bool CSynPlmLine :: HasFlag (PlmLineFlagEnum Flag)  const
{ 
	return (m_Flags & Flag) > 0;
}

void CSynPlmLine :: SetPoses (size_t value)
{ 
	poses = value;
}

void CSynPlmLine :: InitSynPlmLine (const CSynWord* pActiveWord, const CSynHomonym* pActiveHomonym)
{
    m_pActiveWord =  pActiveWord;
    m_pActiveHomonym = pActiveHomonym;
}

const char* CSynPlmLine :: get_word() const
{
   return m_pActiveWord ? m_pActiveWord->m_strWord.c_str() : 0;
}

const char* CSynPlmLine :: get_upper_word() const
{
    return m_pActiveWord ? m_pActiveWord->m_strUpperWord.c_str() : 0;
}
const char* CSynPlmLine :: get_lemma() const
{
    return m_pActiveHomonym ? m_pActiveHomonym->m_strLemma.c_str() : 0;
}

const COborotForSyntax* CSynPlmLine :: get_oborot_ptr() const
{
    if (!m_pActiveHomonym || m_pActiveHomonym->m_OborotNo==-1) 
        return 0;
    else
        return m_pActiveHomonym->GetOborotPtr(); 
}

int     CSynPlmLine :: get_coord_conj_no() const
{
    return m_pActiveHomonym ? m_pActiveHomonym->m_CoordConjNo : 0;
    
}
const vector<int>* CSynPlmLine :: get_simple_preps() const 
{
    return m_pActiveHomonym ? &m_pActiveHomonym->m_SimplePrepNos : 0;
}

bool CSynPlmLine :: has_space_before() const 
{
    return m_pActiveWord ? m_pActiveWord->m_bHasSpaceBefore : false;
}

bool CSynPlmLine :: can_syn_depend_on_adv() const 
{
    return m_pActiveHomonym ? m_pActiveHomonym->m_bCanSynDependOnAdv : false;
}

bool CSynPlmLine :: can_syn_depend_on_adj() const 
{
    return m_pActiveHomonym ? m_pActiveHomonym->m_bCanSynDependOnAdj : false;
}
bool CSynPlmLine :: is_adv_adj() const 
{
    return m_pActiveHomonym ? m_pActiveHomonym->m_bAdvAdj : false;
}

bool CSynPlmLine :: is_month() const 
{
    return m_pActiveHomonym ? m_pActiveHomonym->m_bMonth : false;
}

bool CSynPlmLine :: has_unk_gramcodes() const 
{
    return m_pActiveHomonym ? m_pActiveHomonym->m_bUnkGramcodes : false;
}
bool CSynPlmLine :: has_des(Descriptors g) const 
{
    return m_pActiveWord ? m_pActiveWord->HasDes(g) : false;

}



