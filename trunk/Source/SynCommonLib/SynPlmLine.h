// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#ifndef plmline_h
 #define plmline_h

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "Period.h"
#include "oborot.h"
#include "../common/utilit.h"
#include "../common/AncodePattern.h"



enum  PlmLineFlagEnum {
	fl_punct=1<<1,
	fl_russian_odin=1<<2, // for Russian
	fl_oborot1=1<<3,
	fl_oborot2=1<<4,
	fl_in_oborot=1<<5,
	fl_le=1<<6, // for Russian
	fl_ile=1<<7,
	fl_ranknoun=1<<8,
	fl_digit=1<<9,
	fl_ambiguous  =1<<10, 
	fl_comma=1<<11,
	fl_small_number=1<<12, // for Russian
	fl_dg_ch=1<<13,
	fl_standard_param_abbr=1<<14,
	fl_fullstop=1<<15,
	fl_morph_noun=1<<16,
	fl_syn_noun=1<<17,
	fl_relative_clause=1<<18,
	fl_is_russian = 1<<19,
	fl_can_subdue_infinitive = 1<<20,
	fl_can_subdue_instr = 1<<21,
	fl_noun_has_adj_declination = 1<<22

};


class CSynHomonym;
class CSynWord;

class CSynPlmLine
{
	DWORD					m_Flags;

	// a union of all possible grammems, which is built by m_gramcodes
	QWORD					m_Grammems; 


	// all possible part of speeches
	size_t					poses; 
    const CSynHomonym*      m_pActiveHomonym;
    const CSynWord*         m_pActiveWord;
public:	
    
    void InitSynPlmLine (const CSynWord*, const CSynHomonym*);

	// an ID of oborot for disruptive conjunction which should be inited by CFormatCaller::create_disrupt_conj_group
	WORD					m_DisruptOborotId;

	// все падежи из возможных, которыми управляет предлог, вошедший  в предложную группу
	QWORD					m_FoundPrepDependCases;

	
	const char*				m_gramcodes;
	const char*				m_type_gram_code;
	
	
	EUnitType				m_UnitType;
	EClauseType				m_ClauseType;
	CPeriod					m_Clause;

	CSynPlmLine(); 
	void reset ();


	bool	is_single_punct (BYTE c ) const;
	bool	has_lemma() const;
	bool	is_lemma(const char* lemma) const;
	bool	is_word_upper(const char* word_upper) const;
	bool	has_grammem (BYTE g)  const;
	bool	is_morph_noun () const;
	bool	is_syn_noun() const;
	bool	HasFlag (PlmLineFlagEnum Flag)  const;
	QWORD	GetGrammems () const;
	size_t	GetPoses () const;
    bool    HasPOS (BYTE POS) const;

    void	SetFlag (PlmLineFlagEnum PlmLineFlag);
	void	SetGrammems (QWORD value);
	void	SetPoses (size_t value);
	void	SetMorphByAncodePattern (const CAncodePattern & P);
    const char* get_word() const;
    const char* get_upper_word() const;
    const char* get_lemma() const;
    const COborotForSyntax* get_oborot_ptr() const;
    int                     get_coord_conj_no() const;
    const vector<int>*      get_simple_preps() const;
    bool has_space_before() const;
    bool can_syn_depend_on_adv() const;
    bool can_syn_depend_on_adj() const;
    bool is_adv_adj() const;
    bool is_month() const;
    bool has_des(Descriptors g) const;
    bool has_unk_gramcodes() const;
};

typedef vector<CSynPlmLine> CPlmVector;
#endif
