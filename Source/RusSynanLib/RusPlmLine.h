// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#pragma once
#include "../SynCommonLib/SynPlmLine.h"
const int  g_BigNumeralsCount = 5;
const std::string g_BigNumerals[g_BigNumeralsCount]  = { 
												_R("МИЛЛИОН"), 
												_R("МИЛЛИАРД"),
												_R("ТРИЛЛИОН"),
												_R("КВАДРИЛЛИОН"),
												_R("ТЫСЯЧА")
												};
