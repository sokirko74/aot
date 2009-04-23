// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#ifndef syn_anal_h
 #define syn_anal_h


#include "Group.h"
#include "SyntaxInit.h"


#define FROM_LEFT 0
#define FROM_RIGHT 1


class CFormatCaller;

typedef bool(CFormatCaller::*CFormatFunc)(CGroup& G); 

struct CFormatCall   
{
		string		m_name;
		int			m_direction;       
        CFormatFunc	m_format;

        CFormatCall();
		CFormatCall(CFormatFunc _format, const char*  _name, int _dir);

		bool operator==(const CFormatCall& X) const
		{ return m_name == X.m_name;};

};

typedef map<int,CFormatCall> CFormatCallMap;
typedef pair<int, int> CIntPair;


class CFormatCaller : public CGroups
{
	bool	format_for_gra_fork(CGroup& G, int GroupType, Descriptors fl_start, Descriptors fl_end, bool bFirstWordIsMain);
public:
	CFormatCallMap FormatCalls; 
	CIntVector	m_vectorRulesNumbers;

	CFormatCaller(const CSyntaxOpt* Opt) : CGroups(Opt) {};
	virtual ~CFormatCaller(){};


	const CAgramtab* GetGramTab() const {  return GetOpt()->GetGramTab(); };
	

	virtual void	AddSimpleSimilarRules( ) = 0;
	virtual void	AddAllRules( ) = 0;
    virtual void	BuildOborotGroups( ) = 0;
	
	

	bool	try_and_step_forward(CFormatCall* FormatCall, CGroup& G, int rule);
	int		main_analyse();
	bool	create_groups_from_termin(const SFoundTermin& FoundTermin);
	

	//  common formats from graphematics
	
	bool	format_for_web_addr(CGroup& G);
	bool	format_for_keyb(CGroup& G);
	bool	format_for_FAM1_FAM2(CGroup& G);
	bool	format_for_oborots(CGroup& G);

	
	const	CGroup* create_disrupt_conj_group(CGroup& G, const CGroup* pFirstGr, const CGroup* pSecondGr, bool pbSearchSubGroups);
	bool	create_repeating_disrupt_conj(CGroup& G, const SDoubleConj& pConj);
	bool	format_for_disrupt_conj(CGroup& G);

	bool is_morph_noun(const CSynPlmLine& L) const;
	bool is_syn_noun(const CSynPlmLine& L) const;
	bool is_morph_adj(const CSynPlmLine& L) const;
	bool is_morph_article(const CSynPlmLine& L) const;
	bool is_morph_adv(const CSynPlmLine& L) const;
	bool is_morph_participle(const CSynPlmLine& L) const;
	bool is_morph_predk(const CSynPlmLine& L) const;
	bool is_verb_form(const CSynPlmLine& L) const;
	bool is_morph_pronoun(const CSynPlmLine& L) const;
	bool is_left_noun_modifier (const CSynPlmLine& L) const;
	bool is_numeral(const CSynPlmLine& L) const;
	bool is_infinitive(const CSynPlmLine& L) const;
	bool is_morph_pronoun_adjective(const CSynPlmLine& L) const;
	int		check_first_part_of_prep_noun (const CGroup& G, QWORD& depend_cases, int PREP_NOUN_GROUP_TYPE) const;
	

	void	ChangeGroupType(CGroup& G, int NewGroupType);

protected:	  
	virtual int		GetRuleByGroupTypeForThesaurus(int GroupType) const = 0;
	void			change_if_has_obor_inside(CGroup& group, vector<CPeriod>& oborots);
	int				FindEndOfExpression(int StartWordNo) const;

};






#define W1 sent[G.m_iFirstWord]
#define W2 sent[G.m_iFirstWord+1]
#define W3 sent[G.m_iFirstWord+2]
#define Wi sent[i]
#define Wk sent[k]
#define Wj sent[j]






#endif
