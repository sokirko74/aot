// SynanWrapper.cpp : implementation file
//

#include "StdAfx.h"
#include "VisualSynan.h"
#include "SynanWrapper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSynanWrapper

CSynanWrapper::CSynanWrapper()
{
	m_strCommandLine.Empty();
}

CSynanWrapper::CSynanWrapper(CString strCommandLine)
{	
	m_strCommandLine = strCommandLine;
}

CSynanWrapper::~CSynanWrapper()
{
}



BOOL CSynanWrapper::BuildSyntaxByCommandLine()
{
	return TRUE;
}