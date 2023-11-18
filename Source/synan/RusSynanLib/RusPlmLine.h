// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#pragma once
#include "synan/SynCommonLib/SynPlmLine.h"

const std::unordered_set<std::string> g_BigNumerals  = { 
												"МИЛЛИОН", 
												"МИЛЛИАРД",
												"ТРИЛЛИОН",
												"КВАДРИЛЛИОН",
												"ТЫСЯЧА"
												};
