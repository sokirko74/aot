// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#pragma once
#include "../SynCommonLib/SynPlmLine.h"
const std::unordered_set<std::string> g_BigNumerals  = { 
												_R("МИЛЛИОН"), 
												_R("МИЛЛИАРД"),
												_R("ТРИЛЛИОН"),
												_R("КВАДРИЛЛИОН"),
												_R("ТЫСЯЧА")
												};
