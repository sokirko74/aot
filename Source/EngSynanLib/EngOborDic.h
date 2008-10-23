// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#ifndef eng_oborot_h
#define eng_oborot_h

#include "../SynCommonLib/oborot.h"

class CEngOborDic : public COborDic 
{
public:
	

	CEngOborDic(CSyntaxOpt* Opt);

protected:
	bool	ReadOborDic (const CDictionary* piOborDic);
	

};

#endif
