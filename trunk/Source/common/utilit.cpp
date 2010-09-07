#include "utilit.h"
#include "util_classes.h"
#include "../common/bserialize.h"
#include <time.h>
#include <errno.h>

//  for mkdir
#ifdef WIN32
	#include <direct.h>
#else
	#include <sys/stat.h>
#endif

#ifdef DETECT_MEMORY_LEAK
	#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
	#endif
#endif

const WORD fWordDelim = 1;
const WORD RusUpper = 2;
const WORD RusLower = 4;
const WORD GerUpper = 8;
const WORD GerLower = 16;
const WORD EngUpper = 32;
const WORD EngLower = 64;
const WORD OpnBrck = 128;
const WORD ClsBrck = 256;
const WORD UpRomDigits = 512;
const WORD LwRomDigits = 1024;
const WORD LatinVowel = 2048;
const WORD RussianVowel = 4096;
const WORD URL_CHAR = 8192;


const WORD ASCII[256] = {
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
/*character tabulation set (hts) '_'*/             fWordDelim,
/*character tabulation with justification (htj) '%'*/ fWordDelim,
/*line tabulation set (vts) '_'*/                  0,
/*partial line forward (pld) '<'*/                 fWordDelim,
/*partial line backward (plu) '_'*/                fWordDelim,
/*reverse line feed (ri) '_'*/                     fWordDelim,
/*single-shift two (ss2) '_'*/                     fWordDelim,
/*single-shift three (ss3) '_'*/                   fWordDelim,
/*device control string (dcs) '_'*/                fWordDelim,
/*private use one (pu1) '''*/                      fWordDelim,
/*private use two (pu2) '''*/                      fWordDelim,
/*set transmit state (sts) '"'*/                   fWordDelim,
/*cancel character (cch) '"'*/                     fWordDelim,
/*message waiting (mw) ''*/                       fWordDelim,
/*start of guarded area (spa) '-'*/                fWordDelim,
/*end of guarded area (epa) '-'*/                  fWordDelim,
/*start of string (sos) '_'*/                      fWordDelim,
/*single graphic character introducer (sgci) 'T'*/ fWordDelim,
/*single character introducer (sci) '_'*/          fWordDelim,
/*control sequence introducer (csi) '>'*/          fWordDelim,
/*string terminator (st) '_'*/                     fWordDelim,
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

file_off_t FileSize (const char *filename)
{
	FILE * fp = fopen (filename,"rb");
	if (fp == NULL) return 0;
	if (!FSeek(fp, 0,  SEEK_END)) 
        throw CExpc ("Cannot move to the begining of the file: %s", filename);;

	file_off_t size = FTell (fp);
	fclose (fp);
	return size;
}


bool FSeek(FILE* fp, file_off_t pos, int origin)
{
	#ifdef WIN32
		return _fseeki64(fp, pos, origin) == 0;
	#else
		return fseeko(fp, pos, origin) == 0;
	#endif

}

file_off_t FTell(FILE* fp)
{
	#ifdef WIN32
		return _ftelli64(fp) ;
	#else
		return ftello(fp);
	#endif
}


void(*GlobalErrorMessage)(const string&) = 0;

void ErrorMessage (const string& Titul, const string& Message)
{
	if (GlobalErrorMessage)
	{
		string q = Titul + ":"+Message;
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

	string q = Message;
	if (q.empty()|| (q[q.length() -1]!='\n'))	q+='\n';
	fprintf(stderr, "%s: %s", Titul.c_str(), q.c_str());
	
};

void ErrorMessage (const string& Message)
{
	ErrorMessage("error", Message);
};

string MakeFName ( const string& InpitFileName,  const string& Ext)
{
	string Result = InpitFileName;
	size_t i = Result.find_last_of('.'); // найти последнее расширение
	if ( i  != string::npos) 
		Result.erase(i);	
	Result += string(".") ;
	Result += Ext;
	return Result;
}







bool MakePath (const char* RossPath, const char* FileName, char* FullPath)
{ 
	if (!RossPath || !FileName || !FullPath) return false;

	strcpy(FullPath,RossPath);

	#ifdef WIN32

		if (		 (FullPath[strlen(FullPath) - 1] != '\\') 
				&&	(FullPath[strlen(FullPath) - 1] != '/') 
			)
			strcat (FullPath,"\\");
	#else

		if (FullPath[strlen(FullPath) - 1] != '/')
			strcat (FullPath,"/");

	#endif

	strcat (FullPath, FileName);	  

	return  FileExists(FullPath);
};

string GetPathByFile (string FileName)
{ 
  size_t i = FileName.rfind("\\");	  
  size_t j = FileName.rfind("/");	  

  if (			(i == string::npos) 
		&&		(j == string::npos)
	  )
	  return "";
  else
	  if (i == string::npos) 
		  return FileName.substr(0, j+1);
	  else
		if (j == string::npos) 
			return FileName.substr(0, i+1);
		else
			return FileName.substr(0, max(i,j)+1); 
};


bool IsBinFile (const char* FileName)
{
	return      FileName 
		    &&  (strlen (FileName) > 3)
		    &&  !strncmp (FileName+strlen(FileName) - 3, "bin", 3);
};

string	CreateTempFileName()
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


void rml_TRACE( const char* format, ... )
{

#ifdef WIN32
	#ifdef _DEBUG
	      va_list arglst;
          char s[2000];
		  if (strlen (format) >  200)
		  {
			  OutputDebugString("!!!!! too long debug line!!!");
			  return;
		  };

          va_start( arglst, format );
	      vsprintf( s, format, arglst);
	      va_end( arglst );
		  OutputDebugString(s);
	#endif
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





// ================== IsSuperEqualChar проверяет, что символы равны с точностью до регистра
//  и с точностью до "языка", m.е. латинское "a" будет равно  русскому "а".
bool IsSuperEqualChar (BYTE ch1, BYTE ch2, MorphLanguageEnum langua)
{
	if	(		(ch1 == ch2) 
			||	(ch1 == ReverseChar(ch2, langua))
		) 
	return true;

	const size_t LenEF    = 53;
	//  первая буква - прописная  кириллическая буква, что необходимо,
	// чтобы установить порядок на полях эквивалентности букв
	char const EF[LenEF+1] = "\0аАaA\0еЕEe\0зЗ3\0кКKk\0мМM\0нНH\0оОOo0\0рРpP\0сСCc\0уУy\0хХXx";

	size_t i = 0, k=0;
	for (i=0; i<LenEF; i++)
		if (!EF[i]) 
			k = i+1;
		else
		{
			if (ch1 == (BYTE)EF[i]) ch1 = (BYTE)EF[k];
			if (ch2 == (BYTE)EF[i]) ch2 = (BYTE)EF[k];
		}

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





BYTE force_rus_char (BYTE ch)
{
	// в первой строке находятся латинские буквы, который могут быть заменены на
	// русские
	char search[] = "AaEe3KkMHOoPpCcyXxuT";
	// во второй строке находятся русские
	char replace[] = "АаЕеЗКкМНОоРрСсуХхиТ";

	char *t = strchr (search,ch);

	if (t == 0) return 0;

	return replace[(size_t) ((char*)t- (char*)search)];
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

string& IntToStr (int Value, string& oBuffer)
{
	char Buffer[256];
	sprintf (Buffer, "%i", Value);
	oBuffer = Buffer;
	return oBuffer;
};

#ifndef WIN32
void strrev(char* s)
{
  if (!s) return;
  size_t len = strlen(s);
  for (int i=0; i < len/2; i++)
    {
      swap(s[i], s[len - i - 1]);
    }

};
#endif



bool IsHtmlFile (const string& FileName)
{
	size_t len = FileName.length();
	string R = FileName;
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

#ifdef WIN32
	string TryReadRMLFromRegistry()
	{
		HKEY hKeyResult; 
		LONG res; 
		res = RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment",  0, KEY_ALL_ACCESS, &hKeyResult ); 
		if(res != ERROR_SUCCESS)
			throw CExpc("Error! Environment variable \"RML\"is not set!");
		else
		{ 
			char buf[512]; 
			DWORD dwBytes = sizeof(buf); 
			res = RegQueryValueEx( hKeyResult, "RML", 0, NULL, reinterpret_cast <PBYTE> (buf), &dwBytes); 
			if(res != ERROR_SUCCESS) 
				throw CExpc("Error! Environment variable \"RML\"is not set!");
			return buf;
		}
	} 
#endif

string GetRmlVariable()
{
	const char* rml = getenv("RML");
	string s;
	if (rml)
		s = rml;
	else
	{
		#ifndef WIN32
			throw CExpc("Error! Environment variable \"RML\"is not set!");
		#else
			s = TryReadRMLFromRegistry();
		#endif
		
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


const char RML_REGISTRY_FILENAME[] = "rml.ini";


static string GetStringInnerFromTheFile (string RegistryPath, string MainPath, string RmlPath) 
{
	string FileName = 	MainPath +  "/" + RML_REGISTRY_FILENAME;
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
		string s = buffer;
		Trim(s);
		if (s.empty()) continue;
		size_t end_field_name = strcspn(s.c_str(), " \t");
		if (end_field_name == s.length())
		{
			fclose(fp);
			throw CExpc ("Cannot parse line "+s);
		};
		string Key = s.substr(0,end_field_name);
		if (RegistryPath == Key)
		{
			string Value = s.substr(end_field_name);
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

string GetIniFilePath()
{
	return GetRmlVariable()+"/Bin";
};

string GetRegistryString (string RegistryPath)
{
	string Result = GetStringInnerFromTheFile(RegistryPath, GetIniFilePath(), GetRmlVariable().c_str());
	return Result;
	
};

string GetRegistryStringFromLocalIniFile (string RegistryPath)
{
	string  Result = GetStringInnerFromTheFile(RegistryPath, ".", "$RML");
	return Result;
};

bool CanGetRegistryString (string RegistryPath)  
{
	try {
		string s = GetRegistryString(RegistryPath);
		return  s != "";
	}
	catch (...)
	{
		return false;
	};
}


void SetRegistryString (string RegistryPath, string Value)
{  
	if (RegistryPath.find(' ') != -1) 
		throw CExpc ("A registry path cannot contain spaces");

	string FileName = GetIniFilePath() + "/" + RML_REGISTRY_FILENAME;

	FILE* fp = fopen (FileName.c_str(), "r");
	if (fp == 0)
		throw CExpc ("Cannot open "+FileName);
	string TempFileName = CreateTempFileName();
	FILE* outfp = fopen (TempFileName.c_str(), "w");
	if (!outfp) 
		throw CExpc ("Cannot open temporary file for setting variables in Registry");

	char buffer[2048];

	while (fgets(buffer,2048,fp))
	{
		string s = buffer;
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


bool	IsRmlRegistered(string& Error)
{
	try 
	{
		string IniFile = GetIniFilePath() +  "/" + RML_REGISTRY_FILENAME;
	
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


bool CheckEvaluationTime ()
{
	tm today = RmlGetCurrentTime();

	//  2011 year
	if (today.tm_year > 111)
	{
		ErrorMessage("Evaluation period is expired. Write to sokirko@yandex.ru!");
		return false;
	};
	return true;
};


//=======================           Morphology  =======================

const int predict_poses_size = 4;

//  we cannot use here class string, because VC7 shows in this case memory leaks
struct CPredictPartOfSpeech {
	char m_pos_str[100];
};
const CPredictPartOfSpeech  predict_rus_pos[predict_poses_size] =
{
	"С",
	"ИНФИНИТИВ",
	"П",
	"Н",
};

const CPredictPartOfSpeech predict_eng_pos[predict_poses_size] =
{
	"NOUN",
	"VERB",
	"ADJECTIVE",
	"ADVERB",
};
const CPredictPartOfSpeech predict_ger_pos[predict_poses_size] =
{
	"SUB",
	"VER",
	"ADJ",
	"ADV",
};


int GetPredictionPartOfSpeech(const string& PartOfSpeech, MorphLanguageEnum langua)
{
	const CPredictPartOfSpeech* PossiblePoses;
	switch (langua)
	{
		case morphEnglish: PossiblePoses = predict_eng_pos; break;
		case morphGerman: PossiblePoses = predict_ger_pos; break;
		case morphRussian: PossiblePoses = predict_rus_pos; break;
		default : assert(false);
	};


	int k=0;
	for(; k < predict_poses_size; k++ )
		if( PartOfSpeech == PossiblePoses[k].m_pos_str )
		{
			break;
		};

	if (k == predict_poses_size)
		return UnknownPartOfSpeech;
	else
		return k;
};


bool GetLanguageByString (string s, MorphLanguageEnum& Result)
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

string GetStringByLanguage (MorphLanguageEnum Langua)
{
	switch (Langua) {
		case morphRussian: return  "Russian";
		case morphEnglish: return  "English";
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

const BYTE  Ccedille = 199;
const BYTE  Egravis = 200;
const BYTE  Ezirkun = 202;
const BYTE  Azirkun = 194;
const BYTE  Ozirkun = 212;
const BYTE  Ntilda = 209;
const BYTE  Eakut = 201;
const BYTE  Uzirkun = 219;

const BYTE  ccedille = 231;
const BYTE  egravis = 232;
const BYTE  ezirkun = 234;
const BYTE  azirkun = 226;
const BYTE  ozirkun = 244;
const BYTE  ntilda = 241;
const BYTE  eakut = 233;
const BYTE  uzirkun = 251;

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
		case egravis : return Egravis;  // E with gravis
		case ezirkun : return Ezirkun;  // E with zirkun
		case azirkun : return Azirkun;  // A with zirkun
		case ozirkun : return Ozirkun;  // O with zirkun
		case ouml : return Ouml;  // O umlaut
		case ntilda : return Ntilda;  // N with tilda
		case uzirkun : return Uzirkun;  // U with zirkun
		case eakut : return Eakut;  // E acut
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
		case Egravis : return egravis;  // E with gravis
		case Ezirkun : return ezirkun;  // E with zirkun
		case Azirkun : return azirkun;  // A with zirkun
		case Ozirkun : return ozirkun;  // O with zirkun
		case Ouml : return ouml;  // O umlaut
		case Ntilda : return ntilda;  // N with tilda
		case Uzirkun : return uzirkun;  // U with zirkun
		case Eakut : return eakut;  // E acut
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
		return  ch -= 'а'-'А';
	else
		return ch;
};

BYTE rtolower (BYTE ch) 
{ 
	if (ch == UpperJO) return LowerJO;
	if (ch == Apostrophe ) return Apostrophe;

	if ( is_russian_upper(ch) )
		return ch += 'а'-'А';
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

bool is_lower_consonant(BYTE x, MorphLanguageEnum Langua)
{
	if (!is_lower_alpha(x, Langua)) return false;
	return !is_lower_vowel(x,Langua);
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

string& EngMakeUpper (string& word)
{
	return RegisterConverter(word, word.length(), is_english_lower, etoupper);
}

string& EngMakeLower (string& word)
{
	return RegisterConverter(word, word.length(), is_english_upper, etolower);
}


char* GerMakeUpper (char *word)
{
	return RegisterConverter(word, strlen(word), is_german_lower, gtoupper);
}

string& GerMakeUpper (string& word)
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
bool IsRussian (const string& word)
{
	return CheckLanguage(word, word.length(), morphRussian);
}

bool IsEnglish (const char *word)
{
	return CheckLanguage(word, strlen(word), morphEnglish);
}

bool IsEnglish (const string& word)
{
	return CheckLanguage(word, word.length(), morphEnglish);
}

bool IsGerman (const char *word)
{
	return CheckLanguage(word, strlen(word), morphGerman);
}
bool IsGerman (const string& word)
{
	return CheckLanguage(word, word.length(), morphGerman);
}

bool CheckLanguage (const char *word, MorphLanguageEnum langua)
{
	return CheckLanguage(word, strlen(word), langua);
};

bool CheckLanguage (const string& word, MorphLanguageEnum langua)
{
	return CheckLanguage(word, word.length(), langua);
};




// конвертирует из прописные  кириллицы в строчную
string& EngRusMakeLower (string& word)
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

string& RmlMakeUpper (string& word, MorphLanguageEnum langua)
{
	GerEngRusMakeUpperTemplate(word, langua, word.length());
	return word;
};	

string& RmlMakeLower (string& word, MorphLanguageEnum langua)
{
	if (word.length() == 0) return word;

	if (langua == morphGerman)
		return RegisterConverter(word, word.length(), is_german_upper, gtolower);
	else
		return EngRusMakeLower(word);
};	


// конвертирует из строчной кириллицы в прописные 
string&  EngRusMakeUpper (string& word)
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
			src[i] = (BYTE)'е';
		else
		
			if ( ( (BYTE)src[i]) == UpperJO)
			src[i] = (BYTE)'Е';
	}

};

void ConvertJO2Je(char* src, size_t Length)
{
	ConvertJO2JeTemplate(src, Length);
};



void ConvertJO2Je(string& src) 
{

	ConvertJO2JeTemplate(src, src.length());
};
void ConvertJO2Je(char* src) 
{
	ConvertJO2JeTemplate(src, strlen(src));
};





string Format( const char* format, ... )
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
	string Res = pBuffer;
	delete pBuffer;
	return Res;

};



string&  TrimLeft (string& str)
{
	if (str.size() == 0) return str;
	size_t i = str.find_first_not_of(" \t\n\r");
	str.erase (0, i);
	return str;
};

string&  TrimRight (string& str)
{
	if (str.size() == 0) return str;
	size_t i = str.find_last_not_of(" \t\n\r");
	str.erase (i+1);
	return str;
};

string& Trim (string& str)
{
	TrimLeft(str);
	TrimRight(str);
	return str;
};



const string open_brackets = "{[(<";
const string close_brackets = "}])>";
const string all_brackets = open_brackets+close_brackets;


int isbracket  (BYTE x)
{
	if (open_brackets.find(x) != string::npos)	return 1; //открывающие скобки
	if (close_brackets.find(x) != string::npos)	return 2; // закрывающие
	return 0; 
}

// выдает соответствующую скобку 
size_t dual_bracket (BYTE x)
{
	size_t i = all_brackets.find(x);
	if (i == string::npos) return x;
	size_t s = open_brackets.length();
	if (i >= s)
		return all_brackets[i-s];
	else
		return all_brackets[i+s];
}



// ============     CShortString and  CShortStringHolder =========================

CShortString::CShortString(vector<char>::const_iterator pData)
{
	m_pStringPointer = pData;
};
BYTE CShortString::GetLength() const
{	
	return	(BYTE)m_pStringPointer[0];	
}

vector<char>::const_iterator	CShortString::GetData() const
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






void CShortStringHolder::ReadShortStringHolder(string filename)
{
	clear();
	file_off_t BufferSize = (int)FileSize(filename.c_str()) - sizeof(size_t);

	FILE* fp = fopen(filename.c_str(), "rb");
	if (!fp) return;
	size_t Count;
	fread ((void *)&Count, 1, sizeof(size_t), fp);
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
	for (DWORD i=0; i < Count; i++)
	{
		CShortString R(m_Buffer.begin()+Offset);
		push_back(R);
		Offset +=   R.GetLength() + 2;
	};
	


};

template<class T>
bool CShortStringHolder::CreateFromSequence(T begin, T end)
{
	
	m_Buffer.clear();
	size_t Count = 0;
	for (; begin != end; begin++)
	{
		size_t length = begin->length();
		if (length > 254)
		{
			string s = *begin + " - too long";
			ErrorMessage(s.c_str(), "Short string convertor");
			return false;
		};

		m_Buffer.push_back((BYTE)length);
		// add with terminating null 
		m_Buffer.insert(m_Buffer.end(), begin->c_str(), begin->c_str() + length+1);

		Count++;
	}

	
	size_t Offset = 0;
	clear();
	for (DWORD i=0; i < Count; i++)
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

bool CShortStringHolder::WriteShortStringHolder(const string& FileName) const
{
	FILE* fp = fopen (FileName.c_str(), "wb");
	if (!fp)	return false;
	try
	{
		size_t nLength = size();
		if (fwrite((void*)&nLength, sizeof(size_t), 1,  fp) != 1)
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
	sprintf (t, "%s  = %6.0f seconds;%6.0f ticks ;%i calls", 
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

void CMyTimeSpanHolder::StartTimer(const string& Name, long NestId)
{
	if (!m_bTimeSpanHolderEnabled)  return ;
	Iterator it = m_TimeSpans.find(Name);

	if(it == m_TimeSpans.end())
	{
		CMyTimeSpan X (m_SequenceId, NestId);

		pair<Iterator, bool> p = m_TimeSpans.insert(pair<string, CMyTimeSpan>(Name, X));

		it = p.first;
	};

	it->second.m_StartTime = clock();
	it->second.m_InvokeCount++;

	m_SequenceId++;
};

//  finish timer "Name" and returns the elapsed time in ticks
double CMyTimeSpanHolder::EndTimer(const string& Name)
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


string CMyTimeSpanHolder::GetStrRepresentation (double AllClocksCount) const
{
	string Result;

	if (!m_bTimeSpanHolderEnabled)  return "profiler is disabled\n";

	
	map<int, ConstIterator>			TimerSequence;

	for (ConstIterator It = m_TimeSpans.begin(); It != m_TimeSpans.end();It++)
		TimerSequence[It->second.m_SequenceId] = It;

	for (map<int, ConstIterator>::const_iterator It1 = TimerSequence.begin(); It1 != TimerSequence.end();It1++)
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



void QPEncodeString(string& s)
{
	string z;
	for (size_t i=0; i<s.length(); i++)
		z+= Format("%2x",(BYTE)s[i]);
	s = z;
}


void QPDecodeString(string& s)
{
	string z;
	char b[3];
	b[2] = 0;
	for (size_t i=0; i<s.length(); i+=2)
	{
		strncpy(b, s.c_str()+i,2);
		int c;
		sscanf(b, "%x",&c);
		z += (BYTE)c;
	};
	s = z;
}




bool LoadFileToString(string FileName, string& Result)
{
	if ( access(FileName.c_str(), 4) != 0) return false;


	vector<char> V;
	ReadVector(FileName,V);
	Result = string (V.begin(), V.end());
	return true;
};

void		SqueezeSpacesAndDeleteEoln(string& s)
{
	string result;
	for (size_t i=0; i< s.length(); i++)
		if ( isspace((BYTE) s[i]) )
		{
			if ( (i== 0) || !isspace((BYTE) s[i-1]))
				result += " ";
		}
		else
		result += s[i];
	s = result;

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


bool is_word_delim (BYTE x)
{
	return 	isbracket(x)
		||	ispunct(x)
		||	(x <= 32)
		||	is_pseudo_graph (x)
		||	(x == cCompanyChar)
		||	(x == cIonChar)
		||	(x == cNumberChar)
		||	(x == cPiChar)
		||	(x == cEllipseChar);
}



// ======================== CExcp ================
CExpc::CExpc(const string& Cause) 
{
    m_strCause = Cause;
	m_ErrorCode = 0;
};

CExpc::CExpc(int ErrorCode, const string& Cause) 
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

bool  ReadTimeOutFromRegistry (bool bReadFromLocalFile, int& TimeOut)
{
	string NetworkTimeOutStr;
	try {
		if (bReadFromLocalFile)
			NetworkTimeOutStr  = GetRegistryStringFromLocalIniFile( "NetworkTimeOut" );
		else
			NetworkTimeOutStr  = GetRegistryString( "NetworkTimeOut" );

	}
	catch (...) {

		ErrorMessage("Cannot read NetworkTimeOut from rml.ini\n");
		return false;
	};

	TimeOut = 0;
		
	if (!NetworkTimeOutStr.empty())
		TimeOut = atoi(NetworkTimeOutStr.c_str());

	if (TimeOut == 0)
		TimeOut = 100;

	return true;
};


//==================================================================
//=========================== KOI8 <==> WIN ========================
//==================================================================

BYTE kw[] = {128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
254,224,225,246,228,229,244,227,245,232,233,234,235,236,237,238,
239,255,240,241,242,243,230,226,252,251,231,248,253,249,247,250,
222,192,193,214,196,197,212,195,213,200,201,202,203,204,205,206,
207,223,208,209,210,211,198,194,220,219,199,216,221,217,215,218};


BYTE wk[] = {128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
225,226,247,231,228,229,246,250,233,234,235,236,237,238,239,240,
242,243,244,245,230,232,227,254,251,253,255,249,248,252,224,241,
193,194,215,199,196,197,214,218,201,202,203,204,205,206,207,208,
210,211,212,213,198,200,195,222,219,221,223,217,216,220,192,209};



char wtk(char c)
{
	
	c&= 0377;
	if(c & 0200)
		c	= wk[c & 0177];
	return c;
}
char ktw(char c)
{
	c&= 0377;
	if(c & 0200)
		c	= kw[c & 0177];
	return c;
}


void WinToKOI8 (string& s)
{
	 size_t count = s.length();
	 for (int i=0; i< count; i++)
		 s[i] = wtk(s[i]);
};

void KOI8ToWin (string& s)
{
	 size_t count = s.length();
	 for (int i=0; i< count; i++)
		 s[i] = ktw(s[i]);
};




static const DWORD arrdwCrc32Table[256] =
{
	0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
	0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
	0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
	0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
	0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
	0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
	0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
	0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
	0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
	0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
	0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
	0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
	0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
	0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
	0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
	0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,

	0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
	0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
	0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
	0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
	0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
	0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
	0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
	0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
	0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
	0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
	0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
	0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
	0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
	0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
	0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
	0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,

	0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
	0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
	0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
	0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
	0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
	0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
	0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
	0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
	0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
	0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
	0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
	0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
	0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
	0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
	0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
	0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,

	0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
	0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
	0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
	0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
	0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
	0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
	0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
	0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
	0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
	0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
	0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
	0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
	0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
	0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
	0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
	0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D,
};


DWORD StringCrc32(const char* szString)
{
	DWORD dwCrc32 = 0xFFFFFFFF;

	while(*szString != '\0')
	{
		dwCrc32 = ((dwCrc32) >> 8) ^ arrdwCrc32Table[((unsigned char)*szString) ^ ((dwCrc32) & 0x000000FF)];
		szString++;
	}

	return ~dwCrc32;
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
		return delims.find_first_of(ch) != string::npos;
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
string StringTokenizer::next_token ()
{
	const char* s = operator()();
	if (s) 
		return s;
	else 
		return string();
}



// redifined operators new and delete are used to find memory leaks
#ifdef DETECT_MEMORY_LEAK
#undef new
	bool bStartTraceAllocation = false;
	void* operator new(size_t nSize, LPCSTR lpszFileName, int nLine)
	{
		void *p = new char[nSize];
		if (bStartTraceAllocation)
			rml_TRACE("allocation %X size %i from file %s line %i\n", p, nSize, lpszFileName, nLine);
		return p;
	};

	void  operator delete(void* p, LPCSTR lpszFileName, int nLine)
	{
		delete p;
	};
#endif



string ConvertASCIIToHtmlSymbols(const string& txt)
{
	string out;
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


bool  MakeDir(const string& txt)
{
	#ifdef WIN32
		return _mkdir(txt.c_str()) != -1;
	#else
		return mkdir(txt.c_str(), 0777) != -1;
	#endif
};






bool RemoveWithPrint (const string& FileName)
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
    static const DWORD mask[sizeof(DWORD)] = {
        0x00000000, 0x000000FF, 0x0000FFFF, 0x00FFFFFF
    };
    size_t len =     strlen(pc);
    size_t h = len;
    size_t i = len / sizeof(DWORD);
    const DWORD *c = (const DWORD *)pc;

    while (i--)
        h ^= *c++;

    i = len % sizeof(DWORD);
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

string BuildRMLPath (const char* s)
{
    string path = s;
    size_t i = path.find("$RML");
    if (i != string::npos)
        path.replace(i, 4, GetRmlVariable());
    return path;
}
