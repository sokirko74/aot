// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#ifndef format_caller_init
	#define format_caller_init

#include "RusFormatCaller.h"

void InitAllRules( CFormatCaller& FormatCaller);
void InitGroupRulesForClause( CFormatCaller& FormatCaller);
void InitRulesBeforeSimClauses( CFormatCaller& FormatCaller);
void InitRulesAfterSimClauses( CFormatCaller& FormatCaller);
void InitSimpleSimilarRules( CFormatCaller& FormatCaller);
void InitSimpleRulesForAnalyticalVerbFormChecking( CFormatCaller& FormatCaller);
void InitSimilarNounGroupRules( CFormatCaller& FormatCaller );

#endif
