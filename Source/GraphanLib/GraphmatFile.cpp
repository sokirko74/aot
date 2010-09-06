// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Graphematical Module (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1996-2001)

#include "StdGraph.h"
#include "GraphmatFile.h"
#include "GraphanDicts.h"

#include "HTMLConv.h"
#include "abbrev.h"
#include <time.h>
#include <sys/timeb.h>
#include <sys/stat.h> 


CGraphmatFile :: CGraphmatFile() 
{
	m_pDicts = new CGraphanDicts (morphUnknown);
	m_MinParOfs = 3;
	m_MaxParOfs = 6;
	m_bSentBreaker = true;
	m_bSentenceAsParagraph = false;
	m_bForceToRus = true;;
	m_TabSize = 8;
	m_MaxUpLen =  7000;
	m_bSubdueWrongMacroSynUnitToMainRoot = true;
	m_bMacSynHierarchy = false;
	m_bEmptyLineIsSentenceDelim = true;
	m_bUseParagraphTagToDivide = false;
	m_bUseIndention = true;
	m_bConvertRussianJo2Je = false;
	m_bFilterUnprintableSymbols = true;
	m_MaxSentenceLength = 9000;
	m_bRecognizeShortFIOs = false;
}


CGraphmatFile :: ~CGraphmatFile()
{
	assert (m_pDicts);
	if (m_pDicts) delete m_pDicts;
	FreeTable();
};




void CGraphmatFile::FreeDicts()
{
		try {
			const_cast<CGraphanDicts*>(m_pDicts)->FreeData();
		}
		catch (...)
		{
			assert (false);
			return;
		};
}



bool CGraphmatFile::LoadDicts ()
{
	try {

		if (!m_pDicts)
		{
			assert (false);
			m_LastError = "LoadDicts was already called";
			return false;
		};
		assert (m_Language != morphUnknown);
		CGraphanDicts* pDicts = new CGraphanDicts(m_Language);
		assert (pDicts);

		pDicts->ReadSpaces (GetRegistryString("Software\\Dialing\\Graphan\\SpDicFile"));
		pDicts->ReadENames (GetRegistryString("Software\\Dialing\\Graphan\\ENamesFile"));
		pDicts->ReadIdents (GetRegistryString("Software\\Dialing\\Graphan\\IdentsFile"));
		pDicts->ReadAbbrevations ();

		if ( !pDicts->ReadKeyboard(GetRegistryString("Software\\Dialing\\Graphan\\Keyboard")) )
		{
			m_LastError = "Cannot load Keyboard file";
			return false;
		};

		if ( !pDicts->ReadExtensions(GetRegistryString("Software\\Dialing\\Graphan\\Extensions")) )
		{
			m_LastError = "Cannot load Extensions file";
			return false;
		};

		if (m_pDicts->m_pOborDic.m_Pointer == 0) 
		{

			pDicts->m_pOborDic.SetPointer(new CDictionary, true);
			string Path;
			if (m_Language != morphGerman) 
				Path = GetRegistryString("Software\\Dialing\\Obor\\DictPath").c_str();
			else
				Path = GetRegistryString("Software\\Dialing\\GerObor\\DictPath").c_str();
			if (!pDicts->m_pOborDic.m_Pointer->Load(Path.c_str() ) )
			{
				delete pDicts;
				m_LastError = "Cannot load oborots";
				return false;
			};

		}
		else
		{
			pDicts->m_pOborDic.SetPointer(m_pDicts->m_pOborDic.m_Pointer, m_pDicts->m_pOborDic.m_bOwnPointer);
			//  we create a full clone of this share pointer (m_pDicts->m_pOborDic) 
			// in new pDicts, and afterwards we will delete the current version of m_pDicts
			// and replace it with the new version that' why  we should'not delete 
			// this share pointer when the we free we old pointer.
			const_cast<CGraphanDicts*>(m_pDicts)->m_pOborDic.m_bOwnPointer = false;
		};

		pDicts->BuildOborottos();
		delete m_pDicts;

		m_pDicts = pDicts;
		return true;
	}
	catch (CExpc& C)
	{
		m_LastError = C.m_strCause;
		return false;
	}
	catch (...)
	{
		m_LastError = "general exception";
		return false;
	};
}













bool CGraphmatFile :: GraphmatMain ()
{
	m_LastError = "";

	if (0x500000 < GetInputBuffer().size())
	{
		m_LastError = "File is to large, it cannot be more than 5 MB";
		return false;
	};


	InitTokenBuffer();

	try
	{
		// NUMBER of all UNITS which were read from INPUT file
		size_t  CurrOutBufOffset  = 0;

		// we should process all bytes except the last terminating null
		size_t InputBufferSize = GetInputBuffer().size()-1;

		for (size_t InputOffset  = 0; InputOffset < InputBufferSize ; )
		{
			CGraLine NewLine;
			
			NewLine.SetToken(GetUnitBufferStart() + CurrOutBufOffset);
			DWORD	PageNumber;
			InputOffset = NewLine.ReadWord(InputOffset,this, PageNumber);
			
			//  ignore single spaces in order to save memory
			if	( !NewLine.IsSingleSpaceToDelete() )
			{
				AddUnit(NewLine);  
				CurrOutBufOffset += NewLine.GetTokenLength();
				if (NewLine.IsPageBreak() )
					SetPageNumber(GetUnits().size() - 1,PageNumber);
			}
			else
			{
				assert (!GetUnits().empty());
				GetUnit(GetUnits().size() -1).SetSingleSpaceAfter();
			};
		}
	}
	catch (...)
	{

		m_LastError = "Graphan Error! An exception during tokenization";
		return false;
	};


	// больше TBuf не нужен, так что освобождаем память
	ClearInputBuffer();


	try
	{
		size_t Count = GetUnits().size();
		for (size_t i=1; i< Count; i++)  
			InitNonContextDescriptors(GetUnit(i));
	}
	catch (...)
	{
		m_LastError = "Graphan Error! An exception during ascribing non-context graphematical descriptors";
		return false;
	};


	
	if (m_bConvertRussianJo2Je)
	{
		size_t Count = GetUnits().size();
		for (size_t i=1; i< Count; i++)  
			if (HasDescr(i, ORLE))
				ConvertJO2Je(const_cast<char*>(GetUnit(i).GetToken()), GetUnit(i).GetTokenLength());
	};

	BuildUnitBufferUpper();

	try {
		InitContextDescriptors (0,GetUnits().size());  
	}
	catch (...)
	{
		if (m_LastError.empty())
			m_LastError = "Graphan Error! An exception during ascribing context graphematical descriptors";
		return false;
	};

	try 
	{
		MacSynHierarchy();
	}
	catch (...)
	{
		m_LastError = "Graphan Error! An exception during building macrosyntax structure";
		return false;
	};


	if (m_bSentBreaker)
		if (!DealSentBreaker ())
		{
			m_LastError = "An exception occurred in Sentence breaker";
			return false;
		};



	if   (!m_GraOutputFile.empty())
	{
		//fprintf (stderr, "write to %s\n", m_GraOutputFile.c_str());
		WriteGraphMat (m_GraOutputFile.c_str());
	}


	return true;
};


bool CGraphmatFile::LoadStringToGraphan(const string& szBuffer)
{
	try {
		m_GraOutputFile = "";
		m_XmlMacSynOutputFile = "";

		if (!InitInputBuffer(szBuffer)) 
		{
			m_LastError = Format("Cannot init inpur buffer for %i bytes", szBuffer.length());
			return false;
		};

		return GraphmatMain();
	}
	catch (CExpc& C)
	{
        m_LastError = C.m_strCause;
		return false;
	}
	catch (...)
	{
		m_LastError = Format("general exception in Graphan; Length Of Input = %i", szBuffer.length());
		return false;
	};
}


#ifdef WIN32
#include <io.h>

bool IsOlder (const char* FileName1, const char* FileName2)
{
   struct _stat buf1;
   struct _stat buf2;

   FILE* fp1 = fopen (FileName1, "r");
   FILE* fp2 = fopen (FileName2, "r");
   _fstat( fileno(fp1), &buf1 ); 
   _fstat( fileno(fp2), &buf2 ); 
   fclose (fp1);
   fclose (fp2);
   return buf1.st_mtime < buf2.st_mtime;
};

#else

#include <sys/stat.h>

bool IsOlder (const char* FileName1, const char* FileName2)
{
   struct stat buf1;
   struct stat buf2;

   FILE* fp1 = fopen (FileName1, "r");
   FILE* fp2 = fopen (FileName2, "r");
   fstat( fileno(fp1), &buf1 ); 
   fstat( fileno(fp2), &buf2 ); 
   fclose (fp1);
   fclose (fp2);
   return buf1.st_mtime < buf2.st_mtime;
};

#endif



bool CGraphmatFile :: LoadFileToGraphan (const string&  CommandLine)
{
	try 
	{
		m_SourceFileName = CommandLine.c_str();
		
		if (IsHtmlFile(m_SourceFileName))
		{
			HTML Convert;
			string Text = Convert.GetTextFromHtmlFile(m_SourceFileName);
			
			if (!InitInputBuffer(Text)) 
			{	
				m_LastError = Format("Cannot init inpur buffer for %i bytes", Text.length());
				return false;
			}

		}
		else
		{
			if (access(m_SourceFileName.c_str(), 04) != 0) return  false;
			string Text;
			LoadFileToString(m_SourceFileName, Text);
			if (!InitInputBuffer(Text)) 
			{
				m_LastError = Format("Cannot init inpur buffer for %i bytes", Text.length());
				return false;
			};

		};



		return  GraphmatMain ();

	}
	catch (CExpc& C)
	{
        m_LastError = C.m_strCause;
		return false;
	}
	catch (...)
	{
		m_LastError = "general exception";
		return false;
	};
};

const string&	CGraphmatFile::GetLastError() const
{
	return m_LastError;
};


MorphLanguageEnum CGraphmatFile::GetTokenLanguage (int LineNo) const
{
	if (HasDescr (LineNo, ORLE))
		return morphRussian;
	else
		if (HasDescr (LineNo, OLLE))
		{
			if (m_Language == morphGerman)
				return morphGerman;
			else
				return morphEnglish;
				
		}
		else
			return morphUnknown;
};


bool CGraphmatFile :: StartsFixedOborot(size_t LineNo) const
{
	short OborotNo = GetOborotNo(LineNo);
	return		(OborotNo != -1)
			&&	m_pDicts->m_Oborottos[OborotNo].m_bFixedFet;
};


const CDictionary* CGraphmatFile :: GetOborDic() const
{
	assert(m_pDicts);
	return m_pDicts->GetOborDic();
};




