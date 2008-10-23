// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#ifndef ger_oborot_h
#define ger_oborot_h

#include "../SynCommonLib/oborot.h"

class CGerOborDic : public COborDic 
{
public:
	

	CGerOborDic(CSyntaxOpt* Opt);

protected:
	bool	ReadOborDic (const CDictionary* piOborDic);
	

};

#endif
