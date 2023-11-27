#pragma once

#include "resource.h"

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls

#include <afxcmn.h>			// MFC support for windows Common Controls


#include "common/cortege.h"
#include "morph_dict/common/utilit.h"
#include "seman/SemanLib/TranslatorHolder.h"
#include "seman/SemanLib/struct_dict_holder.h"
#include "RossDev.h"
#include "TckTkWrapper.h"
	

struct CRossDevTextField
{
	uint16_t  StartPosition;
	uint16_t  EndPosition;
	uint32_t StartLine;
	uint32_t EndLine;

	BYTE  FieldNo;
	BYTE  LeafId;
	BYTE  BracketLeafId;
	CRossDevTextField (BYTE _FieldNo, BYTE _LeafId, BYTE _BracketLeafId)
	{
		LeafId = _LeafId;
		BracketLeafId = _BracketLeafId;
		FieldNo = _FieldNo;
	};

	bool operator==(const CRossDevTextField& X) const
		{return    (FieldNo == X.FieldNo)
				&& (BracketLeafId  == X.BracketLeafId)
				&& (LeafId  == X.LeafId); }

};


inline void SelectLine ( CRichEditCtrl& C, long LineNo )
{
	long nStartChar = C.LineIndex(LineNo);
	long nEndChar = nStartChar + C.LineLength(); 
	C.SetSel(nStartChar, nEndChar);
};


