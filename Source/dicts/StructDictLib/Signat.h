// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Structural Dictionary (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1998-2002)

#pragma once

#include "morph_dict/common/utilit.h"

class TItemContainer;

struct TSignatItem  {
	BYTE m_DomNo;
	bool m_IsMult;
	TSignatItem (BYTE DomNo, bool IsMult)
	{
		m_DomNo = DomNo;
	    m_IsMult = IsMult;
	};
	TSignatItem ()
	{
		m_IsMult = false;
		m_DomNo = 0;
	};
};


// 1. CSignat::SignatId is stored in TCortege, 
// 2. During parsing dictionary articles we read field values from  strings according to the order of signats in fields.json
//    not to the order of CSignat::SignatId's.
// 3. CSignat::SignatId can be changed only for an empty dictionary, ie CSignat::SignatId is immutable.
// 4. One can add a new signat (to the end of the list or inside the list), create a new CSignat::SignatId for not empty dictionaries.


class  CSignat
{
	//runtime
	std::string   sFrmt;
	std::vector<TSignatItem>  DomsWithDelims;
	std::vector<BYTE>  DomsWoDelims;

public:
	// serialized
	BYTE    SignatId;
	std::string  FormatStr;

	std::string   sFrmtWithotSpaces;
   
   
    void BuildSignatFormat(const TItemContainer* parent, BYTE MaxNumDom, std::string fieldStr);
	const std::string& GetFrmt() const;
	const std::vector<BYTE>& GetDomsWoDelims() const;
	const std::vector<TSignatItem>& GetDomsWithDelims() const;
};

