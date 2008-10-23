// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko


#ifndef rus_word_h
#define rus_word_h

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "GerSynan.h"
#include "../SynCommonLib/Word.h"


extern bool is_article_for_weak_declination(const CSynHomonym& H);
extern bool is_article_for_mixed_declination(const CSynHomonym& H);
extern bool IsPrepositionArticle(const char* WordUpper, QWORD& Grammems);


#endif // !defined(AFX_WORD_H__79995343_E12C_11D2_8BE2_00105A68ADF3__INCLUDED_)
