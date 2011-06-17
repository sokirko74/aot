// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#ifndef rus_plmline_h
 #define rus_plmline_h


#include "../SynCommonLib/SynPlmLine.h"


const int  g_BigNumeralsCount = 4;
const char g_BigNumerals[g_BigNumeralsCount][MAX_WORD_LEN]  = { 
												"ÌÈËËÈÎÍ", 
												"ÌÈËËÈÀĞÄ",
												"ÒĞÈËËÈÎÍ",
												"ÒÛÑß×À"
												};

#endif
