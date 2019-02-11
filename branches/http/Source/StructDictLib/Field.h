// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Structural Dictionary (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1998-2002)


#ifndef __FIELD_H_
#define __FIELD_H_

#include "Signat.h"

const size_t FieldStrLen = 100;
struct  CField  
{
       int				FieldId;
       char				FieldStr[FieldStrLen];
	   vector<CSignat>	m_Signats;
       char				TypeRes;
       int				OrderId;
	   bool				IsApplicToActant;
	   CField& operator = (const CField& _F)
	   {
		   FieldId = _F.FieldId;
		   strcpy (FieldStr, _F.FieldStr);
		   TypeRes = _F.TypeRes;
		   OrderId = _F.OrderId;
		   IsApplicToActant = _F.IsApplicToActant;
           	   m_Signats =  _F.m_Signats; 
		   return *this;
	   };

};

#endif //__FIELD_H_
