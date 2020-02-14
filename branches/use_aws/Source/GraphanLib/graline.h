// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Graphematical Module (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1996-2001)


#ifndef  graline_h
#define graline_h
#include  "../common/utilit.h"
#include  "../common/gra_descr.h"




const WORD stSpace		 = 1;
const WORD stEOLN		 = 2;
const WORD stGrouped  = 4;
const WORD stAbbreviation       = 8;
const WORD stNotPrint   = 16;
const WORD stParagraphChar  = 64;
const WORD stEnglishName = 128;

const WORD stKeyModifier = 256;
const WORD stPunct     = 512;
const WORD stElectronicAddress  = 1024;

const WORD stTextAreaEnd      = 2048;
const WORD stSingleSpaceAfter = 4096;
const WORD stIdent       = 8192 ;
const WORD stParagraphTag      = 8192*2 ;
const WORD stPageBreak      = 8192*4 ;


class CGraphmatFile;


struct  CGraphemOborot {
	string m_UnitStr;
	WORD m_UnitNo;
	// стать€ имеет RESTR = fixed !
	bool m_bFixedFet;		
	vector<WORD> m_TokenIds;

	bool operator == (const string& s)
	{
		return s == m_UnitStr;
	};
};



const DWORD UnknownPageNumber  = 0xffffffff;

class CGraLine  
{
	 // Ётот указатель либо указывает на строку выделенную специально дл€ 
	// одного объекта этого класса, либо в m_TokenBuf (с каким-то смещением).
	//  ѕеременна€ m_m_UnitBuf  определена в файле graphmat.cpp
	const char* unit;
 
	// длина  строки с учетом табул€ции (screen length) и разр€дки
	// ƒлина каждого конца строк - один символ.
	BYTE		slen;            

	// длина строки unit
    BYTE		ulen;             
	QWORD		m_Descriptors;
    WORD		m_Status;
	DWORD		m_InputOffset;

	size_t		LengthUntilDelimiters (const char *s, const CGraphmatFile* G);

public:
       
    CGraLine();
	BYTE GetTokenLength() const { return ulen; };
	const char* GetToken() const {return unit; };
	BYTE GetScreenLength() const {return slen;};
	DWORD GetInputOffset() const {return m_InputOffset;};
	QWORD GetDescriptors() const {return m_Descriptors; };

	bool IsNotPrint  () const;
	// we leave these function in the header, since VC doesn't want to make them inline
	bool IsSpace () const {return (m_Status & stSpace) != 0; };
	bool IsEOLN  () const {return (m_Status & stEOLN) != 0; };
	bool IsSoft  () const {return IsSpace() || IsEOLN();};
	bool IsPunct  () const {return (m_Status & stPunct) != 0;};

	bool IsIdent () const;
;
	bool IsEnglishName  () const;
	bool IsGrouped  () const;

	bool IsAbbreviation  () const;
  	bool IsParagraphChar () const;
	bool IsPageBreak() const;
	bool IsTextAreaEnd() const;
	bool IsElectronicAddress()  const;
	bool IsParagraphTag () const;
	bool IsKeyModifier()  const;
	bool IsChar (int c) const;
	bool IsAsterisk () const;
	bool HasSingleSpaceAfter()  const;
	bool IsString (const char* s) const;
	int  ToInt () const;
	bool IsEndTextPeriodDescriptor (int d) const;
	bool HasMacroSyntaxDelimiter () const;
	bool IsWordOrNumberOrAbbr() const;
	

	void SetSpace ();
	void SetPunct ();
	void SetEOLN  ();
	void SetNotPrint();
	void SetParagraphChar();
	void SetParagraphTag();
	void SetKeyModifier();
	void SetSingleSpaceAfter();
	void SetIdent();
	void SetElectronicAddress();
	void SetPageBreak();
	void SetTextAreaEnd();
	void SetEnglishName  ();
	void DelDes(Descriptors d);
	void SetDes(Descriptors d);  
	void MakeSpaces(size_t SpacesLength);
	void AddStatus(WORD add_state);
	void AddLength(const CGraLine& L);
	void SetToken(const char*);
	size_t		ReadWord (size_t Offset, const CGraphmatFile* G, DWORD& PageNumber);
	bool IsSingleSpaceToDelete() const;
};


#endif
