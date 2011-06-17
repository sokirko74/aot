// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#ifndef eng_format_caller
 #define eng_format_caller

#include "../SynCommonLib/Group.h"
#include "../SynCommonLib/FormatCaller.h"
#include "EngSyntaxOpt.h"



enum ESynRules {
};

class CEngFormatCaller;

typedef bool(CEngFormatCaller::*simple_word_check_t)(const CSynPlmLine&)const;


class CEngFormatCaller : public CFormatCaller
{
public:
	CEngFormatCaller(const CSyntaxOpt* Opt);
	const CEngSyntaxOpt* GetOpt() const {  return (const CEngSyntaxOpt*)CFormatCaller::GetOpt(); };
	const CEngGramTab* GetEngGramTab() const {  return (const CEngGramTab*)GetOpt()->GetGramTab(); };

	int		GetRuleByGroupTypeForThesaurus(int GroupType) const;
	void	AddAllRules();
	void	AddSimpleSimilarRules();
	void	BuildOborotGroups( ) {};

};





#endif

