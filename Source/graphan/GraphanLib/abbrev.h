// ==========  This file is under  LGPL, the GNU Lesser General Public License
// ==========  Dialing Graphematical Module (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1996-2001)
#pragma once

#include "morph_dict/common/utilit.h"

enum AbbrevItemTypeEnum  {abString, abNumber, abUpperCase, abAny};
struct  CAbbrevItem 
{
	AbbrevItemTypeEnum m_Type;
	std::string	m_ItemStr;
	bool operator == (const CAbbrevItem X) const;
	bool operator < (const CAbbrevItem X) const;
};

struct CAbbrev {
	std::vector<CAbbrevItem> m_AbbrevItems;
};

