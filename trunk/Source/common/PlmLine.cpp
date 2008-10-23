// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Posmorphological Module (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Alexey Sokirko (1999-2002)

#pragma warning(disable:4786) 
#include "../common/util_classes.h"
#include "PlmLine.h"

#ifdef DETECT_MEMORY_LEAK
	#ifdef _DEBUG
	#define new DEBUG_NEW

	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
	#endif
#endif


CPlmLine::CPlmLine ()
{
	m_bQuoteMarks = false;
	m_bFirstUpperAlpha = false;
	m_bToDelete = false;
	m_bFI1 = false;;
	m_bFI2 = false;
	m_bName = false;;
	m_bSent2 = false;
	m_bHyphenWord = false;
	m_bMorphName = false;
	m_bMorphSurname = false;
	m_TokenType = OTHER_TOKEN_TYPE;
	m_Grammems = 0;
	m_bOborot1 = false;
	m_bOborot2 = false;
	m_Pos= UnknownPartOfSpeech;
	m_FilePosition = 0;
	m_TokenLengthInFile = 0;
	m_HomoWeight = "0";
	m_ParadigmId = "-1";
	m_MorphSign = 0;

};




// выдает смещение, начиная с которого стоит знак морфологического  словаря
// если такого знака нет, то выдается -1
// [-+][:alpha:\?][:alpha:\?]
int GetMorphSignPosition(const char* LineStr) 
{
	int LineLength= strlen(LineStr);
	for (int i = 0; i < LineLength - 3; i++)
	if( 		 ( (LineStr [i] == '+')			 || (LineStr [i] == '-') || (LineStr [i] == '?'))
			&&	 ( is_alpha((BYTE)LineStr [i+1])  || (LineStr [i+1] == '?') )
			&&	 ( is_alpha((BYTE)LineStr [i+2])  || (LineStr [i+2] == '?') )
		)   
		return i;

	return -1; 	
};


bool  IsHomonym(const char* str)
{ 
	return (str[0] == ' '); 
}


string GetWordForm(const string& LineStr)
{
	int ii = 0;
	if( IsHomonym(LineStr.c_str()) )
		ii = 2;
	int word_len = strcspn(LineStr.c_str() + ii," ");
	if (word_len == 0) return "";
	return LineStr.substr(ii, word_len);
};




bool CPlmLine::HasGraDescr(const char* GraphDescr) const
{
  int i =  m_GraphDescr.find(GraphDescr);

  if ( i == -1) return false;

  if (   (i != 0)  
	  && ((unsigned char)m_GraphDescr[i-1] != ' ')
	 )
	 return false;

  int len = strlen(GraphDescr);
  if (   (i+len != len - 1 )  
	  && ((unsigned char)m_GraphDescr[i+len] != ' ')
	 )
	 return false;

  return true;
};





void CPlmLine :: DeleteOb1() 
{
	m_bOborot1 = false;
	for (long i=0; i < m_GraphDescr.length(); i++)
	if (m_GraphDescr.substr(i,5) == "EXPR1")
	{
		m_GraphDescr.erase(i,5);
	}
	else
	if (m_GraphDescr.substr(i,7) == "EXPR_NO")
	{
		m_GraphDescr.erase(i,7);

		while	(			(i < m_GraphDescr.length())
					&&		(isdigit((unsigned char)m_GraphDescr[i]))
				)
		m_GraphDescr.erase(i, 1);
	};
};

void CPlmLine :: DeleteDescr(const char* Descr) 
{
	 long len =  strlen(Descr);
	 for (long i=0; i < m_GraphDescr.length(); i++)
	  if (m_GraphDescr.substr(i,len) == Descr)
	  {
		  m_GraphDescr.erase(i,len);
	  }

};
void CPlmLine :: DeleteOb2() 
{
	m_bOborot2 = false;
	DeleteDescr("EXPR2");
};


bool init_flag(string& GraphDescr, const char* flag_str)
{

  int i =  GraphDescr.find(flag_str);

  if ( i == string::npos) return false;

  if (   (i != 0)  
	  && ((unsigned char)GraphDescr[i-1] != ' ')
	 )
	 return  false;

  int len = strlen(flag_str);
  if (   (i+len != GraphDescr.length() )  
	  && ((BYTE)GraphDescr[i+len] != ' ')
	 )
	 return  false;

  GraphDescr.erase(i, len);
  return true;
};

bool CPlmLine :: LoadPlmLineFromString (string LineStr, bool bStartLine, const CAgramtab* pRusGramTab) 
{
	m_bToDelete = false;
	m_bQuoteMarks = false;
	
	if (!bStartLine)
	{
		m_bHomonym = (LineStr[0] == ' ');

		Trim(LineStr);
		SetWord(::GetWordForm(LineStr) );
	}
	else
	{
		m_bHomonym = false;
		SetWord( "" );

	};


	long i = 1; // the first char can be a space (if this line contains a homonym)
	for (; i < LineStr.length(); i++)
		if (!isspace((BYTE) LineStr[i]) == 0)
			break;

	if (sscanf (LineStr.c_str()+i, "%i %i", &m_FilePosition, &m_TokenLengthInFile) != 2)
		return false;

	// pass all numbers
	for (; i < LineStr.length(); i++)
		 if (		(isdigit((BYTE) LineStr[i]) == 0)
				&&	(isspace((BYTE) LineStr[i]) == 0)
				&&	(((BYTE)LineStr[i]) != '-')
			)
			break;

	int MorphSignPos = GetMorphSignPosition(LineStr.c_str()+i);
	if (MorphSignPos == -1)
		MorphSignPos = LineStr.length();
	else
		MorphSignPos += i; // make MorphSignPos an absolute offset in LineStr

	m_GraphDescr = LineStr.substr (i, MorphSignPos - i);
	/* вставим пробел в начало, потому что часто ищут графету с пробелом в начале,
	например, " ЛЕ"*/
	m_GraphDescr = " " + m_GraphDescr;

	if (MorphSignPos != LineStr.length())
	{
		StringTokenizer tok(LineStr.c_str()+MorphSignPos," ");

		if (!tok() ) return false;
		string MorphSign  = tok.val();
		if (MorphSign.length() != 3) return false;
		m_MorphSign = MorphSign[0];
		m_CommonGramCode = MorphSign.substr(1);

		
		if (!tok() ) return false;
		m_Lemma = tok.val();
		if (m_Lemma.empty())  return false;
		
		if (!tok() ) return false;
		SetGramCodes ( tok.val(), pRusGramTab);

		if (!tok() ) return false;
		m_ParadigmId = tok.val();

		if (!tok() ) return false;
		m_HomoWeight = tok.val();

	};

	m_TokenType = OTHER_TOKEN_TYPE;
	for (int k=(int)RLE; k < OTHER_TOKEN_TYPE; k++)
		if (init_flag (m_GraphDescr, TokenTypeToString((MainTokenTypeEnum)k).c_str() ))
		{
            m_TokenType = (MainTokenTypeEnum)k;
			break;
		};

	if (init_flag (m_GraphDescr, "Aa"))
		m_Register = UpLow;
	else
	if (init_flag (m_GraphDescr, "AA"))
		m_Register = UpUp;
	else
	{
		init_flag (m_GraphDescr, "aa");
		m_Register = LowLow;		
	};

	m_bFirstUpperAlpha =  (m_Register == UpUp) || (m_Register == UpLow);

	m_bFI1 = init_flag (m_GraphDescr, "FAM1");
	m_bFI2 = init_flag (m_GraphDescr, "FAM2");
	m_bName = init_flag (m_GraphDescr, "NAM?");
	m_bSent2 = init_flag (m_GraphDescr, "SENT_END");
	int hyphen_occur = m_Word.find("-");
	m_bHyphenWord = (hyphen_occur != string::npos) && ( (m_TokenType == RLE) ||(m_TokenType == LLE));

	m_bOborot1 = (m_GraphDescr.find("EXPR1") != string::npos);
	m_bOborot2 = (m_GraphDescr.find("EXPR2") != string::npos);
	bool bRomanNumber = is_roman_number(m_Word.c_str(), m_Word.length());
	
	if ((hyphen_occur != string::npos) && (hyphen_occur!=0))
	{
		// "Павла I-го" 
		// "I-го" - одно слово
		bRomanNumber = is_roman_number(m_Word.c_str(), hyphen_occur);
	};
	if (bRomanNumber)
	{
		m_TokenType = ROMAN_NUM;
		m_CommonGramCode = "";
		m_MorphSign = 0;
		m_ParadigmId = "";
	};
	Trim(m_GraphDescr);
	return true;

};



string CPlmLine :: GetStr ()  const
{

	string Result = m_Word;
	if (m_bHomonym) Result = "  "+Result;
	Result += " ";
    Result += Format (" %i %i ", m_FilePosition, m_TokenLengthInFile);


	if (m_TokenType != OTHER_TOKEN_TYPE)
	{
		MainTokenTypeEnum t = m_TokenType;
		if (t == ROMAN_NUM)
			t = NUM;
		Result += " ";
		Result += TokenTypeToString(t);
		Result += " ";
	}

	if (   (m_TokenType == RLE)
		|| (m_TokenType == LLE)
	   )
	{
		if (m_Register == LowLow)
			Result +=  " aa ";
		else
		if (m_Register == UpLow)
			Result +=  " Aa ";
		else
		if (m_Register == UpUp)
			Result +=  " AA ";
	};

	
	Result += m_GraphDescr + " ";

	if (m_bQuoteMarks)
		Result +=  " #QUOTED ";

	if (m_bFI1) 
		Result +=  " FAM1 ";
	if (m_bFI2) 
		Result +=  " FAM2 ";
	if (m_bName) 
		Result +=  " NAM? ";
	if (m_bSent2) 
		Result +=  " SENT_END ";

	if (m_MorphSign != 0)
	{
		Result += m_MorphSign+m_CommonGramCode + " " + m_Lemma + " " + m_GramCodes + " ";
		Result +=  m_ParadigmId + " " + m_HomoWeight;
	};

	return  Result;
};

void	CPlmLine :: SetGramCodes(const string& NewGramCodes, const CAgramtab* pRusGramTab)
{
	m_GramCodes = NewGramCodes;

	if	(		(m_GramCodes != "") 
			&&	(m_GramCodes != "??")
		)
	{
		m_Grammems = pRusGramTab->GetAllGrammems(m_GramCodes.c_str());
		m_Pos = pRusGramTab->GetPartOfSpeech(m_GramCodes.c_str());
	}
	else
	{
		m_Grammems = 0;
		m_Pos = 255;
	};

	if ( (m_CommonGramCode.length() == 2) && (m_CommonGramCode != "??") )
		m_Grammems |= pRusGramTab->GetAllGrammems(m_CommonGramCode.c_str());

};



bool CPlmLine::IsPartOfNonSingleOborot() const
{
	return m_bOborot1 !=  m_bOborot2;
};


bool	CPlmLine::IsFoundInMorphology() const
{
	return	(			(m_TokenType == RLE)
				|| 		(m_TokenType == LLE)
			)
			&&	(		(m_MorphSign != '-') 
					&&	(m_MorphSign != '?')
				);
};


void	CPlmLine::SetWord(string s)
{
	m_Word = s;
	m_UpperWord = s;
	EngRusMakeUpper(m_UpperWord);
};

const string&  CPlmLine::GetWord() const
{
	return m_Word;
};
const string&  CPlmLine::GetUpperWord() const
{
	return m_UpperWord;
};

const string&  CPlmLine::GetCommonGramCode() const
{
	return m_CommonGramCode;
};

void  CPlmLine::SetMorphUnknown()
{
	m_CommonGramCode = "??";
	m_MorphSign = '-';
	m_ParadigmId = "-1";
};

void  CPlmLine::SetMorph(char MorphSign, const string& CommonAncode, const DWORD& ParadigmId)
{
	m_CommonGramCode = CommonAncode.empty() ? "??" : CommonAncode;
	m_MorphSign = MorphSign;
	m_ParadigmId = Format("%i",ParadigmId);
};

string TokenTypeToString(const MainTokenTypeEnum & t)
{
	switch (t){
		case RLE : return "RLE";
		case LLE : return "LLE";
		case NUM : return "DC";
		case ROMAN_NUM : return "ROMAN";
		case NUM_CHAR : return "DSC";
		case PUNCTUAT : return "PUN";
		case OTHER_TOKEN_TYPE : return "OTHER";
		default: assert (false);  return "";
	};
};
MainTokenTypeEnum StringToTokenType(const  string& t)
{
	if (t == "RLE")	return RLE;
	if (t == "LLE")	return LLE;
	if (t == "DC")	return NUM;
	if (t == "ROMAN")	return ROMAN_NUM;
	if (t == "DSC")	return NUM_CHAR;
	if (t == "PUN")	return PUNCTUAT;
	return OTHER_TOKEN_TYPE;
};
