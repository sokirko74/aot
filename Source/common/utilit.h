// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Alexey Sokirko
#pragma once
#include  <stdio.h>


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#ifdef WIN32
	#define NOMINMAX 
	#include "windows.h"
	#include "winuser.h"
#endif

#include  "single_byte_encoding.h"

#include  <assert.h>
#include  <stdarg.h>
#include  <ctype.h>
#include  <string>
#include  <algorithm>
#include  <stdexcept>
#include  <vector>
#include  <iostream>
#include <set>
#include  <stack>
#include  <map>
#include  <time.h>
#include  <string.h>
#include  <unordered_set>

#pragma warning (disable : 4018)
#pragma warning (disable : 4244)
#pragma warning (disable : 4103)
#pragma warning  (disable : 4530)
#pragma warning  (disable : 4251)
#pragma warning  (disable : 4996)
		
//using namespace std;

typedef unsigned char BYTE;
typedef uint64_t QWORD;

#ifdef WIN32
	#include  <io.h>
	#include <fcntl.h>
	
#else
	#include  <unistd.h>
	typedef unsigned int	DWORD;	
	typedef unsigned short	WORD;
	typedef unsigned int	UINT;
	const   unsigned int _MAX_PATH = 512;
#endif

typedef std::vector<std::string> StringVector;
typedef std::unordered_set<std::string> StringHashSet;
typedef std::set<std::string> StringSet;
typedef std::vector<DWORD> DwordVector;

inline QWORD GetMaxQWORD ()
{
	#ifdef WIN32
		return  0xffffffffffffffff;
	#else
		return  0xffffffffffffffffULL;
	#endif
};


// morphology&syntax
typedef enum {	morphUnknown = 0, morphRussian = 1, morphEnglish = 2, morphGerman = 3, morphGeneric = 4, morphURL=5, morphDigits=6} MorphLanguageEnum;

template <class T1, class T2, class T3>
struct	troika : public std::pair<T1, T2>
{
	T3 third;
};


typedef QWORD file_off_t;

class CExpc
{
 public:
	std::string	m_strCause;
	int		m_ErrorCode;

    CExpc(int ErrorCode, const std::string& Cause);
	CExpc(const std::string& Cause);
	CExpc(const CExpc& from);
    CExpc(const char* format, ... );
	CExpc& operator= (const CExpc& from);
};


// working with files
extern bool			FileExists (const char *FName);
extern bool         DirExists(const char *path);
extern file_off_t	FileSize (const char *filename);
extern bool			FSeek(FILE* fp, file_off_t pos, int origin);
extern file_off_t	FTell(FILE* fp);



extern void		ErrorMessage (const std::string& Titul, const std::string& Message);
extern void		ErrorMessage (const std::string& Message);
extern std::string	MakeFName ( const std::string& InpitFileName,  const std::string& Ext);
extern std::string   MakePath(const std::string path, const std::string fileName);
extern bool     MakePathAndCheck(const std::string path, const std::string fileName, std::string& fullPath);
extern std::string	GetPathByFile (std::string FileName);
extern bool		IsEmptyLine ( const char *t);
extern bool		IsHtmlFile (const std::string& FileName);
extern void		AddFile(const char* MainFile, const char* ToAdd);
extern bool     RmlMoveFile(const char *oldpath, const char *newpath);
extern bool     RmlCopyFile(const char *oldpath, const char *newpath);
extern std::string	CreateTempFileName();
extern bool		MakeDir(const std::string& txt);
extern bool     RemoveWithPrint (const std::string& FileName);

// working with registry 
extern std::string	GetRegistryString (std::string RegistryPath);
extern std::string	GetRegistryStringFromLocalIniFile (std::string RegistryPath);
extern bool		CanGetRegistryString (std::string RegistryPath);
extern void		SetRegistryString (std::string RegistryPath, std::string Value);
extern bool		IsRmlRegistered(std::string& Error);
extern bool		CheckEvaluationTime();
extern struct tm  RmlGetCurrentTime ();
extern bool		ReadTimeOutFromRegistry(bool bReadFromLocalFile, int& TimeOut);
extern std::string	GetIniFilePath();
extern std::string	GetRmlVariable();
extern std::string   BuildRMLPath (const char* s);

// working with std::strings
extern char*	rtrim (char* s);
extern bool		IsSuperEqualChar (BYTE ch1, BYTE ch2, MorphLanguageEnum langua);
extern bool		strscmp ( const char *s1, const char *s2, size_t l, MorphLanguageEnum langua);
extern int		CompareWithoutRegister ( const char *s1, const char *s2, size_t l, MorphLanguageEnum langua);
extern BYTE		force_rus_char (BYTE ch);
extern bool		force_to_rus (char* dest, const char* sour, size_t len);
extern char*	IntToStr (int Value, char* Buffer);
extern std::string&	IntToStr (int Value, std::string& oBuffer);
extern std::string	Format( const char* format, ... );
extern std::string&  TrimLeft (std::string& str);
extern std::string&  TrimRight (std::string& str);
extern std::string&	Trim (std::string& str);
extern void		rml_TRACE( const char* format, ... );
extern bool		LoadFileToString(std::string FileName, std::string& Result);
extern void		SqueezeSpacesAndDeleteEoln(std::string& s);
extern DWORD	StringCrc32(const char* szString);

const BYTE	UnknownPartOfSpeech		= 0xff;
const int	UnknownSyntaxElement = 0xffff;
typedef enum {DontKillHomonyms = 0, CoverageKillHomonyms = 1} KillHomonymsEnum;

typedef enum { LocThes = 0, FinThes, CompThes, OmniThes, NoneThes } EThesType;
typedef enum { EClause = 0, EWord, EGroup, ENoneType } EUnitType;


extern int GetPredictionPartOfSpeech(const std::string& PartOfSpeech, MorphLanguageEnum langua);
extern bool GetLanguageByString (std::string s, MorphLanguageEnum& Result);
extern std::string GetStringByLanguage (MorphLanguageEnum Langua);
// ============     Brackets =========================


extern int isbracket  (BYTE x);
extern size_t dual_bracket (BYTE x);



//==================    Digits   ===========================
extern bool is_upper_roman_digit (BYTE ch);
extern bool is_lower_roman_digit (BYTE ch);
extern bool is_roman_number ( const char *s, size_t len);
extern bool is_pseudo_graph(BYTE x);
//  =============  Punctuation Letters ======================

extern	bool is_spc_fill (BYTE x);
extern  bool is_english_upper(BYTE x);
extern  bool is_english_lower(BYTE x);
extern  bool is_german_upper(BYTE x);
extern  bool is_german_lower(BYTE x);
extern  bool is_russian_upper(BYTE x);     
extern  bool is_russian_lower(BYTE x);

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
extern std::string& EngMakeUpper (std::string& word);
extern std::string& EngMakeLower (std::string& word);
extern char* GerMakeUpper (char *word);
extern std::string& GerMakeUpper (std::string& word);
extern char* RusMakeLower (char *word);
extern std::string& EngRusMakeLower (std::string& word);
extern char* EngRusMakeLower (char* word);
extern char* RmlMakeUpper (char *word, MorphLanguageEnum langua);
extern std::string& RmlMakeUpper (std::string& word, MorphLanguageEnum langua);
extern std::string& RmlMakeLower (std::string& word, MorphLanguageEnum langua);

extern std::string&  EngRusMakeUpper (std::string& word);
extern char*  EngRusMakeUpper (char* word);
std::string convert_from_utf8(const char *utf8str, const MorphLanguageEnum langua);
std::string convert_to_utf8(const std::string& str, const MorphLanguageEnum langua);



// check languaage 
extern bool IsRussian (const char *word); 
extern bool IsRussian (const std::string& word);
extern bool IsEnglish (const char *word);
extern bool IsEnglish (const std::string& word);
extern bool IsGerman (const char *word);
extern bool IsGerman (const std::string& word);
extern bool CheckLanguage (const char *word, MorphLanguageEnum langua);
extern bool CheckLanguage (const std::string& word, MorphLanguageEnum langua);


extern bool HasJO(std::string src);
extern void ConvertJO2Je(std::string& src);
extern void ConvertJO2Je(char* src);
extern void ConvertJO2Je(char* src, size_t Length);
extern std::string ConvertASCIIToHtmlSymbols(const std::string& txt);


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
#define _QM(X) (((QWORD)1)<<(X))

typedef  DWORD poses_mask_t;

enum RegisterEnum {AnyRegister=0, LowLow=1, UpLow=2, UpUp=3};


extern QWORD pow(QWORD x,int y);

extern int CountBits(QWORD value);
extern size_t FindFloatingPoint(const char* str);

inline std::string _R(const char* buffer) {
	return convert_from_utf8(buffer, morphRussian);
}

inline std::string _E(const char* buffer) {
	return convert_from_utf8(buffer, morphEnglish);
}

inline std::string _G(const char* buffer) {
	return convert_from_utf8(buffer, morphGerman);
}

inline bool startswith(const std::string& main, const std::string& prefix) {
	return main.rfind(prefix, 0) == 0;
}

inline bool endswith(const std::string& main, const std::string& suffix) {
	if (main.length() >= suffix.length()) {
		return (0 == main.compare(main.length() - suffix.length(), suffix.length(), suffix));
	}
	else {
		return false;
	}
}
extern std::vector<std::string> list_path_by_file_mask(std::string filemask);
extern std::string join_string(const std::vector<std::string>& items, const std::string& delimiter);
void SetEnvVariable(std::string varname, std::string value);
