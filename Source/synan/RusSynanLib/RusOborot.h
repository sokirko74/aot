// ==========  This file is under  LGPL, the GNU Lesser General Public License
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#ifndef rus_oborot_h
#define rus_oborot_h

#include "synan/SynCommonLib/oborot.h"

class CRusOborDic : public COborDic {
public :
	CRusOborDic(const CSyntaxOpt* Opt);
	
protected:
	bool	ReadOborDic (const CDictionary* piOborDic);

};
#endif
