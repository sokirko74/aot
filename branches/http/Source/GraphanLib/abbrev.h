// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Graphematical Module (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1996-2001)

#ifndef abbrev_h
#define abbrev_h

#include  "list"

const  size_t WordSize  = 50;
const  size_t AbbrevSize  = 200;

enum AbbrevItemTypeEnum  {abString, abNumber, abUpperCase, abAny};
struct  CAbbrevItem 
{
	AbbrevItemTypeEnum	m_Type;
	string				m_ItemStr;
	bool operator == (const CAbbrevItem X) const;
	bool operator < (const CAbbrevItem X) const;

	
};
typedef list<CAbbrevItem> CAbbrev;




#endif
