#include "utilit.h"
#include "util_classes.h"
#include "bserialize.h"
#include <time.h>
#include <errno.h>
#include <limits>
#include <codecvt>
#include <locale>
#include <filesystem>
#include <iostream>
#include <sstream>

//  for mkdir
#ifdef WIN32
	#include <direct.h>
	#define NOMINMAX 
	#include "windows.h"
#else
	#include <sys/stat.h>
#endif

const uint16_t fWordDelim = 1;
const uint16_t RusUpper = 2;
const uint16_t RusLower = 4;
const uint16_t GerUpper = 8;
const uint16_t GerLower = 16;
const uint16_t EngUpper = 32;
const uint16_t EngLower = 64;
const uint16_t OpnBrck = 128;
const uint16_t ClsBrck = 256;
const uint16_t UpRomDigits = 512;
const uint16_t LwRomDigits = 1024;
const uint16_t LatinVowel = 2048;
const uint16_t RussianVowel = 4096;
const uint16_t URL_CHAR = 8192;


const uint16_t ASCII[256] = {
/*null (nul)*/                                     fWordDelim,
/*start of heading (soh)*/                         fWordDelim,
/*start of text (stx)*/                            fWordDelim,
/*end of text (etx)*/                              fWordDelim,
/*end of transmission (eot)*/                      fWordDelim,
/*enquiry (enq)*/                                  fWordDelim,
/*acknowledge (ack)*/                              fWordDelim,
/*bell (bel)*/                                     fWordDelim,
/*backspace (bs)*/                                 fWordDelim,
/*character tabulation (ht)*/                      fWordDelim,
/*line feed (lf)*/                                 fWordDelim,
/*line tabulation (vt)*/                           fWordDelim,
/*form feed (ff)*/                                 fWordDelim,
/*carriage return (cr)*/                           fWordDelim,
/*shift out (so)*/                                 fWordDelim,
/*shift in (si)*/                                  fWordDelim,
/*datalink escape (dle)*/                          fWordDelim,
/*device control one (dc1)*/                       fWordDelim,
/*device control two (dc2)*/                       fWordDelim,
/*device control three (dc3)*/                     fWordDelim,
/*device control four (dc4)*/                      fWordDelim,
/*negative acknowledge (nak)*/                     fWordDelim,
/*syncronous idle (syn)*/                          fWordDelim,
/*end of transmission block (etb)*/                fWordDelim,
/*cancel (can)*/                                   fWordDelim,
/*end of medium (em)*/                             fWordDelim,
/*substitute (sub)*/                               fWordDelim,
/*escape (esc)*/                                   fWordDelim,
/*file separator (is4)*/                           fWordDelim,
/*group separator (is3)*/                          fWordDelim,
/*record separator (is2)*/                         fWordDelim,
/*unit separator (is1)*/                           fWordDelim,
/*space ' '*/                                      fWordDelim,
/*exclamation mark '!'*/                           fWordDelim|URL_CHAR,
/*quotation mark '"'*/                             fWordDelim,
/*number sign '#'*/                                fWordDelim,
/*dollar sign '$'*/                                fWordDelim|URL_CHAR,
/*percent sign '%'*/                               fWordDelim|URL_CHAR,
/*ampersand '&'*/                                  fWordDelim|URL_CHAR,
/*apostrophe '''*/                                 fWordDelim|RusUpper|RusLower, // твердый знак в виде апострофа
/*left parenthesis '('*/                           fWordDelim|OpnBrck|URL_CHAR,
/*right parenthesis ')'*/                          fWordDelim|ClsBrck|URL_CHAR,
/*asterisk '*'*/                                   fWordDelim|URL_CHAR,
/*plus sign '+'*/                                  fWordDelim|URL_CHAR,
/*comma ','*/                                      fWordDelim|URL_CHAR,
/*hyphen-minus '-'*/                               fWordDelim|URL_CHAR,
/*full stop '.'*/                                  fWordDelim|URL_CHAR,
/*solidus '/'*/                                    fWordDelim|URL_CHAR,
/*digit zero '0'*/                                 URL_CHAR,
/*digit one '1'*/                                  URL_CHAR,
/*digit two '2'*/                                  URL_CHAR,
/*digit three '3'*/                                URL_CHAR,
/*digit four '4'*/                                 URL_CHAR,
/*digit five '5'*/                                 URL_CHAR,
/*digit six '6'*/                                  URL_CHAR,
/*digit seven '7'*/                                URL_CHAR,
/*digit eight '8'*/                                URL_CHAR,
/*digit nine '9'*/                                 URL_CHAR,
/*colon ':'*/                                      fWordDelim|URL_CHAR,
/*semicolon ';'*/                                  fWordDelim|URL_CHAR,
/*less-than sign '<'*/                             fWordDelim|OpnBrck,
/*equals sign '='*/                                fWordDelim|URL_CHAR,
/*greater-than sign '>'*/                          fWordDelim|ClsBrck,
/*question mark '?'*/                              fWordDelim|URL_CHAR,
/*commercial at '@'*/                              fWordDelim|URL_CHAR,
/*latin capital letter a 'A'*/                     GerUpper|EngUpper|LatinVowel,
/*latin capital letter b 'B'*/                     GerUpper|EngUpper,
/*latin capital letter c 'C'*/                     GerUpper|EngUpper,
/*latin capital letter d 'D'*/                     GerUpper|EngUpper,
/*latin capital letter e 'E'*/                     GerUpper|EngUpper|LatinVowel,
/*latin capital letter f 'F'*/                     GerUpper|EngUpper,
/*latin capital letter g 'G'*/                     GerUpper|EngUpper,
/*latin capital letter h 'H'*/                     GerUpper|EngUpper,
/*latin capital letter i 'I'*/                     GerUpper|EngUpper|UpRomDigits|LatinVowel,
/*latin capital letter j 'J'*/                     GerUpper|EngUpper,
/*latin capital letter k 'K'*/                     GerUpper|EngUpper,
/*latin capital letter l 'L'*/                     GerUpper|EngUpper|UpRomDigits,
/*latin capital letter m 'M'*/                     GerUpper|EngUpper,
/*latin capital letter n 'N'*/                     GerUpper|EngUpper,
/*latin capital letter o 'O'*/                     GerUpper|EngUpper|LatinVowel,
/*latin capital letter p 'P'*/                     GerUpper|EngUpper,
/*latin capital letter q 'Q'*/                     GerUpper|EngUpper,
/*latin capital letter r 'R'*/                     GerUpper|EngUpper,
/*latin capital letter s 'S'*/                     GerUpper|EngUpper,
/*latin capital letter t 'T'*/                     GerUpper|EngUpper,
/*latin capital letter u 'U'*/                     GerUpper|EngUpper|LatinVowel,
/*latin capital letter v 'V'*/                     GerUpper|EngUpper|UpRomDigits,
/*latin capital letter w 'W'*/                     GerUpper|EngUpper,
/*latin capital letter x 'X'*/                     GerUpper|EngUpper|UpRomDigits,
/*latin capital letter y 'Y'*/                     GerUpper|EngUpper,
/*latin capital letter z 'Z'*/                     GerUpper|EngUpper,
/*left square bracket '['*/                        fWordDelim|OpnBrck,
/*reverse solidus '\'*/                            fWordDelim,
/*right square bracket ']'*/                       fWordDelim|ClsBrck,
/*circumflex accent '^'*/                          fWordDelim,
/*low line '_'*/                                   fWordDelim,
/*grave accent '`'*/                               fWordDelim,
/*latin small letter a 'a'*/                       GerLower|EngLower|LatinVowel|URL_CHAR,
/*latin small letter b 'b'*/                       GerLower|EngLower|URL_CHAR,
/*latin small letter c 'c'*/                       GerLower|EngLower|URL_CHAR,
/*latin small letter d 'd'*/                       GerLower|EngLower|URL_CHAR,
/*latin small letter e 'e'*/                       GerLower|EngLower|LatinVowel|URL_CHAR,
/*latin small letter f 'f'*/                       GerLower|EngLower|URL_CHAR,
/*latin small letter g 'g'*/                       GerLower|EngLower|URL_CHAR,
/*latin small letter h 'h'*/                       GerLower|EngLower|URL_CHAR,
/*latin small letter i 'i'*/                       GerLower|EngLower|LwRomDigits|LatinVowel|URL_CHAR,
/*latin small letter j 'j'*/                       GerLower|EngLower|URL_CHAR,
/*latin small letter k 'k'*/                       GerLower|EngLower|URL_CHAR,
/*latin small letter l 'l'*/                       GerLower|EngLower|LwRomDigits|URL_CHAR,
/*latin small letter m 'm'*/                       GerLower|EngLower|URL_CHAR,
/*latin small letter n 'n'*/                       GerLower|EngLower|URL_CHAR,
/*latin small letter o 'o'*/                       GerLower|EngLower|LatinVowel|URL_CHAR,
/*latin small letter p 'p'*/                       GerLower|EngLower|URL_CHAR,
/*latin small letter q 'q'*/                       GerLower|EngLower|URL_CHAR,
/*latin small letter r 'r'*/                       GerLower|EngLower|URL_CHAR,
/*latin small letter s 's'*/                       GerLower|EngLower|URL_CHAR,
/*latin small letter t 't'*/                       GerLower|EngLower|URL_CHAR,
/*latin small letter u 'u'*/                       GerLower|EngLower|LatinVowel|URL_CHAR,
/*latin small letter v 'v'*/                       GerLower|EngLower|LwRomDigits|URL_CHAR,
/*latin small letter w 'w'*/                       GerLower|EngLower|URL_CHAR,
/*latin small letter x 'x'*/                       GerLower|EngLower|LwRomDigits|URL_CHAR,
/*latin small letter y 'y'*/                       GerLower|EngLower|URL_CHAR,
/*latin small letter z 'z'*/                       GerLower|EngLower|URL_CHAR,
/*left curly bracket '{'*/                         fWordDelim|OpnBrck,
/*vertical line '|'*/                              fWordDelim,
/*right curly bracket '}'*/                        fWordDelim|ClsBrck,
/*tilde '~'*/                                      fWordDelim,
/*delete ''*/                                     0,
/*padding character (pad) '_'*/                    fWordDelim,
/*high octet preset (hop) '_'*/                    0,
/*break permitted here (bph) '''*/                 0,
/*no break here (nbh) '_'*/                        fWordDelim,
/*index (ind) '"'*/                                0,
/*next line (nel) ':'*/                            fWordDelim,
/*start of selected area (ssa) '+'*/               fWordDelim,
/*end of selected area (esa) '+'*/                 fWordDelim,
/*character tabulation std::set (hts) '_'*/             fWordDelim,
/*character tabulation with justification (htj) '%'*/ fWordDelim,
/*line tabulation std::set (vts) '_'*/                  0,
/*partial line forward (pld) '<'*/                 fWordDelim,
/*partial line backward (plu) '_'*/                fWordDelim,
/*reverse line feed (ri) '_'*/                     fWordDelim,
/*single-shift two (ss2) '_'*/                     fWordDelim,
/*single-shift three (ss3) '_'*/                   fWordDelim,
/*device control std::string (dcs) '_'*/                fWordDelim,
/*private use one (pu1) '''*/                      fWordDelim,
/*private use two (pu2) '''*/                      fWordDelim,
/*std::set transmit state (sts) '"'*/                   fWordDelim,
/*cancel character (cch) '"'*/                     fWordDelim,
/*message waiting (mw) ''*/                       fWordDelim,
/*start of guarded area (spa) '-'*/                fWordDelim,
/*end of guarded area (epa) '-'*/                  fWordDelim,
/*start of std::string (sos) '_'*/                      fWordDelim,
/*single graphic character introducer (sgci) 'T'*/ fWordDelim,
/*single character introducer (sci) '_'*/          fWordDelim,
/*control sequence introducer (csi) '>'*/          fWordDelim,
/*std::string terminator (st) '_'*/                     fWordDelim,
/*operating system command (osc) '_'*/             fWordDelim,
/*privacy message (pm) '_'*/                       fWordDelim,
/*application program command (apc) '_'*/          fWordDelim,
/*no-break space ' '*/                             fWordDelim,
/*inverted exclamation mark 'Ў'*/                  fWordDelim,
/*cent sign 'ў'*/                                  fWordDelim,
/*pound sign '_'*/                                 fWordDelim,
/*currency sign '¤'*/                              fWordDelim,
/*yen sign '_'*/                                   fWordDelim,
/*broken bar '¦'*/                                 fWordDelim,
/*section sign '§'*/                               fWordDelim,
/*diaeresis 'Ё'*/                                  fWordDelim|RusUpper|RussianVowel,
/*copyright sign 'c'*/                             fWordDelim,
/*feminine ordinal indicator 'Є'*/                 fWordDelim,
/*left pointing double angle quotation mark '<'*/  fWordDelim,
/*not sign '¬'*/                                   fWordDelim,
/*soft hyphen '-'*/                                fWordDelim,
/*registered sign 'R'*/                            fWordDelim,
/*macron 'Ї'*/                                     fWordDelim,
/*degree sign '°'*/                                fWordDelim,
/*plus-minus sign '+'*/                            fWordDelim,
/*superscript two '_'*/                            fWordDelim,
/*superscript three '_'*/                          fWordDelim,
/*acute '_'*/                                      fWordDelim,
/*micro sign 'ч'*/                                 fWordDelim|GerLower|GerUpper,
/*pilcrow sign '¶'*/                               fWordDelim,
/*middle dot '·'*/                                 fWordDelim,
/*cedilla 'ё'*/                                    RusLower|RussianVowel,
/*superscript one '№'*/                            fWordDelim,
/*masculine ordinal indicator 'є'*/                fWordDelim,
/*right pointing double angle quotation mark '>'*/ fWordDelim,
/*vulgar fraction one quarter '_'*/                fWordDelim,
/*vulgar fraction one half '_'*/                   fWordDelim,
/*vulgar fraction three quarters '_'*/             fWordDelim,
/*inverted question mark 'ї'*/                     fWordDelim,
/*latin capital letter a with grave 'А'*/          RusUpper|RussianVowel,
/*latin capital letter a with acute 'Б'*/          RusUpper,
/*latin capital letter a with circumflex 'В'*/     RusUpper|GerUpper|EngUpper|LatinVowel,
/*latin capital letter a with tilde 'Г'*/          RusUpper,
/*latin capital letter a with diaeresis 'Д'*/      RusUpper|GerUpper|LatinVowel,
/*latin capital letter a with ring above 'Е'*/     RusUpper|RussianVowel,
/*latin capital ligature ae 'Ж'*/                  RusUpper,
/*latin capital letter c with cedilla 'З'*/        RusUpper|GerUpper|EngUpper,
/*latin capital letter e with grave 'И'*/          RusUpper|GerUpper|EngUpper|LatinVowel|RussianVowel,
/*latin capital letter e with acute 'Й'*/          RusUpper|GerUpper|EngUpper|LatinVowel,
/*latin capital letter e with circumflex 'К'*/     RusUpper|GerUpper|EngUpper|LatinVowel,
/*latin capital letter e with diaeresis 'Л'*/      RusUpper,
/*latin capital letter i with grave 'М'*/          RusUpper,
/*latin capital letter i with acute 'Н'*/          RusUpper,
/*latin capital letter i with circumflex 'О'*/     RusUpper|RussianVowel,
/*latin capital letter i with diaeresis 'П'*/      RusUpper,
/*latin capital letter eth (icelandic) 'Р'*/       RusUpper,
/*latin capital letter n with tilde 'С'*/          RusUpper|GerUpper|EngUpper,
/*latin capital letter o with grave 'Т'*/          RusUpper,
/*latin capital letter o with acute 'У'*/          RusUpper|RussianVowel,
/*latin capital letter o with circumflex 'Ф'*/     RusUpper|GerUpper|EngUpper|LatinVowel,
/*latin capital letter o with tilde 'Х'*/          RusUpper,
/*latin capital letter o with diaeresis 'Ц'*/      RusUpper|GerUpper|EngUpper|LatinVowel,
/*multiplication sign 'Ч'*/                        RusUpper,
/*latin capital letter o with stroke 'Ш'*/         RusUpper|UpRomDigits,
/*latin capital letter u with grave 'Щ'*/          RusUpper,
/*latin capital letter u with acute 'Ъ'*/          RusUpper,
/*latin capital letter u with circumflex 'Ы'*/     RusUpper|GerUpper|EngUpper|LatinVowel|RussianVowel,
/*latin capital letter u with diaeresis 'Ь'*/      RusUpper|GerUpper|LatinVowel,
/*latin capital letter y with acute 'Э'*/          RusUpper|RussianVowel,
/*latin capital letter thorn (icelandic) 'Ю'*/     RusUpper|RussianVowel,
/*latin small letter sharp s (german) 'Я'*/        RusUpper|GerLower|GerUpper|RussianVowel,
/*latin small letter a with grave 'а'*/            RusLower|RussianVowel,
/*latin small letter a with acute 'б'*/            RusLower,
/*latin small letter a with circumflex 'в'*/       RusLower|GerLower|EngLower|LatinVowel,
/*latin small letter a with tilde 'г'*/            RusLower,
/*latin small letter a with diaeresis 'д'*/        RusLower|GerLower|LatinVowel,
/*latin small letter a with ring above 'е'*/       RusLower|RussianVowel,
/*latin small ligature ae 'ж'*/                    RusLower,
/*latin small letter c with cedilla 'з'*/          RusLower|GerLower|EngLower,
/*latin small letter e with grave 'и'*/            RusLower|GerLower|EngLower|LatinVowel|RussianVowel,
/*latin small letter e with acute 'й'*/            RusLower|GerLower|EngLower|LatinVowel,
/*latin small letter e with circumflex 'к'*/       RusLower|GerLower|EngLower|LatinVowel,
/*latin small letter e with diaeresis 'л'*/        RusLower,
/*latin small letter i with grave 'м'*/            RusLower,
/*latin small letter i with acute 'н'*/            RusLower,
/*latin small letter i with circumflex 'о'*/       RusLower|RussianVowel,
/*latin small letter i with diaeresis 'п'*/        RusLower,
/*latin small letter eth (icelandic) 'р'*/         RusLower,
/*latin small letter n with tilde 'с'*/            RusLower|GerLower|EngLower,
/*latin small letter o with grave 'т'*/            RusLower,
/*latin small letter o with acute 'у'*/            RusLower|RussianVowel,
/*latin small letter o with circumflex 'ф'*/       RusLower|GerLower|EngLower|LatinVowel,
/*latin small letter o with tilde 'х'*/            RusLower,
/*latin small letter o with diaeresis 'ц'*/        RusLower|GerLower|EngLower|LatinVowel,
/*division sign 'ч'*/                              RusLower,
/*latin small letter o with stroke 'ш'*/           RusLower,
/*latin small letter u with grave 'щ'*/            RusLower,
/*latin small letter u with acute 'ъ'*/            RusLower,
/*latin small letter u with circumflex 'ы'*/       RusLower|GerLower|EngLower|LatinVowel|RussianVowel,
/*latin small letter u with diaeresis 'ь'*/        RusLower|GerLower|LatinVowel,
/*latin small letter y with acute 'э'*/            RusLower|RussianVowel,
/*latin small letter thorn (icelandic) 'ю'*/       RusLower|RussianVowel,
/*latin small letter y with diaeresis  'я'*/       RusLower|RussianVowel
};


bool FileExists (const char *FName)
{
	return (access(FName, 0) == 0);
}

bool DirExists(const char *path) {
    struct stat info;

    if(stat( path, &info ) != 0)
        return false;
    else if(info.st_mode & S_IFDIR)
        return true;
    else
        return false;
}

file_off_t FileSize (const char *filename)
{
    std::ifstream ifile(filename);
    ifile.seekg(0, std::ios_base::end);//seek to end
    file_off_t size = ifile.tellg();
    ifile.close();
	return size;
}


void(*GlobalErrorMessage)(const std::string&) = 0;

void ErrorMessage (const std::string& Titul, const std::string& Message)
{
	if (GlobalErrorMessage)
	{
		std::string q = Titul + ":"+Message;
		if (q.empty()|| (q[q.length() -1]!='\n'))	q+='\n';
		GlobalErrorMessage(q);
		return;
	};
	
	#ifdef WIN32
		#ifndef _CONSOLE
			MessageBox(0, Message.c_str(), Titul.c_str(), MB_OK);
			return;
		#endif
	#endif

	std::string q = Message;
	if (q.empty()|| (q[q.length() -1]!='\n'))	q+='\n';
	fprintf(stderr, "%s: %s", Titul.c_str(), q.c_str());
	
};

void ErrorMessage (const std::string& Message)
{
	ErrorMessage("error", Message);
};

std::string MakeFName ( const std::string& InpitFileName,  const std::string& Ext)
{
	std::string Result = InpitFileName;
	size_t i = Result.find_last_of('.'); // найти последнее расширение
	if ( i  != std::string::npos) 
		Result.erase(i);	
	Result += std::string(".") ;
	Result += Ext;
	return Result;
}


std::string MakePath (const std::string path, const std::string fileName) { 
	std::string result = path;
	
	if (!result.empty()) {
#ifdef WIN32
		if ((result.back() != '\\')
			&& (result.back() != '/')
			)
			result += "\\";
#else

		if (result.back() != '/')
			result += "/";

#endif
	}

	return result + fileName;	  
};

bool MakePathAndCheck(const std::string path, const std::string fileName, std::string& fullPath) {
	fullPath = MakePath(path, fileName);
	return FileExists(fullPath.c_str());
}

std::string GetPathByFile (std::string FileName)
{ 
  size_t i = FileName.rfind("\\");	  
  size_t j = FileName.rfind("/");	  

  if (			(i == std::string::npos) 
		&&		(j == std::string::npos)
	  )
	  return "";
  else
	  if (i == std::string::npos) 
		  return FileName.substr(0, j+1);
	  else
		if (j == std::string::npos) 
			return FileName.substr(0, i+1);
		else
			return FileName.substr(0, std::max(i,j)+1); 
};


std::string	CreateTempFileName()
{
	char tmpfilename[_MAX_PATH];
    const char * sTempPath = getenv("TMP");
	#ifdef WIN32
        if (sTempPath)
		{
		    strcpy (tmpfilename, _tempnam(sTempPath, "rml"));
        }
        else
        {
            strcpy (tmpfilename, _tempnam(".", "rml"));
            //tmpnam((tmpfilename);
        }
	#else
		if (sTempPath)
		{
			strcpy(tmpfilename, sTempPath);
			if (strlen(tmpfilename) > 0 && tmpfilename[strlen(tmpfilename)-1] != '/')
				strcat (tmpfilename,"/");
			strcat (tmpfilename,"rml-temp.XXXXXX");
		}
		else
            strcpy(tmpfilename, "/tmp/rml-temp.XXXXXX");	
		
		int fd = mkstemp(tmpfilename);
		if (fd == -1)
			throw CExpc ("Cannot create temp file");
		close (fd);
	#endif

	return tmpfilename;

};

#ifdef _DEBUG
    static std::ofstream logger;
#endif

void rml_TRACE( const char* format, ... )
{
#ifdef _DEBUG

        if (!format) {
            return;
        }
        va_list arglst;
        char buffer[2000];
        if (strlen (format) >  200)
        {
            #ifdef WIN32
                OutputDebugString("!!!!! too long debug line!!!");
            #endif
            return;
        };
        va_start( arglst, format );
        vsprintf(buffer, format, arglst);
        va_end( arglst );

        #ifdef WIN32
            OutputDebugString(buffer);
        #endif

		if (!logger.is_open()) {
			logger.open("trace.log", std::ios::binary);
		}
		logger << convert_to_utf8(buffer, morphRussian)	;
		logger.flush();
#endif
};


bool IsEmptyLine ( const char *t)
{
	if (*t==0) return true;

	while (*t != '\r')
	{
		if ( (BYTE) t[0]>' ') 
			return false;

		t++;
	}

	return true;
}

char* rtrim (char* s)
{
	size_t len = strlen(s);

	while ( len > 0 && isspace((BYTE)s[len-1]))
		s[--len] = 0;

	return s;

}

BYTE convert_latin_char_similar_russian_lower_char(BYTE ch) {
    switch  (ch) {
        case 'a' : return RussianLowerA;
        case 'A' : return RussianLowerA;
        case 'e' : return RussianLowerE;
        case 'E' : return RussianLowerE;
        case 'k' : return RussianLowerK;
        case 'K' : return RussianLowerK;
        case 'M' : return RussianLowerM;
        //case 'm' : return RussianLowerM; //latin lower m  looks different
        case 'H' : return RussianLowerN;
        case 'o' : return RussianLowerO;
        case 'O' : return RussianLowerO;
        case '0' : return RussianLowerO;
        case 'p' : return RussianLowerR;
        case 'P' : return RussianLowerR;
        case 'C' : return RussianLowerS;
        case 'c' : return RussianLowerS;
        case 'y' : return RussianLowerU;
        case 'Y' : return RussianLowerU;
        case 'X' : return RussianLowerX;
        case 'x' : return RussianLowerX;
        default: return ch;
    }
}


// ================== IsSuperEqualChar проверяет, что символы равны с точностью до регистра
//  и с точностью до "языка", m.е. латинское "a" будет равно  русскому "а".
bool IsSuperEqualChar (BYTE ch1, BYTE ch2, MorphLanguageEnum langua)
{
	if	(		(ch1 == ch2) 
			||	(ch1 == ReverseChar(ch2, langua))
		) 
	return true;

	ch1 = convert_latin_char_similar_russian_lower_char(ch1);
    ch2 = convert_latin_char_similar_russian_lower_char(ch2);

	if (is_russian_upper(ch1))
		ch1 = rtolower(ch1);

	if (is_russian_upper(ch2))
		ch2 = rtolower(ch2);

	if (is_english_upper (ch1))
		ch1 = tolower (ch1);

	if (is_english_upper (ch2))
		ch2 = tolower (ch2);

	return ch1 == ch2;
}




/* сравнивает строки с помощью предиката IsSuperEqualChar */
// возращает true, если строки одинаковы
bool strscmp ( const char *s1, const char *s2, size_t l, MorphLanguageEnum langua)
{
	for	(	;
			(		(l > 0)
				&&	IsSuperEqualChar((BYTE)*s1,(BYTE)*s2, langua) 
				&&	*s1 
				&&	*s2  
			);
			s1++,s2++,l--
		);
	//если обе строки закончились
	return (!*s1 && !*s2);
}

// == strincmp
int CompareWithoutRegister ( const char *s1, const char *s2, size_t l, MorphLanguageEnum langua)
{
	for	(	;
			(		(l > 0)
				&&	(	( (BYTE)*s1 == (BYTE)*s2)
				     ||	( (BYTE)*s1 == ReverseChar((BYTE)*s2, langua) )
					)
				&&	*s1 
				&&	*s2  
			);
			s1++,s2++,l--
		);

	if (l == 0) return 0;

	if (*s1 < *s2)
		return -1;
	else
		if (*s1 > *s2)
			return 1;
		else
			return 0;
}





// в первой строке находятся латинские буквы, который могут быть заменены на
// русские
const static std::string latin_chars = "AaEe3KkMHOoPpCcyXxuT";

// во второй строке находятся русские
const static std::string russian_chars = _R("АаЕеЗКкМНОоРрСсуХхиТ");


BYTE force_rus_char (BYTE ch)
{

#ifdef _DEBUG
	assert(latin_chars.length() == russian_chars.length());
#endif

	size_t i  = latin_chars.find(ch);

	if (i == std::string::npos) return 0;

	return russian_chars[i];
}



bool force_to_rus (char* dest, const char* sour, size_t len)
{
	BYTE ch = 0 ;

	for (size_t i=0; i < len; i++)
		if  (is_russian_alpha((BYTE)sour[i]) || sour[i] == '-')
			dest[i] = sour[i];
		else
			if ((ch = force_rus_char((BYTE)sour[i])) != 0)
				dest[i] = ch;
			else
				return false;
	return true;
}

char* IntToStr (int Value, char* Buffer)
{
	sprintf (Buffer, "%i", Value);
	return Buffer;
};

std::string& IntToStr (int Value, std::string& oBuffer)
{
	char Buffer[256];
	sprintf (Buffer, "%i", Value);
	oBuffer = Buffer;
	return oBuffer;
};

bool IsHtmlFile (const std::string& FileName)
{
	size_t len = FileName.length();
	std::string R = FileName;
	EngMakeLower(R);

	return (   (len > 4)
			&& (   !strcmp (FileName.c_str()+len-3, "htm")
			    || !strcmp (FileName.c_str()+len-4, "html")
				|| !strcmp (FileName.c_str()+len-5, "shtml")
			   )
		   );
}


void AddFile(const char* MainFile, const char* ToAdd)
{
	char s[_MAX_PATH*2];
	#ifdef WIN32
		sprintf (s, "type %s  >> %s ", ToAdd, MainFile);
	#else
		sprintf (s, "cat %s  >> %s ", ToAdd, MainFile);
	#endif
	system (s);

};

std::string GetRmlVariable()
{
	const char* rml = getenv("RML");
	std::string s;
	if (rml)
		s = rml;
	else
	{
		throw CExpc("Error! Environment variable \"RML\"is not std::set!");
	}
	 
	Trim(s);
	for (size_t  i=0; i<s.length(); i++)
		if (s[i] == '\\')
			s[i] = '/';
	if (!s.empty() &&  (s[0] == '\"'))
		s.erase(0, 1);
	if (!s.empty() && (s[s.length() - 1] == '\"'))
		s.erase(s.length() - 1);
	if (!s.empty() &&  (s[s.length() - 1] == '/'))
		s.erase(s.length() - 1);
	if (s.empty())
		throw CExpc ("Environment variable \"RML\" is not properly initialized!");
	return s;
}


void SetEnvVariable(std::string varname, std::string value)
{
	#ifdef WIN32
		_putenv(Format("%s=%s", varname.c_str(), value.c_str()).c_str());
	#else
		char buffer[1024];
		auto s = Format("%s=%s", varname.c_str(), value.c_str());
		strcpy(buffer, s.c_str());
		putenv(buffer);
	#endif
	assert(getenv(varname.c_str()) == value);
}

const char RML_REGISTRY_FILENAME[] = "rml.ini";


static std::string GetStringInnerFromTheFile (std::string RegistryPath, std::string MainPath, std::string RmlPath) 
{
	std::string FileName = 	MainPath +  "/" + RML_REGISTRY_FILENAME;
	if (access  (FileName.c_str(),04) != 0)
	{
		//fprintf (stderr, "Cannot read %s\n", FileName.c_str());
		throw CExpc ("Cannot read "+FileName);		
	};
	FILE* fp;
	try 
	{
		fp = fopen (FileName.c_str(), "r");
		if (fp == 0)
			throw CExpc ("Cannot open "+FileName);
	}
	catch (...)
	{
		throw CExpc ("Cannot open "+FileName);
	}
	char buffer[2048];
	while (fgets(buffer,2048,fp))
	{
		std::string s = buffer;
		Trim(s);
		if (s.empty()) continue;
		size_t end_field_name = strcspn(s.c_str(), " \t");
		if (end_field_name == s.length())
		{
			fclose(fp);
			throw CExpc ("Cannot parse line "+s);
		};
		std::string Key = s.substr(0,end_field_name);
		if (RegistryPath == Key)
		{
			std::string Value = s.substr(end_field_name);
			Trim(Value);
			if (Value.substr(0, 4) == "$RML")
				Value.replace(0,4, RmlPath);

			fclose(fp);
			return Value;
		};
	};
	fclose (fp);

	return "";
};

std::string GetIniFilePath()
{
	return GetRmlVariable()+"/Bin";
};

std::string GetRegistryString (std::string RegistryPath)
{
	std::string Result = GetStringInnerFromTheFile(RegistryPath, GetIniFilePath(), GetRmlVariable().c_str());
	return Result;
	
};

std::string GetRegistryStringFromLocalIniFile (std::string RegistryPath)
{
	std::string  Result = GetStringInnerFromTheFile(RegistryPath, ".", "$RML");
	return Result;
};

bool CanGetRegistryString (std::string RegistryPath)  
{
	try {
		std::string s = GetRegistryString(RegistryPath);
		return  s != "";
	}
	catch (...)
	{
		return false;
	};
}


void SetRegistryString (std::string RegistryPath, std::string Value)
{  
	if (RegistryPath.find(' ') != -1) 
		throw CExpc ("A registry path cannot contain spaces");

	std::string FileName = GetIniFilePath() + "/" + RML_REGISTRY_FILENAME;

	FILE* fp = fopen (FileName.c_str(), "r");
	if (fp == 0)
		throw CExpc ("Cannot open "+FileName);
	std::string TempFileName = CreateTempFileName();
	FILE* outfp = fopen (TempFileName.c_str(), "w");
	if (!outfp) 
		throw CExpc ("Cannot open temporary file for setting variables in Registry");

	char buffer[2048];

	while (fgets(buffer,2048,fp))
	{
		std::string s = buffer;
		Trim(s);
		if (s.empty()) continue;
		size_t end_field_name = strcspn(s.c_str(), " \t");
		if (end_field_name == s.length()) 
		{
			fclose(fp);
			remove (TempFileName.c_str());
			throw CExpc ("Cannot parse line "+s);
		};

		if (RegistryPath != s.substr(0,end_field_name))
			fprintf (outfp, "%s\n", s.c_str());
	};

	fprintf (outfp, "%s %s\n", RegistryPath.c_str(), Value.c_str());

	
	fclose(fp);
	fclose(outfp);

	remove (FileName.c_str());

    if (!RmlMoveFile(TempFileName.c_str(), FileName.c_str()))
		throw CExpc ("Cannot move temporary file for setting variables to main file");


};


bool	IsRmlRegistered(std::string& Error)
{
	try 
	{
		std::string IniFile = GetIniFilePath() +  "/" + RML_REGISTRY_FILENAME;
	
		if (!FileExists(IniFile.c_str() ))
		{
			Error = "ini file not found: " + IniFile;
			return false;
		};
		if (access  (IniFile.c_str(),04) != 0)
		{
			Error = "ini file exists but cannot be read: " + IniFile;
			return false;
		};

	}
	catch (CExpc c)
	{
			Error = c.m_strCause;
			return false;
	}
	catch (...)
	{
		return false;
	}
	return true;
};

struct tm  RmlGetCurrentTime ()
{
	time_t ltime;
	time( &ltime );
	#ifdef	WIN32
		struct tm *today;
		today = localtime( &ltime );
		return *today;
	#else
		//  we should  thread safe  variants
		struct tm today;
		localtime_r( &ltime, &today );
		return today;
	#endif
};



//=======================           Morphology  =======================

const int predict_poses_size = 4;

const static std::string  predict_rus_pos[predict_poses_size] =
{
	_R("С"),
	_R("ИНФИНИТИВ"),
	_R("П"),
	_R("Н"),
};

const static std::string predict_eng_pos[predict_poses_size] =
{
	"NOUN",
	"VERB",
	"ADJECTIVE",
	"ADVERB",
};
const static std::string predict_ger_pos[predict_poses_size] =
{
	"SUB",
	"VER",
	"ADJ",
	"ADV",
};


int GetPredictionPartOfSpeech(const std::string& PartOfSpeech, MorphLanguageEnum langua)
{
	const std::string* PossiblePoses;
	switch (langua)
	{
		case morphEnglish: PossiblePoses = predict_eng_pos; break;
		case morphGerman: PossiblePoses = predict_ger_pos; break;
		case morphRussian: PossiblePoses = predict_rus_pos; break;
		default : assert(false);
	};
	for (size_t k = 0; k < predict_poses_size; k++) {
		if (PartOfSpeech == PossiblePoses[k])
		{
			return k;
		};
	}
	return UnknownPartOfSpeech;
};


bool GetLanguageByString (std::string s, MorphLanguageEnum& Result)
{
	
	EngRusMakeUpper(s);

	if(s == "RUSSIAN"){
		Result = morphRussian;
		return true;
	}else if(s == "ENGLISH"){
		Result = morphEnglish;
		return true;
	}
	else if(s == "GERMAN"){
		Result = morphGerman;
		return true;
	}
	else if(s == "GENERIC"){
		Result = morphGeneric;
		return true;
	}
	else if(s == "URL_ABC"){
		Result = morphURL;
		return true;
	}else 
		return false;
	
};

std::string GetStringByLanguage (MorphLanguageEnum Langua)
{
	switch (Langua) {
		case morphRussian: return "Russian";
		case morphEnglish: return "English";
		case morphGerman: return  "German";
		case morphGeneric: return  "Generic";
		case morphURL: return  "URL_ABC";
		default :return "unk";
	}
};

//=================================================================
//===================  STRINGS =====================================
//=================================================================


bool is_upper_roman_digit (BYTE ch)
{ 
	return	(ASCII[ch]&UpRomDigits)>0; 
}

bool is_lower_roman_digit (BYTE ch)
{ 
	return (ASCII[ch]&LwRomDigits) > 0; 
}


bool is_roman_number ( const char *s, size_t len)
{
	if (len == 0) return false;
	if (!s) return false;
	bool bLowRoman = true;
	bool bUpperRoman = true;
	for(size_t i=0; i<len; i++)
	{
		if ( (i>0) && (s[i] == '-') && (i+3>=len)) //  окончания пишутся после дефиса, типа "Павла I-го"
		{
			return bLowRoman || bUpperRoman;
		};

		bLowRoman = bLowRoman && ((ASCII[(BYTE)s[i]]&LwRomDigits)>0);
		bUpperRoman = bUpperRoman && ((ASCII[(BYTE)s[i]]&UpRomDigits)>0);
		
	};
	return    bLowRoman || bUpperRoman;
}

//  =============  Punctuation Letters ======================
bool is_spc_fill (BYTE x)
{
	return ((x)==' '||(x)=='_'); 
};

//====================================================
//============= ENGLISH                   ============
//====================================================


bool is_generic_upper(BYTE x)     
{
	// why ,,
 	return		(ASCII[x]&EngUpper)>0;
}; 
 
bool is_generic_lower(BYTE x)     
{
	return		(ASCII[x]&EngLower)>0 || x>=128;
};

bool is_generic_alpha(BYTE x)      
{
	return is_english_alpha(x) || x>=128; 
};

bool is_URL_alpha(BYTE x)      
{
	return (ASCII[x]&URL_CHAR)>0; 
};


bool is_english_upper(BYTE x)     
{
	return		(ASCII[x]&EngUpper)>0;
}; 

bool is_english_upper_vowel(BYTE x)
{
	return			((ASCII[x] & EngUpper)>0)
				&&	((ASCII[x] & LatinVowel)>0);
};


bool is_english_lower(BYTE x)     
{
	return		(ASCII[x]&EngLower)>0;
}; 

bool is_english_lower_vowel(BYTE x)     
{
	return			( (ASCII[x] & EngLower)>0)
				&&	( (ASCII[x] & LatinVowel)>0);
};

bool is_english_alpha(BYTE x)      
{
	return is_english_lower(x) || is_english_upper(x); 
};

BYTE etoupper (BYTE ch)
{
	if (ch>=(BYTE) 'a' && ch<= (BYTE) 'z')
		return ch -= 'a'-'A';
	else
	switch (ch) {
		case ccedille : return Ccedille; // C with cedille
		case egrave : return Egravis;  // E with gravis
		case ezirkun : return Ezirkun;  // E with zirkun
		case azirkun : return Azirkun;  // A with zirkun
		case ozirkun : return Ozirkun;  // O with zirkun
		case ouml : return Ouml;  // O umlaut
		case ntilda : return Ntilda;  // N with tilda
		case uzirkun : return Uzirkun;  // U with zirkun
		case eacute : return Eakut;  // E acut
		default : return ch;
	};
};


BYTE etolower (BYTE ch)
{
	if (ch>=(BYTE) 'A' && ch<= (BYTE) 'Z')
		return ch += 'a'-'A';
	else
	switch (ch) {
		case Ccedille : return ccedille; // C with cedille
		case Egravis : return egrave;  // E with gravis
		case Ezirkun : return ezirkun;  // E with zirkun
		case Azirkun : return azirkun;  // A with zirkun
		case Ozirkun : return ozirkun;  // O with zirkun
		case Ouml : return ouml;  // O umlaut
		case Ntilda : return ntilda;  // N with tilda
		case Uzirkun : return uzirkun;  // U with zirkun
		case Eakut : return eacute;  // E acut
		default : return ch;
	};
};


//====================================================
//============= GERMAN                  ============
//====================================================
 
bool is_german_upper(BYTE x)     
{
	return		(ASCII[x]&GerUpper) > 0;
}; 

bool is_german_lower(BYTE x)     
{
	return		(ASCII[x]&GerLower) > 0;
}; 

bool is_german_lower_vowel(BYTE x)     
{
	return			(ASCII[x] & GerLower)
				&&	(ASCII[x] & LatinVowel);
};

bool is_german_upper_vowel(BYTE x)
{
	return			(ASCII[x] & GerUpper)
				&&	(ASCII[x] & LatinVowel);

};

bool is_german_alpha(BYTE x)      
{
	return is_german_lower(x) || is_german_upper(x); 
};

BYTE gtoupper (BYTE ch) 
{ 
	if (is_english_lower (ch)) 
		return etoupper (ch);
	else
		switch (ch )
		 {
			case auml  : return Auml;;
			case uuml  : return Uuml;
			case ouml : return Ouml;
			default : return ch;
		 };
};

BYTE gtolower (BYTE ch)
{ 
	if (is_english_upper (ch)) 
		return etolower (ch);
	else
		switch (ch )
		 {
			case Auml : return auml;;
			case Uuml  : return uuml;
			case Ouml  : return ouml;
			default : return ch;
		 };
};

//====================================================
//============= RUSSIAN                  ============
//====================================================

bool is_russian_upper(BYTE x)     
{
	return		(ASCII[x]&RusUpper) > 0;
};

bool is_russian_lower(BYTE x)     
{
	return		(ASCII[x]&RusLower) > 0;
};

bool is_russian_lower_vowel(BYTE x)     
{
	return			( (ASCII[x] & RusLower) > 0)
				&&	( (ASCII[x] & RussianVowel) > 0);

};
bool is_russian_upper_vowel(BYTE x)
{
	return			((ASCII[x] & RusUpper) > 0)
				&&	((ASCII[x] & RussianVowel) > 0);
};

bool is_russian_lower_consonant(BYTE x) 
{
	return !is_russian_lower_vowel(x) &&  is_russian_lower(x);
}
bool is_russian_upper_consonant(BYTE x) 
{
	return !is_russian_upper_vowel(x) &&  is_russian_upper(x);
}
bool is_russian_consonant(BYTE x)  
{
	return is_russian_lower_consonant(x) || is_russian_upper_consonant(x);
}


bool is_russian_alpha(BYTE x)      
{
	return is_russian_lower(x) || is_russian_upper(x); 
};

BYTE rtoupper (BYTE ch) 
{ 
	if (ch == LowerJO) return UpperJO;
	if (ch == Apostrophe ) return Apostrophe;

	if ( is_russian_lower(ch) )
		return  ch - (RussianLowerA - RussianUpperA);
	else
		return ch;
};

BYTE rtolower (BYTE ch) 
{ 
	if (ch == UpperJO) return LowerJO;
	if (ch == Apostrophe ) return Apostrophe;

	if ( is_russian_upper(ch) )
		return ch + RussianLowerA - RussianUpperA;
	else
		return ch;
};


//========================================= ===========
//============= General encoding functions ============
//====================================================

bool is_lower_vowel(BYTE x, MorphLanguageEnum Langua)
{
	switch (Langua)
	{
		case morphRussian : return is_russian_lower_vowel(x);
		case morphEnglish : return is_english_lower_vowel(x);
		case morphGerman : return is_german_lower_vowel(x);
	};
	assert (false);
	return false;
};

bool is_upper_vowel(BYTE x, MorphLanguageEnum Langua)
{
	switch (Langua)
	{
		case morphRussian : return is_russian_upper_vowel(x);
		case morphEnglish : return is_english_upper_vowel(x);
		case morphGerman : return is_german_upper_vowel(x);
	};
	assert (false);
	return false;
};

bool is_upper_consonant(BYTE x, MorphLanguageEnum Langua)
{
	if (!is_upper_alpha(x, Langua)) return false;
	return !is_upper_vowel(x,Langua);
};

bool is_alpha (BYTE x)       
{
	return is_russian_alpha(x) || is_german_alpha(x); 
};


bool is_alpha (BYTE x, MorphLanguageEnum Langua)
{
	switch (Langua)
	{
		case morphRussian : return is_russian_alpha(x);
		case morphEnglish : return is_english_alpha(x);
		case morphGerman : return is_german_alpha(x);
		case morphGeneric : return is_generic_alpha(x);
		case morphURL : return is_URL_alpha(x);
	};
	assert (false);
	return false;
};

bool is_lower_alpha(BYTE x, MorphLanguageEnum Langua)
{
	switch (Langua)
	{
		case morphRussian : return is_russian_lower(x);
		case morphEnglish : return is_english_lower(x);
		case morphGerman : return is_german_lower(x);
		case morphGeneric : return is_generic_lower(x);
		case morphURL : return false;
	};
	assert (false);
	return false;
};

bool is_upper_alpha(BYTE x, MorphLanguageEnum Langua)
{
	switch (Langua)
	{
		case morphRussian : return is_russian_upper(x);
		case morphEnglish : return is_english_upper(x);
		case morphGerman : return is_german_upper(x);
		case morphGeneric : return is_generic_upper(x);
		case morphURL : return false;
	};
	assert (false);
	return false;
};

bool isnspace(BYTE x)    
{
	return x==' ' || x=='\t';
};



BYTE ReverseChar (BYTE ch, MorphLanguageEnum langua)
{
   if  (langua == morphGerman)
   {
	   if (is_english_lower (ch))
		   return etoupper (ch);
	   else
		 if (is_english_upper (ch))
			return etolower (ch);
		 else switch (ch )
		 {
			case Auml  : return auml;
			case auml  : return Auml;;
			case Uuml  : return uuml;
			case uuml  : return Uuml;
			case Ouml  : return ouml;
			case ouml  : return Ouml;
			default : return ch;
		 };
   }
   else if (langua == morphGeneric)
   {
	   if (is_english_lower (ch))
		   return etoupper (ch);
	   else
		 if (is_english_upper (ch))
			return etolower (ch);

    }
   else
   if (is_russian_lower(ch))
	   return rtoupper (ch);
   else
     if (is_russian_upper(ch))
		 return rtolower(ch);
     else
       if (is_english_lower (ch))
			return etoupper (ch);
       else
			if (is_english_upper (ch))
				return etolower (ch);

	return ch;
};


// конвертирует из прописной кириллицы в заглавную 
char* RusMakeUpper (char *word)
{
	return RegisterConverter(word, strlen(word), is_russian_lower, rtoupper);
}
// конвертирует из прописной кириллицы в заглавную 
char* EngMakeUpper (char *word)
{
	return RegisterConverter(word, strlen(word), is_english_lower, etoupper);
}

std::string& EngMakeUpper (std::string& word)
{
	return RegisterConverter(word, word.length(), is_english_lower, etoupper);
}

std::string& EngMakeLower (std::string& word)
{
	return RegisterConverter(word, word.length(), is_english_upper, etolower);
}


char* GerMakeUpper (char *word)
{
	return RegisterConverter(word, strlen(word), is_german_lower, gtoupper);
}

std::string& GerMakeUpper (std::string& word)
{
	return RegisterConverter(word, word.length(), is_german_lower, gtoupper);
}

// конвертирует из заглавной кириллицы в прописную 
char* RusMakeLower (char *word)
{
	return RegisterConverter(word, strlen(word), is_russian_upper, rtolower);
}

bool is_arab_digit (BYTE c)
{
	return isdigit(c)!= 0;
}


typedef bool (*SymbolCheckerFunc) (BYTE);

template<class T>
bool CheckLanguage(const T& src, size_t Length, MorphLanguageEnum langua)
{
	SymbolCheckerFunc SymbolChecker = 0;

	switch (langua) {
		case morphRussian : SymbolChecker =  is_russian_alpha; break;
		case morphGerman : SymbolChecker =  is_german_alpha; break;
		case morphEnglish : SymbolChecker =  is_english_alpha; break;
		case morphDigits : SymbolChecker =  is_arab_digit; break;
	};
	if (!SymbolChecker) return false;

	for( size_t i = 0 ; i < Length; i++ )
        if ( !SymbolChecker( (BYTE)src[i] )  )
			if ( (BYTE)src[i] != '-') 
				return false;
	return true;
};

bool IsRussian (const char *word) 
{
	return CheckLanguage(word, strlen(word), morphRussian);
}
bool IsRussian (const std::string& word)
{
	return CheckLanguage(word, word.length(), morphRussian);
}

bool IsEnglish (const char *word)
{
	return CheckLanguage(word, strlen(word), morphEnglish);
}

bool IsEnglish (const std::string& word)
{
	return CheckLanguage(word, word.length(), morphEnglish);
}

bool IsGerman (const char *word)
{
	return CheckLanguage(word, strlen(word), morphGerman);
}
bool IsGerman (const std::string& word)
{
	return CheckLanguage(word, word.length(), morphGerman);
}

bool CheckLanguage (const char *word, MorphLanguageEnum langua)
{
	return CheckLanguage(word, strlen(word), langua);
};

bool CheckLanguage (const std::string& word, MorphLanguageEnum langua)
{
	return CheckLanguage(word, word.length(), langua);
};




// конвертирует из прописные  кириллицы в строчную
std::string& EngRusMakeLower (std::string& word)
{
	size_t len = word.length();

	for( size_t i = 0 ; i < len; i++ )
        if ( is_russian_upper((BYTE)word[i]) )
            word[i] = rtolower ((BYTE)word[i]);
		else
			word[i] = etolower ((BYTE) word[i]);

	return word;
}


// конвертирует из прописные  кириллицы в строчную
char* EngRusMakeLower (char* word)
{
	if (word == 0) return 0;
	size_t len =  strlen(word);
	for( size_t i = 0 ; i < len; i++ )
        if ( is_russian_upper((BYTE)word[i]) )
			word[i] = rtolower ((BYTE)word[i]);
		else
			word[i] = etolower ((BYTE) word[i]);

	return word;
}



char* RmlMakeUpper (char *word, MorphLanguageEnum langua)
{
	if (!word) return word;
	return GerEngRusMakeUpperTemplate(word, langua, strlen(word));
};	

std::string& RmlMakeUpper (std::string& word, MorphLanguageEnum langua)
{
	GerEngRusMakeUpperTemplate(word, langua, word.length());
	return word;
};	

std::string& RmlMakeLower (std::string& word, MorphLanguageEnum langua)
{
	if (word.length() == 0) return word;

	if (langua == morphGerman)
		return RegisterConverter(word, word.length(), is_german_upper, gtolower);
	else
		return EngRusMakeLower(word);
};	


// конвертирует из строчной кириллицы в прописные 
std::string&  EngRusMakeUpper (std::string& word)
{
	size_t len = word.length();
	for( size_t i = 0 ; i < len; i++ )
        if ( is_russian_lower((BYTE)word[i]) )
			word[i] = rtoupper ((BYTE)word[i]);
		else
			word[i] = etoupper ((BYTE) word[i]);

	return word;
}



// конвертирует из строчной кириллицы в прописные 
char*  EngRusMakeUpper (char* word)
{
	if (word == 0) return 0;
	size_t len =  strlen(word);

	for( size_t i = 0 ; i < len; i++ )

        if ( is_russian_lower((BYTE)word[i]) )
			word[i] = rtoupper ((BYTE)word[i]);
		else
         if ( is_english_lower((BYTE)word[i]) )
			word[i] = etoupper ((BYTE) word[i]);
		

	return word;
}


template<class T>
void ConvertJO2JeTemplate(T& src, size_t Length)
{
	for (size_t i = 0; i < Length; i++)
	{
		if ( ((BYTE)src[i]) == LowerJO)
			src[i] = RussianLowerE;
		else
			if ( ( (BYTE)src[i]) == UpperJO)
			    src[i] = RussianUpperE;
	}

};

void ConvertJO2Je(char* src, size_t Length)
{
	ConvertJO2JeTemplate(src, Length);
};

bool HasJO(std::string src) 
{
	for (size_t i = 0; i < src.length(); i++)
	{
		if ( ((BYTE)src[i]) == LowerJO )
			return true;
		else
			if ( ( (BYTE)src[i]) == UpperJO)
			 return true;
	}
	return false;
}

void ConvertJO2Je(std::string& src) 
{

	ConvertJO2JeTemplate(src, src.length());
};
void ConvertJO2Je(char* src) 
{
	ConvertJO2JeTemplate(src, strlen(src));
};





std::string Format( const char* format, ... )
{
	va_list arglst;
	const size_t  SmallBufferSize = 15000;
    char SmallBuffer[SmallBufferSize+1];

    va_start( arglst, format );
	int	OutputLength = vsnprintf( SmallBuffer, SmallBufferSize, format, arglst);
	va_end( arglst );

	if (		(OutputLength == -1)
			||	(OutputLength < SmallBufferSize)
		)
		return SmallBuffer;

	if (OutputLength > 10000000) 
	{
		assert (false);
		OutputLength = 10000000;
	};


	char * pBuffer = new char[OutputLength+2];
	if (!pBuffer)
		return SmallBuffer;
	va_start( arglst, format );
	vsnprintf( pBuffer, OutputLength+1, format, arglst);
	va_end( arglst );
	std::string Res = pBuffer;
	delete pBuffer;
	return Res;

};



std::string&  TrimLeft (std::string& str)
{
	if (str.size() == 0) return str;
	size_t i = str.find_first_not_of(" \t\n\r");
	str.erase (0, i);
	return str;
};

std::string&  TrimRight (std::string& str)
{
	if (str.size() == 0) return str;
	size_t i = str.find_last_not_of(" \t\n\r");
	str.erase (i+1);
	return str;
};

std::string& Trim (std::string& str)
{
	TrimLeft(str);
	TrimRight(str);
	return str;
};



const std::string open_brackets = "{[(<";
const std::string close_brackets = "}])>";
const std::string all_brackets = open_brackets+close_brackets;


int isbracket  (BYTE x)
{
	if (open_brackets.find(x) != std::string::npos)	return 1; //открывающие скобки
	if (close_brackets.find(x) != std::string::npos)	return 2; // закрывающие
	return 0; 
}

// выдает соответствующую скобку 
size_t dual_bracket (BYTE x)
{
	size_t i = all_brackets.find(x);
	if (i == std::string::npos) return x;
	size_t s = open_brackets.length();
	if (i >= s)
		return all_brackets[i-s];
	else
		return all_brackets[i+s];
}



// ============     CShortString and  CShortStringHolder =========================

CShortString::CShortString(std::vector<char>::const_iterator pData)
{
	m_pStringPointer = pData;
};
BYTE CShortString::GetLength() const
{	
	return	(BYTE)m_pStringPointer[0];	
}

std::vector<char>::const_iterator	CShortString::GetData() const
{	
	return	m_pStringPointer;	
}

const char*	CShortString::GetString() const
{	
	return	&(m_pStringPointer[1]);	
}


bool  IsLessShortString::operator ()(const CShortString& Item1,	const char* Item2) const
{
	return strcmp(Item1.GetString(), Item2) < 0;
}

bool  IsLessShortString::operator ()(const char* Item1, const CShortString& Item2) const
{
	return strcmp(Item1, Item2.GetString() ) < 0;
}

bool  IsLessShortString::operator ()(const CShortString& Item1,	const CShortString& Item2) const
{
	return strcmp(Item1.GetString(), Item2.GetString()) < 0;
}

void CShortStringHolder::ReadShortStringHolder(std::string filename)
{
	clear();
	uint32_t Count;
	size_t BufferSize = (size_t)FileSize(filename.c_str()) - sizeof(Count);

	FILE* fp = fopen(filename.c_str(), "rb");
	if (!fp) return;
	
	fread ((void *)&Count, 1, sizeof(Count), fp);
	try {
		m_Buffer.clear();
		ReadVectorInner(fp, m_Buffer, BufferSize);
		fclose(fp);
		fp = 0;
	}
	catch (...)
	{
		if (fp) fclose(fp);
		throw;
	}

	reserve(Count);
	int Offset = 0;
	for (uint32_t i=0; i < Count; i++)
	{
		CShortString R(m_Buffer.begin()+Offset);
		push_back(R);
		Offset +=   R.GetLength() + 2; // 1 byte for length and 1 byte for 0 in the end
	};
	


};

template<class T>
bool CShortStringHolder::CreateFromSequence(T begin, T end)
{
	
	m_Buffer.clear();
	uint32_t Count = 0;
	for (; begin != end; begin++)
	{
		size_t length = begin->length();
		if (length > 254)
		{
			std::string s = *begin + " - too long";
			ErrorMessage(s.c_str(), "Short std::string convertor");
			return false;
		};

		m_Buffer.push_back((BYTE)length);
		// add with terminating null 
		m_Buffer.insert(m_Buffer.end(), begin->c_str(), begin->c_str() + length+1);

		Count++;
	}

	
	size_t Offset = 0;
	clear();
	for (uint32_t i=0; i < Count; i++)
	{
		CShortString R(m_Buffer.begin()+Offset);
		push_back(R);
		Offset +=   R.GetLength() + 2;
	};

	return true;
}


bool CShortStringHolder::CreateFromVector(const StringVector& in)
{
	return CreateFromSequence<StringVector::const_iterator>(in.begin(), in.end());
}

bool CShortStringHolder::CreateFromSet(const StringSet& in)
{
	return CreateFromSequence<StringSet::const_iterator>(in.begin(), in.end());
}

bool CShortStringHolder::WriteShortStringHolder(const std::string& FileName) const
{
	FILE* fp = fopen (FileName.c_str(), "wb");
	if (!fp)	return false;
	try
	{
        assert (size() < std::numeric_limits<uint32_t>::max());
        uint32_t nLength = size();
		if (fwrite((void*)&nLength, sizeof(nLength), 1,  fp) != 1)
		{
				fclose(fp);
				return false;
		}
		if (!WriteVectorInner(fp, m_Buffer))
		{
				fclose(fp);
				return false;
		}
		if (fclose(fp)) return false;
	}
	catch(...)
	{
		return false;
	}
	return true;
}


// ============     CMyTimeSpan and  CMyTimeSpanHolder =========================


CMyTimeSpan::CMyTimeSpan (long SequenceId, long InterfaceNestId)
{
	m_TimeSpan = 0;
	m_InterfaceNestId = InterfaceNestId;
	m_SequenceId = SequenceId;
	m_InvokeCount = 0;
};

void CMyTimeSpan::GetStrRepresentation(const char* Name, char* buffer, double AllClocksCount) const
{
    buffer[0] = 0;
	for (long i=0; i<m_InterfaceNestId; i++)
		strcat(buffer,"\t");
	char t[300];
	sprintf (t, "%s  = %6.0f seconds;%6.0f ticks ;%lu calls", 
		Name, 
		m_TimeSpan/(double)CLOCKS_PER_SEC, 

		m_TimeSpan, 
		m_InvokeCount
		);	
	strcat (buffer, t);
	if (AllClocksCount > 0)
	{
		char q[50];
		double dd = (AllClocksCount== 0)? 0 : (m_TimeSpan/AllClocksCount)*100;
		sprintf (q, "; %6.0f %%", dd);
		strcat (buffer, q);

	};

};



CMyTimeSpanHolder::CMyTimeSpanHolder()
{
	m_SequenceId = 0;
	m_bTimeSpanHolderEnabled = true;
};

void CMyTimeSpanHolder::StartTimer(const std::string& Name, long NestId)
{
	if (!m_bTimeSpanHolderEnabled)  return ;
	Iterator it = m_TimeSpans.find(Name);

	if(it == m_TimeSpans.end())
	{
		CMyTimeSpan X (m_SequenceId, NestId);

		std::pair<Iterator, bool> p = m_TimeSpans.insert(std::pair<std::string, CMyTimeSpan>(Name, X));

		it = p.first;
	};

	it->second.m_StartTime = clock();
	it->second.m_InvokeCount++;

	m_SequenceId++;
};

//  finish timer "Name" and returns the elapsed time in ticks
double CMyTimeSpanHolder::EndTimer(const std::string& Name)
{
	if (!m_bTimeSpanHolderEnabled)  return 0;

	Iterator it = m_TimeSpans.find(Name);

	if(it == m_TimeSpans.end()) 
	{
		ErrorMessage (Format("Profiler error: no timer with name %s", Name.c_str()) );
		return 0;
	};

	it->second.m_TimeSpan += (double)(clock() - it->second.m_StartTime) ;

	
	return it->second.m_TimeSpan;
};


std::string CMyTimeSpanHolder::GetStrRepresentation (double AllClocksCount) const
{
	std::string Result;

	if (!m_bTimeSpanHolderEnabled)  return "profiler is disabled\n";

	
	std::map<int, ConstIterator>			TimerSequence;

	for (ConstIterator It = m_TimeSpans.begin(); It != m_TimeSpans.end();It++)
		TimerSequence[It->second.m_SequenceId] = It;

	for (std::map<int, ConstIterator>::const_iterator It1 = TimerSequence.begin(); It1 != TimerSequence.end();It1++)
	{
		char s[1000];
		ConstIterator It = It1->second;
		It->second.GetStrRepresentation(It->first.c_str(), s, AllClocksCount);
		Result += s;
		Result += '\n';
	};
	return Result;
};


void CMyTimeSpanHolder::ClearTimers() 
{
	m_TimeSpans.clear();
	m_SequenceId = 0;
};


bool LoadFileToString(std::string FileName, std::string& Result)
{
	if ( access(FileName.c_str(), 4) != 0) return false;


	std::vector<char> V;
	ReadVector(FileName,V);
	Result = std::string (V.begin(), V.end());
	return true;
};

bool is_pseudo_graph(BYTE x)
{
	return  (x==134) 
		  ||(x==135) 
		  ||(x==128) 
		  ||(x==131) 
		  ||(x==136) 
		  ||(x==137) 
		  ||((x>=139) && (x<=175))
		  ||((x>=176) && (x<=181))
		  ||((x>=186) && (x<=191));
};


// ======================== CExcp ================
CExpc::CExpc(const std::string& Cause) 
{
    m_strCause = Cause;
	m_ErrorCode = 0;
};

CExpc::CExpc(int ErrorCode, const std::string& Cause) 
{
    m_strCause = Cause;
	m_ErrorCode = ErrorCode;
};



CExpc::CExpc(const char* format, ... )
{
   	va_list arglst;
	const size_t  SmallBufferSize = 512;
    char SmallBuffer[SmallBufferSize+1];

    va_start( arglst, format );
	vsnprintf( SmallBuffer, SmallBufferSize, format, arglst);
	va_end( arglst );
    m_strCause = SmallBuffer;
    m_ErrorCode = 0;

    
}

CExpc::CExpc(const CExpc& from)
{
	*this = from;
};
CExpc& CExpc::operator= (const CExpc& from)
{
	m_strCause = from.m_strCause;
    m_ErrorCode = from.m_ErrorCode;
	return *this;
}

// ====================   StringTokenizer =======================

void StringTokenizer::initialize(const char *_text, const char *_delims)
{
	delims = _delims;
	i = -1;
	size_t leng = strlen(_text);
	text_ptr = new char[leng+2];
	text = text_ptr;
	strcpy(text, _text);
	text[leng+1] = 0;
};

StringTokenizer::StringTokenizer(const char *_text, const char *_delims)
{
	initialize(_text, _delims);
}

StringTokenizer::~StringTokenizer(){
	delete []text_ptr;
}

const char * StringTokenizer::get_rest () const 
{ 
	return text;	
};
const char * StringTokenizer::val() const  
{
	return _val;
}
int  StringTokenizer::count() const  
{
	return i;
}
bool StringTokenizer::has_next() const 
{
	return (*text) != 0;
}
bool StringTokenizer::is_delim(char ch) const
{
		return delims.find_first_of(ch) != std::string::npos;
}


void StringTokenizer::reinitialize(const char *_text, const char *_delims)
{
	assert (text_ptr);
	delete []text_ptr;
	initialize(_text, _delims);
}

const char * StringTokenizer::operator ()()
{
	++i;
	// skip delims
	while(*text && is_delim(*text))
		text++;

	if(*text == 0)	return 0;

	char *ret_val = text;

	while(*text && !is_delim(*text))
		text++;
	*text++ = 0;
	_val = ret_val;
	if (_val == 0)
		throw CExpc ("Exception in StringTokenizer::operator ()");
	return ret_val;
}
std::string StringTokenizer::next_token ()
{
	const char* s = operator()();
	if (s) 
		return s;
	else 
		return std::string();
}




std::string ConvertASCIIToHtmlSymbols(const std::string& txt)
{
	std::string out;
	char asciiCode[2];
	asciiCode[1] = 0;
	for( int i=0; i<txt.length(); i++ )
	{
		asciiCode[0] = txt[i];
		switch (asciiCode[0])
		{
			case ' ':
				out += " ";
			break;
			case '\t':
				out += "    ";
			break;
			case '"':
				out += "&quot;";
			break;
			case '&':
				out += "&amp;";
			break;
			case '<':
				out += "&lt;";
			break;
			case '>':
				out += "&gt;";
			break;
			default:
				out += asciiCode;
		}
	}
	return out;
}


bool  MakeDir(const std::string& txt)
{
	#ifdef WIN32
		return _mkdir(txt.c_str()) != -1;
	#else
		return mkdir(txt.c_str(), 0777) != -1;
	#endif
};






bool RemoveWithPrint (const std::string& FileName)
{
    if (FileExists(FileName.c_str()))
		if (remove(FileName.c_str()))
		{
			fprintf(stderr, "Cannot remove %s errno=%i\n", FileName.c_str(), errno);
			return false;
		}
    return true;
}

size_t HashValue(const char *pc) 
{
    static const uint32_t mask[sizeof(uint32_t)] = {
        0x00000000, 0x000000FF, 0x0000FFFF, 0x00FFFFFF
    };
    size_t len =     strlen(pc);
    size_t h = len;
    size_t i = len / sizeof(uint32_t);
    const uint32_t *c = (const uint32_t *)pc;

    while (i--)
        h ^= *c++;

    i = len % sizeof(uint32_t);
    if (i > 0)
        h ^= (*c & mask[i]); 

    return h;
}




bool RmlCopyFile(const char *oldpath, const char *newpath)
{
    char buf[512];
   
    FILE* fi = fopen(oldpath, "rb");
    if (!fi)
        return false;
    FILE* fo = fopen(newpath, "wb");
    if (!fo)
        return false;
    int x = 0;
    do 
    {
        x = fread(buf, 1, 512, fi);
        if (x > 0) 
            if (fwrite(buf, 1, x, fo) < x) 
            {	/* Couldn't write */
                fclose(fo);
                fclose(fi);
                unlink(newpath);
                return false;
            }
    }
    while ( x  > 0);
    fclose(fo);
    fclose(fi);
    return true;
}

bool RmlMoveFile(const char *oldpath, const char *newpath)
{
  if (!rename(oldpath, newpath))
    return true;
  
  if (!RmlCopyFile(oldpath, newpath)) return false;
  unlink(oldpath);
  return true;
}

std::string BuildRMLPath (const char* s)
{
    std::string path = s;
    size_t i = path.find("$RML");
    if (i != std::string::npos)
        path.replace(i, 4, GetRmlVariable());
    return path;
}

uint64_t pow(uint64_t x,int y)
{
	if(x>1&&y>100) return 0;
	return y==0 || x==1 ? 1 : x*pow(x,y-1);
}

int CountBits(uint64_t value)
{
	int count = 0;
	for (int i=0; i < 64; i++)
	{
		count += (value >> i) & 1;
	}
	return count;
}

size_t FindFloatingPoint(const char* str)
{
	if(!str || sizeof(str)<3) return -1;
	std::string s(str);
	size_t c = s.rfind(",");
	if (c == std::string::npos) 
		c = s.rfind(".");
	return c == std::string::npos ? -1 : c;
}

std::string convert_from_utf8(const char* utf8str, const MorphLanguageEnum langua) {
	try {
		if (langua == morphRussian) {
			return convert_utf8_to_cp1251(utf8str);
		}
		return convert_utf8_to_cp1252(utf8str);
	}
	catch (convert_exception e) {
		throw CExpc(e.what());
	}
}


std::string convert_to_utf8(const std::string& str, const MorphLanguageEnum langua) {
	try {
		if (langua == morphRussian) {
			return convert_cp1251_to_utf8(str);
		}
		return convert_cp1252_to_utf8(str);
	}
	catch (convert_exception e) {
		throw CExpc(e.what());
	}
}

#ifdef WIN32
	std::wstring utf8_to_utf16(const std::string& utf8)
	{
		std::wstring utf16;
		int len = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
		if (len > 0)
		{
			utf16.resize(len);
			wchar_t* ptr = &utf16[0];
			if (ptr) MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, ptr, len);
		}
		return utf16;
	}

	std::string utf16_to_utf8(const std::wstring & wstr) 
	{
		// I cannot compile converting with wstring_convert under Visual Studio.
		// Remember, that codecvt_utf8_utf16  is deprecated in C++17
		if (!wstr.empty())
		{
			int sizeRequired = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);

			if (sizeRequired > 0)
			{
				std::vector<char> utf8String(sizeRequired);
				int bytesConverted = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(),
					-1, &utf8String[0], utf8String.size(), NULL,
					NULL);
				if (bytesConverted != 0)
				{
					return &utf8String[0];
				}
				else
				{
					std::stringstream err;
					err << __FUNCTION__
						<< " std::string WstrToUtf8Str failed to convert wstring '"
						<< wstr.c_str() << L"'";
					throw std::runtime_error(err.str());
				}
			}
		}
		return std::string("");
	}
#else //UNUX

	std::wstring utf8_to_utf16(const std::string& str) {
		assert (false);
		//std::wstring_convert<std::codecvt_utf8_utf16<char16_t> > converter;
		//return converter.from_bytes(str);
	}


	std::string utf16_to_utf8(const std::wstring& wstr) {
		assert (false);
		//std::u16string u16str(wstr.begin(), wstr.end());
		//std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t > converter;
		//std::string s =  converter.to_bytes(u16str.c_str());
		//return s;
	}

#endif 

void CTestCaseBase::read_test_cases(std::istream& inp) {
	TestCases.clear();
	std::string s;
	while (getline(inp, s)) {
		size_t commentsIndex = s.find("//");
		CTestCase e;
		if (commentsIndex != -1) {
			e.Comment = s.substr(commentsIndex);
			s = s.substr(0, commentsIndex);
		}
		Trim(s);
		e.Text = s;
		TestCases.push_back(e);
	}
}

void CTestCaseBase::write_test_cases(std::ostream& outp) const {
	nlohmann::json cases = nlohmann::json::array();
	for (auto c : TestCases) {
		cases.push_back({
			{"input", c.Text},
			{"result", c.Result},
			{"comments", c.Comment},
			});
	}
	outp << cases.dump(4);
}


//#if __cplusplus > 201402L

// "c:/*.txt" -> c:/a.txt, c:/b.txt
std::vector<std::string> list_path_by_file_mask(std::string filemask)
{
	auto path = std::filesystem::path(filemask);
	std::vector<std::string> filenames;
	for (const auto& p : std::filesystem::directory_iterator(path.parent_path())) 	{
		if (p.path().extension() == path.extension()) {
			filenames.push_back(p.path().string());
		}
	}
	return filenames;
};

std::string join_string(const std::vector<std::string>& items, const std::string& delimiter) {
	std::string result;
	if (items.empty()) {
		return result;
	}
	result = items[0];
	for (size_t i = 1; i < items.size(); ++i) {
		result += delimiter + items[i];
	}
	return result;
}
