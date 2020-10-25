#pragma once

#include "resource.h"

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls

#include <afxcmn.h>			// MFC support for Windows Common Controls


#include "../common/cortege.h"
#include "../common/utilit.h"
#include "../SemanLib/TranslatorHolder.h"
#include "../SemanLib/RossHolder.h"
#include "RossDev.h"

	
const int MaxNumDom = 10;


struct CRossDevTextField
{
	WORD  StartPosition;
	WORD  EndPosition;
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


