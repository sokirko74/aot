// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#ifndef HomonymsKiller_h
#define HomonymsKiller_h

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RusClause.h"

class CSynHomonymsKiller  
{
public:
	CSynHomonymsKiller(CWordVector* pVectorWord, const CAgramtab* piRusGramTab);
	void AboutTwoPredic(int iFirstWord, int iLastWord);	
	void AboutShortAdj(int iFirstWord, int iLastWord);

private:
	void KillHomonymOfPartOfSpeech(int iWord, int iPartOfSpeech);
	int FindWordWithOneHomonym(int iFirstWord, int iLastWord, BYTE pos);
	CWordVector* m_pVectorWords;
	const CAgramtab*	m_piRusGramTab;	
};



#endif
