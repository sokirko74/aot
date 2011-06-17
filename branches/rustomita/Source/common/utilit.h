// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Alexey Sokirko

#ifndef utilit_h
 #define utilit_h

#include  <stdio.h>


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#ifdef WIN32

	// it is used to find memory leaks
	#ifdef DETECT_MEMORY_LEAK
		extern void* operator new(size_t nSize, const char* lpszFileName, int nLine);
		extern void  operator delete(void* p, const char* lpszFileName, int nLine);
		#define DEBUG_NEW new(THIS_FILE, __LINE__)
	#endif
	

	#include "windows.h"
	#include "winuser.h"
#endif

#include  <assert.h>
#include  <stdarg.h>

#include  <ctype.h>
#include  <string>
#include  <algorithm>
#include  <stdexcept>
#include  <vector>
#include  "set"
#include  "stack"
#include  "map"
#include  "time.h"
#include  <string.h>


#pragma warning (disable : 4018)
#pragma warning (disable : 4244)
#pragma warning (disable : 4103)
#pragma warning  (disable : 4530)
#pragma warning  (disable : 4251)
#pragma warning  (disable : 4996)
		

#ifdef STLPORT
	using namespace stlport;
#else
	using namespace std;
#endif





//#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
//using namespace std;
//#endif



typedef unsigned char BYTE;

#ifdef WIN32
	#include  <io.h>
	#include <fcntl.h>
    #ifndef _WINDNS_INCLUDED_
	    typedef __int64 QWORD;
    #endif
	
#else
	#include  <unistd.h>
	typedef unsigned int	DWORD;	
	typedef unsigned short	WORD;
	typedef long long		QWORD;
	typedef unsigned int	UINT;
	const   unsigned int _MAX_PATH = 512;
#endif

typedef vector<string> StringVector;
typedef set<string> StringSet;
typedef vector<DWORD> DwordVector;

inline QWORD GetMaxQWORD ()
{
	#ifdef WIN32
		return  0xffffffffffffffff;
	#else
		return  0xffffffffffffffffULL;
	#endif
};

const BYTE   cHyphenChar   = (BYTE) '-';
const BYTE   cPeriodChar   = (BYTE) '.';
const BYTE   cEofChar      = (BYTE) 0x1a;
const BYTE   cPtrChar      = (BYTE) 0x13;
const BYTE   cSpaceChar    = (BYTE) 32;
const BYTE   cBackChar     = (BYTE) 'Ђ';
const BYTE   cFullChar    = (BYTE) '«';
const BYTE   cNumChar      = (BYTE) 'N';
const BYTE   cAmpChar      = (BYTE) '&';
const BYTE   cParagraph    = (BYTE) 21;
const BYTE   cCompanyChar  = (BYTE) 176;
const BYTE   cIonChar      = (BYTE) 183;
const BYTE   cNumberChar   = (BYTE) 0xB9;
const BYTE   cPiChar       = (BYTE) 182;
const BYTE   cEllipseChar  = (BYTE) 133;

// morphology&syntax
typedef enum {	morphUnknown = 0, morphRussian = 1, morphEnglish = 2, morphGerman = 3, morphGeneric = 4, morphURL=5, morphDigits=6} MorphLanguageEnum;

template <class T1, class T2, class T3>
struct	troika : public pair<T1, T2>
{
	T3 third;
};


typedef QWORD file_off_t;

class CExpc
{
 public:
	string	m_strCause;
	int		m_ErrorCode;

    CExpc(int ErrorCode, const string& Cause);
	CExpc(const string& Cause);
	CExpc(const CExpc& from);
    CExpc(const char* format, ... );
	CExpc& operator= (const CExpc& from);
};


// working with files
extern bool			FileExists (const char *FName);
extern file_off_t	FileSize (const char *filename);
extern bool			FSeek(FILE* fp, file_off_t pos, int origin);
extern file_off_t	FTell(FILE* fp);



extern void		ErrorMessage (const string& Titul, const string& Message);
extern void		ErrorMessage (const string& Message);
extern string	MakeFName ( const string& InpitFileName,  const string& Ext);
extern bool		MakePath (const char* RossPath, const char* FileName, char* FullPath);
extern string	GetPathByFile (string FileName);
extern bool		IsBinFile (const char* FileName);
extern bool		IsEmptyLine ( const char *t);
extern bool		IsHtmlFile (const string& FileName);
extern void		AddFile(const char* MainFile, const char* ToAdd);
extern bool     RmlMoveFile(const char *oldpath, const char *newpath);
extern bool     RmlCopyFile(const char *oldpath, const char *newpath);
extern string	CreateTempFileName();
extern bool		MakeDir(const string& txt);
extern bool     RemoveWithPrint (const string& FileName);

// working with registry 
extern string	GetRegistryString (string RegistryPath);
extern string	GetRegistryStringFromLocalIniFile (string RegistryPath);
extern bool		CanGetRegistryString (string RegistryPath);
extern void		SetRegistryString (string RegistryPath, string Value);
extern bool		IsRmlRegistered(string& Error);
extern bool		CheckEvaluationTime();
extern struct tm  RmlGetCurrentTime ();
extern bool		ReadTimeOutFromRegistry(bool bReadFromLocalFile, int& TimeOut);
extern string	GetIniFilePath();
extern string	GetRmlVariable();
extern string   BuildRMLPath (const char* s);

// working with strings
extern char*	rtrim (char* s);
extern bool		IsSuperEqualChar (BYTE ch1, BYTE ch2, MorphLanguageEnum langua);
extern bool		strscmp ( const char *s1, const char *s2, size_t l, MorphLanguageEnum langua);
extern int		CompareWithoutRegister ( const char *s1, const char *s2, size_t l, MorphLanguageEnum langua);
extern BYTE		force_rus_char (BYTE ch);
extern bool		force_to_rus (char* dest, const char* sour, size_t len);
extern char*	IntToStr (int Value, char* Buffer);
extern string&	IntToStr (int Value, string& oBuffer);
extern string	Format( const char* format, ... );
extern string&  TrimLeft (string& str);
extern string&  TrimRight (string& str);
extern string&	Trim (string& str);
extern void		rml_TRACE( const char* format, ... );
extern bool		LoadFileToString(string FileName, string& Result);
extern void		SqueezeSpacesAndDeleteEoln(string& s);
extern void		KOI8ToWin (string& s);
extern void		WinToKOI8 (string& s);
extern DWORD	StringCrc32(const char* szString);



extern void QPEncodeString(string& s);
extern void QPDecodeString(string& s);
#ifndef WIN32
	extern void strrev(char* s);
#endif

const BYTE	UnknownPartOfSpeech		= 0xff;
const int	UnknownSyntaxElement = 0xffff;
typedef enum {DontKillHomonyms = 0, CoverageKillHomonyms = 1} KillHomonymsEnum;

typedef enum { LocThes = 0, FinThes, CompThes, OmniThes, NoneThes } EThesType;
typedef enum { EClause = 0, EWord, EGroup, ENoneType } EUnitType;


extern int GetPredictionPartOfSpeech(const string& PartOfSpeech, MorphLanguageEnum langua);
extern bool GetLanguageByString (string s, MorphLanguageEnum& Result);
extern string GetStringByLanguage (MorphLanguageEnum Langua);




/*
clears also capacity
*/
template <class T> 
void ClearVector(vector<T>& V)
{
	V.clear();
	vector<T> dummy (V);
	V.swap (dummy);
};





// ============     Brackets =========================


extern int isbracket  (BYTE x);
extern size_t dual_bracket (BYTE x);



//==================    Digits   ===========================
extern bool is_upper_roman_digit (BYTE ch);
extern bool is_lower_roman_digit (BYTE ch);
extern bool is_roman_number ( const char *s, size_t len);
extern bool is_pseudo_graph(BYTE x);
extern bool is_word_delim (BYTE x);
//  =============  Punctuation Letters ======================

extern	bool is_spc_fill (BYTE x);




extern  bool is_english_upper(BYTE x);
extern  bool is_english_lower(BYTE x);

const BYTE Auml  = 196; // "Д"
const BYTE auml  = 228; // "д"
const BYTE Uuml  = 220; // "Ь"
const BYTE uuml  = 252; // "ь"
const BYTE Ouml  = 214; // "Ц"
const BYTE ouml  = 246; // "ц"
const BYTE szlig  = 223; //"Я"
const BYTE Nu  = 181;   // "ч"
const BYTE agrave  = 224; //"р"
const BYTE egrave  = 232; //"ш"
const BYTE eacute  = 233; //"щ"


const BYTE LowerJO  = (BYTE)'ё';
const BYTE UpperJO  = (BYTE)'Ё';
const BYTE Apostrophe  = 39;



 
extern  bool is_german_upper(BYTE x); 
extern  bool is_german_lower(BYTE x);
extern  bool is_russian_upper(BYTE x);     
extern  bool is_russian_lower(BYTE x);

extern  bool is_lower_consonant(BYTE x, MorphLanguageEnum Langua);
extern  bool is_upper_consonant(BYTE x, MorphLanguageEnum Langua);
extern  bool is_lower_vowel(BYTE x, MorphLanguageEnum Langua);
extern  bool is_upper_vowel(BYTE x, MorphLanguageEnum Langua);

extern  bool is_english_alpha(BYTE x);
extern  bool is_russian_alpha(BYTE x);
extern  bool is_german_alpha(BYTE x);

extern  bool is_alpha (BYTE x);
extern  bool is_alpha (BYTE x, MorphLanguageEnum langua);
extern  bool is_lower_alpha(BYTE x, MorphLanguageEnum langua);
extern  bool is_upper_alpha(BYTE x, MorphLanguageEnum langua);
extern  bool isnspace(BYTE x);


// ===============  Register ========================================
extern  BYTE etoupper (BYTE ch);
extern  BYTE etolower (BYTE ch);
extern  BYTE rtoupper (BYTE ch); 
extern  BYTE rtolower (BYTE ch); 
extern  BYTE gtoupper (BYTE ch); 
extern  BYTE gtolower (BYTE ch); 
extern  BYTE ReverseChar (BYTE ch, MorphLanguageEnum langua);
extern char* RusMakeUpper (char *word);
extern char* EngMakeUpper (char *word);
extern string& EngMakeUpper (string& word);
extern string& EngMakeLower (string& word);
extern char* GerMakeUpper (char *word);
extern string& GerMakeUpper (string& word);
extern char* RusMakeLower (char *word);
extern string& EngRusMakeLower (string& word);
extern char* EngRusMakeLower (char* word);
extern char* RmlMakeUpper (char *word, MorphLanguageEnum langua);
extern string& RmlMakeUpper (string& word, MorphLanguageEnum langua);
extern string& RmlMakeLower (string& word, MorphLanguageEnum langua);

// конвертирует из строчной кириллицы в прописные 
extern string&  EngRusMakeUpper (string& word);
// конвертирует из строчной кириллицы в прописные 
extern char*  EngRusMakeUpper (char* word);


// check languaage 
extern bool IsRussian (const char *word); 
extern bool IsRussian (const string& word);
extern bool IsEnglish (const char *word);
extern bool IsEnglish (const string& word);
extern bool IsGerman (const char *word);
extern bool IsGerman (const string& word);
extern bool CheckLanguage (const char *word, MorphLanguageEnum langua);
extern bool CheckLanguage (const string& word, MorphLanguageEnum langua);



extern void ConvertJO2Je(string& src);
extern void ConvertJO2Je(char* src);
extern void ConvertJO2Je(char* src, size_t Length);
extern string ConvertASCIIToHtmlSymbols(const string& txt);


template <class T, class Pred, class Conv>
T& RegisterConverter (T& word, size_t Len, Pred P,  Conv C)
{
	for( size_t i = 0 ; i < Len; i++ )
        if ( P( (BYTE)word[i] )  )
			word[i] = C ( (BYTE)word[i] );

    return word;
}


template <class T>
T& GerEngRusMakeUpperTemplate (T& word, MorphLanguageEnum Langua, size_t Len )
{
	if (Len == 0) return word;

	if (Langua == morphGerman)
		return RegisterConverter(word, Len, is_german_lower, gtoupper);
	else
		for( size_t i = 0 ; i < Len; i++ )
			if (is_russian_lower((BYTE)word[i]))
					word[i] = rtoupper ( (BYTE)word[i] );
			else
			if (is_english_lower((BYTE)word[i]))
					word[i] = etoupper ( (BYTE)word[i] );

	return word;
};	





//  QWORD mask
#define _QM(X) (((QWORD)1)<<X)

enum RegisterEnum {AnyRegister=0, LowLow=1, UpLow=2, UpUp=3};


#endif


