#if !defined(AFX_ROSSDEV_H__553CC260_C720_11D2_A6E4_A290D9000000__INCLUDED_)
#define AFX_ROSSDEV_H__553CC260_C720_11D2_A6E4_A290D9000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "ReportDoc.h"

inline  CDocTemplate* GetTemplate (CString Name)
{
	POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
	CDocTemplate* tmpl;
	CString S;
	do {
		tmpl = AfxGetApp()->GetNextDocTemplate (pos);
		tmpl->GetDocString (S, CDocTemplate::regFileTypeId);
		}
	while ((tmpl != 0) &&  (S != Name));

	ASSERT (tmpl);

	return tmpl;
};

inline  CDocTemplate* GetReportTemplate ()
{
	return GetTemplate("VisualSynan.Report");
};

inline  CDocTemplate* GetSynTemplate ()
{
	return GetTemplate("VisualSynan.Document");
};


bool GlobalOpenReport(CString S, CString Name);

#endif