// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#ifndef rus_syntax_init_h
#define rus_syntax_init_h

#pragma warning(disable:4786)



#include "StdSynan.h"
#include "RusOborot.h"
#include "../SynCommonLib/SyntaxInit.h"
#include "../AgramtabLib/RusGramTab.h"


class CRusSyntaxOpt :  public CSyntaxOpt
{
public:
	// ןאנאלוענû סכמגא ÍÅ×ÅÃÎ	
	long m_lPradigmID_NECHEGO;
	string m_Gramcode_NECHEGO;
	
	SDatItems* AdvAdj;
	SDatItems* SynDependOnAdj;
	SDatItems* SynDependOnAdv;
	SDatItems* VerbsThatCanSubdueInfinitive;
	SDatItems* m_pVerbsWithInstrObj;
    

	StringVector* m_pCompAdvList;
    StringVector* m_pNounNumList;
    StringVector* m_pNumberAdverbsList;
	


    CRusSyntaxOpt (MorphLanguageEnum langua);
	void DestroyOptions ();


	void LoadFromRoss(CDictionary* piRossDict);
	bool InitOptionsLanguageSpecific();
	CAgramtab* GetNewGramTab () const;

	bool IsSimilarGroup (int type) const;
	bool IsGroupWithoutWeight(int GroupType, const char* cause) const;
	bool is_firm_group(int GroupType) const;

};


typedef enum {

NOUN_ADJ,				NUMERALS,
C_NUMERALS,				NUMERAL_ADVERB,			NOUN_NUMERAL,        
PREP_NOUN,				SIMILAR_ADJS,			ADV_VERB,
ADV_ADJ ,				ADV_ADV,				COMP_ADV,
NEG_VERB,				DIR_OBJ,				GEN_NOUN,
SIMILAR_ADVS,			VERB_INF,				SIMILAR_INFS,    
NAMES,					SIMILAR_NOUN_GROUPS,	NUMERAL_NOUN,    
NOUN_GR,				SELECTIVE_GR,			WEB_ADDR,			 
SIMILAR_NUMERALS,		MODIF_ADJ,				NOUN_NUMERAL_APPROX, 
ADV_NUM_ADV,			APPROX_PREP_NOUN,		NOUN_PARTICIPLE,		 
SUBJ,					OBOROTS,					SUB_CLAUSE,
NOUN_ADJUNCT,			NOUN_DETACHED_ADJ,		KEYB,
DISRUPT_CONJ,			DISRUPT_CONJ_RELATION,	ADV_PREDIK, 
NOUN_ADJ_POSTPOSITION,	WHOSE_SUBORDINATE,		ANAT_COMPAR, 
PARENTHESIS_CLAUSE,		RANK_NAMES,				FOREIGN_TERM, 
FROM_COMPAR,			DIR_OBJ_REV,			INSTR_OBJ
       
} rGroupTypeEnum;




#endif
