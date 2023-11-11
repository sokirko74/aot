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

