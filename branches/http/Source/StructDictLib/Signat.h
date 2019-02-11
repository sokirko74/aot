// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Structural Dictionary (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1998-2002)


#ifndef __SIGNAT_H_
#define __SIGNAT_H_


#include "../common/utilit.h"

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
const size_t MaxFormatNameLen = 254;
const size_t MaxFormatStrLen = 254;
struct CSignat 
{
   char   sFrmt[512];
   char   sFrmtWithotSpaces[512];
   vector<TSignatItem>  DomsWithDelims;  
   vector<BYTE>  Doms; 
   
   char   FormatStr[255];
   char   FormatName[255];
   int    SignatId;
   int    OrderNo;

   bool operator==(const CSignat& X) const
			{return     OrderNo == X.OrderNo; }

   bool operator<(const CSignat& X) const
	 { return   (OrderNo < X.OrderNo); }

};




#endif //__SIGNAT_H_
