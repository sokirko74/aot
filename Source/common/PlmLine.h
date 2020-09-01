// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Posmorphological Module (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (2003-2004)

#pragma once 

#include "../common/Graspace.h"
class  CAgramtab;

enum MainTokenTypeEnum {RLE=0, LLE=1, NUM=2, NUM_CHAR=3, PUNCTUAT=4, ROMAN_NUM=5, OTHER_TOKEN_TYPE=6};

extern std::string TokenTypeToString(const MainTokenTypeEnum & t);
extern MainTokenTypeEnum StringToTokenType(const  std::string& t);

class CPlmLine {
	std::string	m_GramCodes;
	std::string	m_Word;
	std::string	m_UpperWord;
	char	m_MorphSign;
	std::string	m_CommonGramCode;

public:
	int		m_FilePosition;
	int		m_TokenLengthInFile;
	std::string m_GraphDescr;
	bool   m_bHomonym;
	
	std::string m_Lemma;
	
	std::string m_ParadigmId;
	std::string m_HomoWeight;
	MainTokenTypeEnum   m_TokenType;
	RegisterEnum	   m_Register;	
	bool	m_bToDelete;
	bool	m_bFirstUpperAlpha;
	bool	m_bQuoteMarks;	
	bool	m_bFI1;
	bool	m_bFI2;
	
	bool	m_bName;
	bool	m_bSent2;
	bool	m_bOborot1;
	bool	m_bOborot2;
	bool	m_bHyphenWord;
	bool	m_bMorphName;	
	bool	m_bMorphSurname;
	QWORD	m_Grammems;	
	BYTE	m_Pos;	


	CPlmLine ();

	bool    LoadPlmLineFromString (std::string Buffer, bool bStartLine, const CAgramtab* pRusGramTab);
	std::string  GetStr() const;
	void	DeleteDescr(const char* Descr);
	void	DeleteOb1();
	void	DeleteOb2();
	bool	HasGraDescr(const char* GraphDescr) const;
	void	SetGramCodes(const std::string& NewGramCodes, const CAgramtab* pRusGramTab);
	const std::string&	GetGramCodes() const { return m_GramCodes;};
	bool	IsPartOfNonSingleOborot() const;
	bool	IsFoundInMorphology() const;
	void	SetWord(std::string s);
	const std::string&  GetWord() const;
	const std::string&  GetUpperWord() const;
	const std::string&   GetCommonGramCode() const;
	void			SetMorphUnknown();
	void			SetMorph(char MorphSign, const std::string& CommonAncode, const DWORD& ParadigmId);
	
};


