// ==========  This file is under  LGPL, the GNU Lesser General Public License
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
	m_MaxSentenceLength = 9000;
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


void CGraphmatFile::ReadIdents(CGraphanDicts* dicts, std::string path)
{
	assert(m_Language != morphUnknown);

	std::ifstream inp(path);
	if (!inp.good()) {
		throw CExpc("cannot read %s", path.c_str());
	}
	std::string q;
	while (std::getline(inp, q))
	{
		Trim(q);
		if (q.empty()) continue;
		assert(q.length() < CriticalTokenLength);
		MakeUpperUtf8(q);
		std::string s8 = convert_from_utf8(q.c_str(), m_Language);
		size_t offset = 0;
		CIdent idnt;
		while (offset < s8.length())
		{
			CGraLine token;
			token.Initialize(m_Language, offset);
			ReadToken(s8.c_str() + offset, token);
			offset += token.GetTokenLength();
			idnt.m_tokens.push_back(token);
		}
		dicts->m_Idents.insert({ idnt.m_tokens[0].GetTokenUpper(), idnt });
	};
};


void CGraphmatFile::LoadDicts (MorphLanguageEnum langua)
{
	m_Language = langua;
	if (!m_pDicts)
	{
		throw CExpc("LoadDicts was already called");
	};
	assert (m_Language != morphUnknown);
	CGraphanDicts* pDicts = new CGraphanDicts(m_Language);
	assert (pDicts);

	pDicts->ReadSpacedWords(GetRegistryString("Software\\Dialing\\Graphan\\SpDicFile"));
	pDicts->ReadENames (GetRegistryString("Software\\Dialing\\Graphan\\ENamesFile"));
	ReadIdents (pDicts, GetRegistryString("Software\\Dialing\\Graphan\\IdentsFile"));
	pDicts->ReadAbbrevations ();

	if ( !pDicts->ReadKeyboard(GetRegistryString("Software\\Dialing\\Graphan\\Keyboard")) )
	{
		throw CExpc("Cannot load Keyboard file");
	};

	pDicts->ReadExtensions(GetRegistryString("Software\\Dialing\\Graphan\\Extensions"));
	if (m_pDicts->m_pOborDictionary.m_Pointer == 0)
	{

		pDicts->m_pOborDictionary.SetPointer(new CDictionary, true);
		std::string Path;
		if (m_Language == morphRussian) 
			Path = GetRegistryString("Software\\Dialing\\Obor\\DictPath").c_str();
		else if (m_Language == morphGerman)
			Path = GetRegistryString("Software\\Dialing\\GerObor\\DictPath").c_str();
		else 
			Path = GetRegistryString("Software\\Dialing\\EngObor\\DictPath").c_str();
		try {
			pDicts->m_pOborDictionary.m_Pointer->Load(Path.c_str());
		}
		catch (std::exception& e) {
			delete pDicts;
			throw CExpc("Cannot load oborots %s", e.what());
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
}

size_t GetInternetAddressStarter(const char* s) {
	if (!strncmp(s, "http://", strlen("http://"))) return strlen("http://");
	if (!strncmp(s, "HTTP://", strlen("http://"))) return strlen("http://");

	if (!strncmp(s, "ftp://", strlen("ftp://"))) return strlen("ftp://");
	if (!strncmp(s, "FTP://", strlen("ftp://"))) return strlen("ftp://");

	if (!strncmp(s, "ftp.", strlen("ftp."))) return strlen("ftp.");
	if (!strncmp(s, "FTP.", strlen("ftp."))) return strlen("ftp.");

	if (!strncmp(s, "www.", strlen("www."))) return strlen("www.");
	if (!strncmp(s, "WWW.", strlen("www."))) return strlen("www.");

	if (!strncmp(s, "www2.", strlen("www2."))) return strlen("www2.");
	if (!strncmp(s, "WWW2.", strlen("www2."))) return strlen("www2.");

	return 0;
}

size_t CGraphmatFile::ReadWord(const char* s, CGraLine& token) const {
	bool bElectronicAddress = GetInternetAddressStarter(s) > 0;
	int i = 0;
	for (i = 0; i < CriticalTokenLength; i++) {

		if (is_alpha((BYTE)s[i])) {
			if (i == 0) // prohibit apostrophe at the first position
				if (s[i] == Apostrophe)
					break;
			continue;
		};


		if (isdigit((BYTE)s[i])) continue;

		if (m_pDicts->IsRegisteredKeyModifier(s, i)) {
			token.SetKeyModifier();
			break;
		};

		if (i > 0) {
			if (s[i] == '-') //  let an inner hyphen be part of the word, for example "test-test"
				if ((i + 1 < CriticalTokenLength) && isdigit((BYTE)s[0]) && isdigit((BYTE)s[i + 1]) &&
					isdigit((BYTE)s[i - 1])) // not ok for "1-2"
					break;
				else continue;
			if ((s[i] == '.') && (i + 1 < CriticalTokenLength)) {
				if ((isdigit((BYTE)s[i - 1]) == isdigit((BYTE)s[i + 1]))
					&& ((m_Language != morphRussian)    // prohibit  "." as a word part  for Russian
						|| !is_russian_alpha((BYTE)s[i - 1])  // for example: г.Самара, В.И.Ленин
						|| !is_russian_alpha((BYTE)s[i + 1])
						)
					)
					continue; //  if "." delimits alphas or digits, let an inner full stops be part of the word, for example "www.lenta.ru" or 1.12.12
				// we exclude cases, if the full stop delimits digits and alphas, since it can lead to tokenization errors, for example such "1.We go to the north;2.We go to the south;"
			};

			if (s[i] == '/') continue; //  let an inner slash be part of the word, for example "TCP/IP"
			if (s[i] == '_') continue; //  let an underscore be part of the word, for example "al_sokirko"
		};

		if (bElectronicAddress) {
			if (s[i] == '.') continue;
			if (s[i] == '_') continue;
			if (s[i] == '/') continue;
			if (s[i] == '\\') continue;
			if (s[i] == ':') continue;
		};
		if (s[i] == '@') {
			// sokirko@medialingua.ru
			if (i + 1 < CriticalTokenLength) {
				if (is_alpha((BYTE)s[i + 1]) || isdigit((BYTE)s[i + 1])) {
					bElectronicAddress = true;
					continue;
				};
			};

		};

		break;
	};
	//  exclude the last full stop or  slash from the word,
	//  for example we  do not consider "Israel/" as one word, but as two tokens "Israel" and "/".
	//  but  do not exclude the last hyphen, cf German examples:
	//   "Reichsfinanz- und Reichsinnenministers"

	for (; i > 0 && (s[i - 1] == '.' || s[i - 1] == '/' || s[i - 1] == ':' || s[i - 1] == '\''); i--);

	if (i == 0) {
		return 1; // return 1, if this is not a word and not a number
	}
	else {
		// sequence "N%" is one token)
		if ((BYTE)s[0] == 'N' && (BYTE)s[1] == '%') {
			return 2;
		};
		if (bElectronicAddress)
			token.SetElectronicAddress();
		return i;
	};
}


// читает из буфера b в структуру 
void CGraphmatFile::ReadToken(const char* in_str, CGraLine& token) const {
	if (token.ReadEolns(in_str)) {
		return;
	};	
	BYTE len = 0;

	/*  if a Bracket occurs ..*/
	if (isbracket((BYTE)in_str[0])) {
		token.SetToken(stPunct, in_str, 1);
		return;
	}

	/* if a Space or Tabulation occurs ... */
	if (token.ReadSpaces(in_str, m_TabSize)) {
		return;
	}
	/*
	последовательность из восклицательных и вопросительных знаков
	(используется как конец предложения)
	*/
	while ( (BYTE)in_str[len] == '?' || (BYTE)in_str[len] == '!') {
		len++;
	}
	if (len > 0) {
		token.SetToken(stPunct, in_str, len);
		return;
	}


	/* if it is a hard delimiter (repeat) */
	while ((
		std::iswpunct(in_str[len])
		|| is_pseudo_graph((BYTE)in_str[len])
		)
		&& (in_str[len] == in_str[0])
		)
	{
		len++;
	}

	if (len > 0) {
		token.SetToken(stPunct, in_str, len);
		return;
	}

	/*  If it is not printable symbol and if the non-printable symbols should be filtered */
	if (((BYTE)in_str[0] < 32)
		|| ((BYTE)in_str[0] == cIonChar)
		|| ((BYTE)in_str[0] == cNumberChar)
		|| ((BYTE)in_str[0] == cPiChar)
		|| ((BYTE)in_str[0] == cCompanyChar)
		|| ((BYTE)in_str[0] == cEllipseChar)
		)
	{
		token.SetToken(stPunct, in_str, 1);
		if ((BYTE)in_str[0] == cParagraph)
			token.SetParagraphChar();
		return;
	}

	if (is_spc_fill(in_str[1])) {
		// spaced words "м а м а"  -> "мама   "
		auto word  = m_pDicts->SearchSpacedWords(in_str, len);
		if (len > 0) {
			token.SetToken(0, word.c_str(), word.length(), len);
			memset(const_cast<char*>(in_str) + word.length(), ' ', len - word.length());
			return;
		}
	}

	/* If a word, number or something else  occurs ...*/
	len = ReadWord(in_str, token);
	token.SetToken(0, in_str, len);
}


void CGraphmatFile :: GraphmatMain ()
{
	size_t offset = 0;
	m_Tokens.clear();
	while(offset < m_InputText.length())
	{
		CGraLine token;
		token.Initialize(m_Language, offset);
		ReadToken(m_InputText.c_str() + offset, token);
		offset += token.GetTokenLength();
			
		if	( !token.IsSingleSpaceToDelete() )
		{
			token.InitNonContextDescriptors(m_bForceToRus);
			m_Tokens.emplace_back(token);
		}
		else
		{
			//  ignore single spaces in order to save memory
			if (!m_Tokens.empty()) {
				m_Tokens.back().SetSingleSpaceAfter();
			}
		};
	}

	// больше TBuf не нужен, так что освобождаем память
	ClearInputBuffer();

	InitContextDescriptors();  

	MacSynHierarchy();

	if (m_bSentBreaker) {
		DealSentBreaker();
	}
	
};


void CGraphmatFile::LoadStringToGraphan(const std::string& szBuffer)
{
	m_XmlMacSynOutputFile = "";
	InitInputText(szBuffer);
	GraphmatMain();
}

static bool IsHtmlFile(const std::string& FileName)
{
	std::string r = FileName;
	MakeLowerUtf8(r);
	return endswith(r, ".htm") || endswith(r, ".html") || endswith(r, ".shtm");
}

void CGraphmatFile :: LoadFileToGraphan (const std::string&  path)
{
	std::string inputText;
	if (IsHtmlFile(path))
	{
		HTML Convert;
		inputText = Convert.GetTextFromHtmlFile(path);
	}
	else
	{
		if (!fs::exists(path)) {
			throw CExpc("Cannot read file %s", path.c_str());
		}
		inputText = LoadFileToString(path, true);

	};
	try {
		InitInputText(inputText);
		GraphmatMain();
	}
	catch (CExpc& e) {
		e.add_to_message(Format(" file: ", path.c_str()));
		throw;
	}
};

const std::string&	CGraphmatFile::GetLastError() const
{
	return m_LastError;
};


const CDictionary* CGraphmatFile :: GetOborDic() const
{
	assert(m_pDicts);
	return m_pDicts->GetOborDic();
};

void CGraphmatFile::WriteGraphMat(std::string filename) const
{
	std::ofstream outp(filename, std::ios::binary);

	for (auto& unit : GetUnits())
	{
		outp << unit.GetGraphematicalLine() << "\n";
	};
}





