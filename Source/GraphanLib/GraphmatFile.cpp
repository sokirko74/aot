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
	m_bFilterUnprintableSymbols = false;
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

		if (m_pDicts->m_pOborDictionary.m_Pointer == 0)
		{

			pDicts->m_pOborDictionary.SetPointer(new CDictionary, true);
			std::string Path;
			if (m_Language != morphGerman) 
				Path = GetRegistryString("Software\\Dialing\\Obor\\DictPath").c_str();
			else
				Path = GetRegistryString("Software\\Dialing\\GerObor\\DictPath").c_str();
			if (!pDicts->m_pOborDictionary.m_Pointer->Load(Path.c_str() ) )
			{
				delete pDicts;
				m_LastError = "Cannot load oborots";
				return false;
			};

		}
		else
		{
			pDicts->m_pOborDictionary.SetPointer(m_pDicts->m_pOborDictionary.m_Pointer, m_pDicts->m_pOborDictionary.m_bOwnPointer);
			//  we create a full clone of this share pointer (m_pDicts->GetOborDic())
			// in new pDicts, and afterwards we will delete the current version of m_pDicts
			// and replace it with the new version that' why  we should'not delete 
			// this share pointer when the we free we old pointer.
			const_cast<CGraphanDicts*>(m_pDicts)->m_pOborDictionary.m_bOwnPointer = false;
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













void CGraphmatFile :: GraphmatMain ()
{
	if (0x500000 < GetInputBuffer().size())
	{
		throw CExpc( "File is to large, it cannot be more than 5 MB");
	};


	InitTokenBuffer();

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


	// больше TBuf не нужен, так что освобождаем память
	ClearInputBuffer();


	size_t Count = GetUnits().size();
	for (size_t i=1; i< Count; i++)  
		InitNonContextDescriptors(GetUnit(i));

	
	if (m_bConvertRussianJo2Je)
	{
		size_t Count = GetUnits().size();
		for (size_t i=1; i< Count; i++)  
			if (HasDescr(i, ORLE))
				ConvertJO2Je(const_cast<char*>(GetUnit(i).GetToken()), GetUnit(i).GetTokenLength());
	};

	BuildUnitBufferUpper();

	InitContextDescriptors (0,GetUnits().size());  

	MacSynHierarchy();
	

	if (m_bSentBreaker) {
		DealSentBreaker();
	}
	
	if   (!m_GraOutputFile.empty())
	{
		WriteGraphMat (m_GraOutputFile.c_str());
	}
};


void CGraphmatFile::LoadStringToGraphan(const std::string& szBuffer)
{
	m_GraOutputFile = "";
	m_XmlMacSynOutputFile = "";

	if (!InitInputBuffer(szBuffer)) 
	{
		throw CExpc ("Cannot init inpur buffer for %i bytes", szBuffer.length());
	};

	GraphmatMain();
}


void CGraphmatFile :: LoadFileToGraphan (const std::string&  fileName)
{
	m_SourceFileName = fileName;
	std::string inputText;
	if (IsHtmlFile(m_SourceFileName))
	{
		HTML Convert;
		inputText = Convert.GetTextFromHtmlFile(m_SourceFileName);
	}
	else
	{
		if (!FileExists(m_SourceFileName.c_str())) {
			throw CExpc("Cannot read file %s", m_SourceFileName.c_str());
		}
		LoadFileToString(m_SourceFileName, inputText);

	};
	
	if (!InitInputBuffer(inputText))
	{
		throw CExpc("Cannot init input buffer for %i bytes", inputText.length());
	}
	GraphmatMain ();
};

const std::string&	CGraphmatFile::GetLastError() const
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




