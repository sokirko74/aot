// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko, Andrey Putrin

#ifndef __PLMLINECOLLECTION_H_
#define __PLMLINECOLLECTION_H_
#pragma warning (disable : 4786)

#include "../GraphanLib/GraphmatFile.h"       
#include "Lemmatizers.h"       



/////////////////////////////////////////////////////////////////////////////
// CPlmLineCollection

class CPlmLineCollection 
{
public:
	StringVector			m_Items;
	CLemmatizer*			m_pLemmatizer;

	CPlmLineCollection();
	~CPlmLineCollection();

	bool SaveToFile(string filename) const;
	bool ProcessPlmLines(const CGraphmatFile* piGraphmatFile);
	
};

inline bool CheckIfHomonymPlmLine(const char* strPlmLine)
{
	return( strPlmLine[0] == ' ') ;
}


#endif //__COMPLMLINECOLLECTION_H_
