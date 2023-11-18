// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko
#pragma  once


#include "StdSynan.h"
#include "RusOborot.h"
#include "synan/SynCommonLib/SyntaxInit.h"
#include "morph_dict/agramtab/RusGramTab.h"


class CRusSyntaxOpt :  public CSyntaxOpt
{
public:
	long m_lPradigmID_NECHEGO;
	std::string m_Gramcode_NECHEGO;

    std::string m_MasSingNomNounGramCode;

	CLemmaList m_AdvAdj;
	CLemmaList m_SynDependOnAdj;
	CLemmaList m_SynDependOnAdv;
	CLemmaList m_VerbsThatCanSubdueInfinitive;
	CLemmaList m_pVerbsWithInstrObj;
	CLemmaList m_CompAdvList;
	CLemmaList m_NounNumList;
	CLemmaList m_NumberAdverbsList;
	


    CRusSyntaxOpt (MorphLanguageEnum langua);
	void DestroyOptions ();


	void LoadFromRoss(CDictionary* piRossDict);
	void InitOptionsLanguageSpecific();

	bool IsSimilarGroup (int type) const;
	bool IsGroupWithoutWeight(int GroupType, const char* cause) const;
	bool is_firm_group(int GroupType) const;
	CSentence* NewSentence() const override;
	virtual CLemmatizer *NewLemmatizer() const override;
	virtual COborDic * NewOborDic(const CSyntaxOpt* opt) override;
	virtual CThesaurusForSyntax* NewThesaurus(const CSyntaxOpt* opt) override;


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


