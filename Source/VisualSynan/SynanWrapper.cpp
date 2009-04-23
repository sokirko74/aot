// SynanWrapper.cpp : implementation file
//

#include "stdafx.h"
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
	/*if(!m_strCommandLine.IsEmpty())
	{
		char* strCommandLineBuffer;
		strCommandLineBuffer = m_strCommandLine.GetBuffer(m_strCommandLine.GetLength());
		
		//imported functions from synan.dll
		InitSyntax(strInitSynanFile);		
		BuildSyntaxByFile(strCommandLineBuffer);
		ClearSyntax(TRUE,);

		m_strCommandLine.ReleaseBuffer();
		return TRUE;
	}
	else
		return FALSE;*/
	return TRUE;
}