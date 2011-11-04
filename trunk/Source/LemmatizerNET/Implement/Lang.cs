using System;
using System.Collections.Generic;
using System.Text;

namespace LemmatizerNET.Implement {
	internal static class Lang {
		[Flags]
		private enum LangType : short {
			None = 0,
			fWordDelim = 1,
			RusUpper = 2,
			RusLower = 4,
			GerUpper = 8,
			GerLower = 16,
			EngUpper = 32,
			EngLower = 64,
			OpnBrck = 128,
			ClsBrck = 256,
			UpRomDigits = 512,
			LwRomDigits = 1024,
			LatinVowel = 2048,
			RussianVowel = 4096,
			URL_CHAR = 8192
		}
		static LangType[] ASCII = new[]{
/*null (nul)*/                                     LangType.fWordDelim,
/*start of heading (soh)*/                         LangType.fWordDelim,
/*start of text (stx)*/                            LangType.fWordDelim,
/*end of text (etx)*/                              LangType.fWordDelim,
/*end of transmission (eot)*/                      LangType.fWordDelim,
/*enquiry (enq)*/                                  LangType.fWordDelim,
/*acknowledge (ack)*/                              LangType.fWordDelim,
/*bell (bel)*/                                     LangType.fWordDelim,
/*backspace (bs)*/                                 LangType.fWordDelim,
/*character tabulation (ht)*/                      LangType.fWordDelim,
/*line feed (lf)*/                                 LangType.fWordDelim,
/*line tabulation (vt)*/                           LangType.fWordDelim,
/*form feed (ff)*/                                 LangType.fWordDelim,
/*carriage return (cr)*/                           LangType.fWordDelim,
/*shift out (so)*/                                 LangType.fWordDelim,
/*shift in (si)*/                                  LangType.fWordDelim,
/*datalink escape (dle)*/                          LangType.fWordDelim,
/*device control one (dc1)*/                       LangType.fWordDelim,
/*device control two (dc2)*/                       LangType.fWordDelim,
/*device control three (dc3)*/                     LangType.fWordDelim,
/*device control four (dc4)*/                      LangType.fWordDelim,
/*negative acknowledge (nak)*/                     LangType.fWordDelim,
/*syncronous idle (syn)*/                          LangType.fWordDelim,
/*end of transmission block (etb)*/                LangType.fWordDelim,
/*cancel (can)*/                                   LangType.fWordDelim,
/*end of medium (em)*/                             LangType.fWordDelim,
/*substitute (sub)*/                               LangType.fWordDelim,
/*escape (esc)*/                                   LangType.fWordDelim,
/*file separator (is4)*/                           LangType.fWordDelim,
/*group separator (is3)*/                          LangType.fWordDelim,
/*record separator (is2)*/                         LangType.fWordDelim,
/*unit separator (is1)*/                           LangType.fWordDelim,
/*space ' '*/                                      LangType.fWordDelim,
/*exclamation mark '!'*/                           LangType.fWordDelim|LangType.URL_CHAR,
/*quotation mark '"'*/                             LangType.fWordDelim,
/*number sign '#'*/                                LangType.fWordDelim,
/*dollar sign '$'*/                                LangType.fWordDelim|LangType.URL_CHAR,
/*percent sign '%'*/                               LangType.fWordDelim|LangType.URL_CHAR,
/*ampersand '&'*/                                  LangType.fWordDelim|LangType.URL_CHAR,
/*apostrophe '''*/                                 LangType.fWordDelim|LangType.RusUpper|LangType.RusLower, // твердый знак в виде апострофа
/*left parenthesis '('*/                           LangType.fWordDelim|LangType.OpnBrck|LangType.URL_CHAR,
/*right parenthesis ')'*/                          LangType.fWordDelim|LangType.ClsBrck|LangType.URL_CHAR,
/*asterisk '*'*/                                   LangType.fWordDelim|LangType.URL_CHAR,
/*plus sign '+'*/                                  LangType.fWordDelim|LangType.URL_CHAR,
/*comma ','*/                                      LangType.fWordDelim|LangType.URL_CHAR,
/*hyphen-minus '-'*/                               LangType.fWordDelim|LangType.URL_CHAR,
/*full stop '.'*/                                  LangType.fWordDelim|LangType.URL_CHAR,
/*solidus '/'*/                                    LangType.fWordDelim|LangType.URL_CHAR,
/*digit zero '0'*/                                 LangType.URL_CHAR,
/*digit one '1'*/                                  LangType.URL_CHAR,
/*digit two '2'*/                                  LangType.URL_CHAR,
/*digit three '3'*/                                LangType.URL_CHAR,
/*digit four '4'*/                                 LangType.URL_CHAR,
/*digit five '5'*/                                 LangType.URL_CHAR,
/*digit six '6'*/                                  LangType.URL_CHAR,
/*digit seven '7'*/                                LangType.URL_CHAR,
/*digit eight '8'*/                                LangType.URL_CHAR,
/*digit nine '9'*/                                 LangType.URL_CHAR,
/*colon ':'*/                                      LangType.fWordDelim|LangType.URL_CHAR,
/*semicolon ';'*/                                  LangType.fWordDelim|LangType.URL_CHAR,
/*less-than sign '<'*/                             LangType.fWordDelim|LangType.OpnBrck,
/*equals sign '='*/                                LangType.fWordDelim|LangType.URL_CHAR,
/*greater-than sign '>'*/                          LangType.fWordDelim|LangType.ClsBrck,
/*question mark '?'*/                              LangType.fWordDelim|LangType.URL_CHAR,
/*commercial at '@'*/                              LangType.fWordDelim|LangType.URL_CHAR,
/*latin capital letter a 'A'*/                     LangType.GerUpper|LangType.EngUpper|LangType.LatinVowel,
/*latin capital letter b 'B'*/                     LangType.GerUpper|LangType.EngUpper,
/*latin capital letter c 'C'*/                     LangType.GerUpper|LangType.EngUpper,
/*latin capital letter d 'D'*/                     LangType.GerUpper|LangType.EngUpper,
/*latin capital letter e 'E'*/                     LangType.GerUpper|LangType.EngUpper|LangType.LatinVowel,
/*latin capital letter f 'F'*/                     LangType.GerUpper|LangType.EngUpper,
/*latin capital letter g 'G'*/                     LangType.GerUpper|LangType.EngUpper,
/*latin capital letter h 'H'*/                     LangType.GerUpper|LangType.EngUpper,
/*latin capital letter i 'I'*/                     LangType.GerUpper|LangType.EngUpper|LangType.UpRomDigits|LangType.LatinVowel,
/*latin capital letter j 'J'*/                     LangType.GerUpper|LangType.EngUpper,
/*latin capital letter k 'K'*/                     LangType.GerUpper|LangType.EngUpper,
/*latin capital letter l 'L'*/                     LangType.GerUpper|LangType.EngUpper|LangType.UpRomDigits,
/*latin capital letter m 'M'*/                     LangType.GerUpper|LangType.EngUpper,
/*latin capital letter n 'N'*/                     LangType.GerUpper|LangType.EngUpper,
/*latin capital letter o 'O'*/                     LangType.GerUpper|LangType.EngUpper|LangType.LatinVowel,
/*latin capital letter p 'P'*/                     LangType.GerUpper|LangType.EngUpper,
/*latin capital letter q 'Q'*/                     LangType.GerUpper|LangType.EngUpper,
/*latin capital letter r 'R'*/                     LangType.GerUpper|LangType.EngUpper,
/*latin capital letter s 'S'*/                     LangType.GerUpper|LangType.EngUpper,
/*latin capital letter t 'T'*/                     LangType.GerUpper|LangType.EngUpper,
/*latin capital letter u 'U'*/                     LangType.GerUpper|LangType.EngUpper|LangType.LatinVowel,
/*latin capital letter v 'V'*/                     LangType.GerUpper|LangType.EngUpper|LangType.UpRomDigits,
/*latin capital letter w 'W'*/                     LangType.GerUpper|LangType.EngUpper,
/*latin capital letter x 'X'*/                     LangType.GerUpper|LangType.EngUpper|LangType.UpRomDigits,
/*latin capital letter y 'Y'*/                     LangType.GerUpper|LangType.EngUpper,
/*latin capital letter z 'Z'*/                     LangType.GerUpper|LangType.EngUpper,
/*left square bracket '['*/                        LangType.fWordDelim|LangType.OpnBrck,
/*reverse solidus '\'*/                            LangType.fWordDelim,
/*right square bracket ']'*/                       LangType.fWordDelim|LangType.ClsBrck,
/*circumflex accent '^'*/                          LangType.fWordDelim,
/*low line '_'*/                                   LangType.fWordDelim,
/*grave accent '`'*/                               LangType.fWordDelim,
/*latin small letter a 'a'*/                       LangType.GerLower|LangType.EngLower|LangType.LatinVowel|LangType.URL_CHAR,
/*latin small letter b 'b'*/                       LangType.GerLower|LangType.EngLower|LangType.URL_CHAR,
/*latin small letter c 'c'*/                       LangType.GerLower|LangType.EngLower|LangType.URL_CHAR,
/*latin small letter d 'd'*/                       LangType.GerLower|LangType.EngLower|LangType.URL_CHAR,
/*latin small letter e 'e'*/                       LangType.GerLower|LangType.EngLower|LangType.LatinVowel|LangType.URL_CHAR,
/*latin small letter f 'f'*/                       LangType.GerLower|LangType.EngLower|LangType.URL_CHAR,
/*latin small letter g 'g'*/                       LangType.GerLower|LangType.EngLower|LangType.URL_CHAR,
/*latin small letter h 'h'*/                       LangType.GerLower|LangType.EngLower|LangType.URL_CHAR,
/*latin small letter i 'i'*/                       LangType.GerLower|LangType.EngLower|LangType.LwRomDigits|LangType.LatinVowel|LangType.URL_CHAR,
/*latin small letter j 'j'*/                       LangType.GerLower|LangType.EngLower|LangType.URL_CHAR,
/*latin small letter k 'k'*/                       LangType.GerLower|LangType.EngLower|LangType.URL_CHAR,
/*latin small letter l 'l'*/                       LangType.GerLower|LangType.EngLower|LangType.LwRomDigits|LangType.URL_CHAR,
/*latin small letter m 'm'*/                       LangType.GerLower|LangType.EngLower|LangType.URL_CHAR,
/*latin small letter n 'n'*/                       LangType.GerLower|LangType.EngLower|LangType.URL_CHAR,
/*latin small letter o 'o'*/                       LangType.GerLower|LangType.EngLower|LangType.LatinVowel|LangType.URL_CHAR,
/*latin small letter p 'p'*/                       LangType.GerLower|LangType.EngLower|LangType.URL_CHAR,
/*latin small letter q 'q'*/                       LangType.GerLower|LangType.EngLower|LangType.URL_CHAR,
/*latin small letter r 'r'*/                       LangType.GerLower|LangType.EngLower|LangType.URL_CHAR,
/*latin small letter s 's'*/                       LangType.GerLower|LangType.EngLower|LangType.URL_CHAR,
/*latin small letter t 't'*/                       LangType.GerLower|LangType.EngLower|LangType.URL_CHAR,
/*latin small letter u 'u'*/                       LangType.GerLower|LangType.EngLower|LangType.LatinVowel|LangType.URL_CHAR,
/*latin small letter v 'v'*/                       LangType.GerLower|LangType.EngLower|LangType.LwRomDigits|LangType.URL_CHAR,
/*latin small letter w 'w'*/                       LangType.GerLower|LangType.EngLower|LangType.URL_CHAR,
/*latin small letter x 'x'*/                       LangType.GerLower|LangType.EngLower|LangType.LwRomDigits|LangType.URL_CHAR,
/*latin small letter y 'y'*/                       LangType.GerLower|LangType.EngLower|LangType.URL_CHAR,
/*latin small letter z 'z'*/                       LangType.GerLower|LangType.EngLower|LangType.URL_CHAR,
/*left curly bracket '{'*/                         LangType.fWordDelim|LangType.OpnBrck,
/*vertical line '|'*/                              LangType.fWordDelim,
/*right curly bracket '}'*/                        LangType.fWordDelim|LangType.ClsBrck,
/*tilde '~'*/                                      LangType.fWordDelim,
/*delete ''*/                                     LangType.None,
/*padding character (pad) '_'*/                    LangType.fWordDelim,
/*high octet preset (hop) '_'*/                    LangType.None,
/*break permitted here (bph) '''*/                 LangType.None,
/*no break here (nbh) '_'*/                        LangType.fWordDelim,
/*index (ind) '"'*/                                LangType.None,
/*next line (nel) ':'*/                            LangType.fWordDelim,
/*start of selected area (ssa) '+'*/               LangType.fWordDelim,
/*end of selected area (esa) '+'*/                 LangType.fWordDelim,
/*character tabulation set (hts) '_'*/             LangType.fWordDelim,
/*character tabulation with justification (htj) '%'*/ LangType.fWordDelim,
/*line tabulation set (vts) '_'*/                  LangType.None,
/*partial line forward (pld) '<'*/                 LangType.fWordDelim,
/*partial line backward (plu) '_'*/                LangType.fWordDelim,
/*reverse line feed (ri) '_'*/                     LangType.fWordDelim,
/*single-shift two (ss2) '_'*/                     LangType.fWordDelim,
/*single-shift three (ss3) '_'*/                   LangType.fWordDelim,
/*device control string (dcs) '_'*/                LangType.fWordDelim,
/*private use one (pu1) '''*/                      LangType.fWordDelim,
/*private use two (pu2) '''*/                      LangType.fWordDelim,
/*set transmit state (sts) '"'*/                   LangType.fWordDelim,
/*cancel character (cch) '"'*/                     LangType.fWordDelim,
/*message waiting (mw) ''*/                       LangType.fWordDelim,
/*start of guarded area (spa) '-'*/                LangType.fWordDelim,
/*end of guarded area (epa) '-'*/                  LangType.fWordDelim,
/*start of string (sos) '_'*/                      LangType.fWordDelim,
/*single graphic character introducer (sgci) 'T'*/ LangType.fWordDelim,
/*single character introducer (sci) '_'*/          LangType.fWordDelim,
/*control sequence introducer (csi) '>'*/          LangType.fWordDelim,
/*string terminator (st) '_'*/                     LangType.fWordDelim,
/*operating system command (osc) '_'*/             LangType.fWordDelim,
/*privacy message (pm) '_'*/                       LangType.fWordDelim,
/*application program command (apc) '_'*/          LangType.fWordDelim,
/*no-break space ' '*/                             LangType.fWordDelim,
/*inverted exclamation mark 'Ў'*/                  LangType.fWordDelim,
/*cent sign 'ў'*/                                  LangType.fWordDelim,
/*pound sign '_'*/                                 LangType.fWordDelim,
/*currency sign '¤'*/                              LangType.fWordDelim,
/*yen sign '_'*/                                   LangType.fWordDelim,
/*broken bar '¦'*/                                 LangType.fWordDelim,
/*section sign '§'*/                               LangType.fWordDelim,
/*diaeresis 'Ё'*/                                  LangType.fWordDelim|LangType.RusUpper|LangType.RussianVowel,
/*copyright sign 'c'*/                             LangType.fWordDelim,
/*feminine ordinal indicator 'Є'*/                 LangType.fWordDelim,
/*left pointing double angle quotation mark '<'*/  LangType.fWordDelim,
/*not sign '¬'*/                                   LangType.fWordDelim,
/*soft hyphen '-'*/                                LangType.fWordDelim,
/*registered sign 'R'*/                            LangType.fWordDelim,
/*macron 'Ї'*/                                     LangType.fWordDelim,
/*degree sign '°'*/                                LangType.fWordDelim,
/*plus-minus sign '+'*/                            LangType.fWordDelim,
/*superscript two '_'*/                            LangType.fWordDelim,
/*superscript three '_'*/                          LangType.fWordDelim,
/*acute '_'*/                                      LangType.fWordDelim,
/*micro sign 'ч'*/                                 LangType.fWordDelim|LangType.GerLower|LangType.GerUpper,
/*pilcrow sign '¶'*/                               LangType.fWordDelim,
/*middle dot '·'*/                                 LangType.fWordDelim,
/*cedilla 'ё'*/                                    LangType.RusLower|LangType.RussianVowel,
/*superscript one '№'*/                            LangType.fWordDelim,
/*masculine ordinal indicator 'є'*/                LangType.fWordDelim,
/*right pointing double angle quotation mark '>'*/ LangType.fWordDelim,
/*vulgar fraction one quarter '_'*/                LangType.fWordDelim,
/*vulgar fraction one half '_'*/                   LangType.fWordDelim,
/*vulgar fraction three quarters '_'*/             LangType.fWordDelim,
/*inverted question mark 'ї'*/                     LangType.fWordDelim,
/*latin capital letter a with grave 'А'*/          LangType.RusUpper|LangType.RussianVowel,
/*latin capital letter a with acute 'Б'*/          LangType.RusUpper,
/*latin capital letter a with circumflex 'В'*/     LangType.RusUpper|LangType.GerUpper|LangType.EngUpper|LangType.LatinVowel,
/*latin capital letter a with tilde 'Г'*/          LangType.RusUpper,
/*latin capital letter a with diaeresis 'Д'*/      LangType.RusUpper|LangType.GerUpper|LangType.LatinVowel,
/*latin capital letter a with ring above 'Е'*/     LangType.RusUpper|LangType.RussianVowel,
/*latin capital ligature ae 'Ж'*/                  LangType.RusUpper,
/*latin capital letter c with cedilla 'З'*/        LangType.RusUpper|LangType.GerUpper|LangType.EngUpper,
/*latin capital letter e with grave 'И'*/          LangType.RusUpper|LangType.GerUpper|LangType.EngUpper|LangType.LatinVowel|LangType.RussianVowel,
/*latin capital letter e with acute 'Й'*/          LangType.RusUpper|LangType.GerUpper|LangType.EngUpper|LangType.LatinVowel,
/*latin capital letter e with circumflex 'К'*/     LangType.RusUpper|LangType.GerUpper|LangType.EngUpper|LangType.LatinVowel,
/*latin capital letter e with diaeresis 'Л'*/      LangType.RusUpper,
/*latin capital letter i with grave 'М'*/          LangType.RusUpper,
/*latin capital letter i with acute 'Н'*/          LangType.RusUpper,
/*latin capital letter i with circumflex 'О'*/     LangType.RusUpper|LangType.RussianVowel,
/*latin capital letter i with diaeresis 'П'*/      LangType.RusUpper,
/*latin capital letter eth (icelandic) 'Р'*/       LangType.RusUpper,
/*latin capital letter n with tilde 'С'*/          LangType.RusUpper|LangType.GerUpper|LangType.EngUpper,
/*latin capital letter o with grave 'Т'*/          LangType.RusUpper,
/*latin capital letter o with acute 'У'*/          LangType.RusUpper|LangType.RussianVowel,
/*latin capital letter o with circumflex 'Ф'*/     LangType.RusUpper|LangType.GerUpper|LangType.EngUpper|LangType.LatinVowel,
/*latin capital letter o with tilde 'Х'*/          LangType.RusUpper,
/*latin capital letter o with diaeresis 'Ц'*/      LangType.RusUpper|LangType.GerUpper|LangType.EngUpper|LangType.LatinVowel,
/*multiplication sign 'Ч'*/                        LangType.RusUpper,
/*latin capital letter o with stroke 'Ш'*/         LangType.RusUpper|LangType.UpRomDigits,
/*latin capital letter u with grave 'Щ'*/          LangType.RusUpper,
/*latin capital letter u with acute 'Ъ'*/          LangType.RusUpper,
/*latin capital letter u with circumflex 'Ы'*/     LangType.RusUpper|LangType.GerUpper|LangType.EngUpper|LangType.LatinVowel|LangType.RussianVowel,
/*latin capital letter u with diaeresis 'Ь'*/      LangType.RusUpper|LangType.GerUpper|LangType.LatinVowel,
/*latin capital letter y with acute 'Э'*/          LangType.RusUpper|LangType.RussianVowel,
/*latin capital letter thorn (icelandic) 'Ю'*/     LangType.RusUpper|LangType.RussianVowel,
/*latin small letter sharp s (german) 'Я'*/        LangType.RusUpper|LangType.GerLower|LangType.GerUpper|LangType.RussianVowel,
/*latin small letter a with grave 'а'*/            LangType.RusLower|LangType.RussianVowel,
/*latin small letter a with acute 'б'*/            LangType.RusLower,
/*latin small letter a with circumflex 'в'*/       LangType.RusLower|LangType.GerLower|LangType.EngLower|LangType.LatinVowel,
/*latin small letter a with tilde 'г'*/            LangType.RusLower,
/*latin small letter a with diaeresis 'д'*/        LangType.RusLower|LangType.GerLower|LangType.LatinVowel,
/*latin small letter a with ring above 'е'*/       LangType.RusLower|LangType.RussianVowel,
/*latin small ligature ae 'ж'*/                    LangType.RusLower,
/*latin small letter c with cedilla 'з'*/          LangType.RusLower|LangType.GerLower|LangType.EngLower,
/*latin small letter e with grave 'и'*/            LangType.RusLower|LangType.GerLower|LangType.EngLower|LangType.LatinVowel|LangType.RussianVowel,
/*latin small letter e with acute 'й'*/            LangType.RusLower|LangType.GerLower|LangType.EngLower|LangType.LatinVowel,
/*latin small letter e with circumflex 'к'*/       LangType.RusLower|LangType.GerLower|LangType.EngLower|LangType.LatinVowel,
/*latin small letter e with diaeresis 'л'*/        LangType.RusLower,
/*latin small letter i with grave 'м'*/            LangType.RusLower,
/*latin small letter i with acute 'н'*/            LangType.RusLower,
/*latin small letter i with circumflex 'о'*/       LangType.RusLower|LangType.RussianVowel,
/*latin small letter i with diaeresis 'п'*/        LangType.RusLower,
/*latin small letter eth (icelandic) 'р'*/         LangType.RusLower,
/*latin small letter n with tilde 'с'*/            LangType.RusLower|LangType.GerLower|LangType.EngLower,
/*latin small letter o with grave 'т'*/            LangType.RusLower,
/*latin small letter o with acute 'у'*/            LangType.RusLower|LangType.RussianVowel,
/*latin small letter o with circumflex 'ф'*/       LangType.RusLower|LangType.GerLower|LangType.EngLower|LangType.LatinVowel,
/*latin small letter o with tilde 'х'*/            LangType.RusLower,
/*latin small letter o with diaeresis 'ц'*/        LangType.RusLower|LangType.GerLower|LangType.EngLower|LangType.LatinVowel,
/*division sign 'ч'*/                              LangType.RusLower,
/*latin small letter o with stroke 'ш'*/           LangType.RusLower,
/*latin small letter u with grave 'щ'*/            LangType.RusLower,
/*latin small letter u with acute 'ъ'*/            LangType.RusLower,
/*latin small letter u with circumflex 'ы'*/       LangType.RusLower|LangType.GerLower|LangType.EngLower|LangType.LatinVowel|LangType.RussianVowel,
/*latin small letter u with diaeresis 'ь'*/        LangType.RusLower|LangType.GerLower|LangType.LatinVowel,
/*latin small letter y with acute 'э'*/            LangType.RusLower|LangType.RussianVowel,
/*latin small letter thorn (icelandic) 'ю'*/       LangType.RusLower|LangType.RussianVowel,
/*latin small letter y with diaeresis  'я'*/       LangType.RusLower|LangType.RussianVowel
};
		internal static bool is_alpha(byte x) {
			return is_russian_alpha(x) || is_german_alpha(x);
		}
		internal static bool is_alpha(byte x, InternalMorphLanguage Langua) {
			switch (Langua) {
				case InternalMorphLanguage.morphRussian: return is_russian_alpha(x);
				case InternalMorphLanguage.morphEnglish: return is_english_alpha(x);
				case InternalMorphLanguage.morphGerman: return is_german_alpha(x);
				case InternalMorphLanguage.morphGeneric: return is_generic_alpha(x);
				case InternalMorphLanguage.morphURL: return is_URL_alpha(x);
			}
			throw new MorphException("unknown char x");
		}
		internal static bool is_russian_alpha(byte x) {
			return is_russian_lower(x) || is_russian_upper(x);
		}
		internal static bool is_english_alpha(byte x) {
			return is_english_lower(x) || is_english_upper(x);
		}
		internal static bool is_german_alpha(byte x) {
			return is_german_lower(x) || is_german_upper(x);
		}
		internal static bool is_generic_alpha(byte x) {
			return is_english_alpha(x) || x >= 128;
		}
		internal static bool is_URL_alpha(byte x) {
			return (ASCII[x] & LangType.URL_CHAR) > 0;
		}
		internal static bool is_russian_lower(byte x) {
			return (ASCII[x] & LangType.RusLower) > 0;
		}
		internal static bool is_russian_upper(byte x) {
			return (ASCII[x] & LangType.RusUpper) > 0;
		}
		internal static bool is_english_upper(byte x) {
			return (ASCII[x] & LangType.EngUpper) > 0;
		}
		internal static bool is_english_lower(byte x) {
			return (ASCII[x] & LangType.EngLower) > 0;
		}
		internal static bool is_german_lower(byte x) {
			return (ASCII[x] & LangType.GerLower) > 0;
		}
		internal static bool is_german_upper(byte x) {
			return (ASCII[x] & LangType.GerUpper) > 0;
		}
		internal static bool is_upper_alpha(byte x, InternalMorphLanguage Langua) {
			switch (Langua) {
				case InternalMorphLanguage.morphRussian: return is_russian_upper(x);
				case InternalMorphLanguage.morphEnglish: return is_english_upper(x);
				case InternalMorphLanguage.morphGerman: return is_german_upper(x);
				case InternalMorphLanguage.morphGeneric: return is_generic_upper(x);
				case InternalMorphLanguage.morphURL: return false;
			}
			return false;
		}
		internal static bool is_generic_upper(byte x) {
			// why ,,
			return (ASCII[x] & LangType.EngUpper) > 0;
		}
		internal static bool is_upper_vowel(byte x, InternalMorphLanguage Langua) {
			switch (Langua) {
				case InternalMorphLanguage.morphRussian: return is_russian_upper_vowel(x);
				case InternalMorphLanguage.morphEnglish: return is_english_upper_vowel(x);
				case InternalMorphLanguage.morphGerman: return is_german_upper_vowel(x);
			};
			return false;
		}
		internal static bool is_russian_upper_vowel(byte x) {
			return ((ASCII[x] & LangType.RusUpper) > 0)
				&& ((ASCII[x] & LangType.RussianVowel) > 0);
		}
		internal static bool is_english_upper_vowel(byte x) {
			return ((ASCII[x] & LangType.EngUpper) > 0)
				&& ((ASCII[x] & LangType.LatinVowel) > 0);
		}
		internal static bool is_german_upper_vowel(byte x) {
			return (ASCII[x] & LangType.GerUpper) > 0
				&& (ASCII[x] & LangType.LatinVowel) > 0;
		}
		internal static bool is_lower_vowel(byte x, InternalMorphLanguage Langua) {
			switch (Langua) {
				case InternalMorphLanguage.morphRussian: return is_russian_lower_vowel(x);
				case InternalMorphLanguage.morphEnglish: return is_english_lower_vowel(x);
				case InternalMorphLanguage.morphGerman: return is_german_lower_vowel(x);
			}
			return false;
		}
		internal static bool is_russian_lower_vowel(byte x) {
			return ((ASCII[x] & LangType.RusLower) > 0)
				&& ((ASCII[x] & LangType.RussianVowel) > 0);

		}
		internal static bool is_english_lower_vowel(byte x) {
			return ((ASCII[x] & LangType.EngLower) > 0)
				&& ((ASCII[x] & LangType.LatinVowel) > 0);
		}
		internal static bool is_german_lower_vowel(byte x) {
			return (ASCII[x] & LangType.GerLower) > 0
				&& (ASCII[x] & LangType.LatinVowel) > 0;
		}
		internal static bool is_upper_consonant(byte x, InternalMorphLanguage Langua) {
			if (!is_upper_alpha(x, Langua)) return false;
			return !is_upper_vowel(x, Langua);
		}
	}
}
