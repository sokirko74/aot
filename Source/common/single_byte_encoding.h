#pragma once
typedef unsigned char BYTE;

#include "../contrib/iconv_subset/iconv.h"

// common single byte chars (shared between windows-1251 and windows-1252),
const BYTE GraphematicalTAB    = (BYTE) 16;
const BYTE cParagraph    = (BYTE) 21;
const BYTE Apostrophe    = (BYTE) 39;
const BYTE cHyphenChar   = (BYTE) 45;

const BYTE cEllipseChar  = (BYTE) 133;
const BYTE GraphematicalSPACE = (BYTE)160;
const BYTE OpenQuoteMark1251  = (BYTE) 171;
const BYTE cCompanyChar  = (BYTE) 176;
const BYTE cPiChar       = (BYTE) 182;
const BYTE GraphematicalEOLN = (BYTE)182;
const BYTE cIonChar      = (BYTE) 183;
const BYTE cNumberChar   = (BYTE) 185;
const BYTE CloseQuoteMark1251  = (BYTE) 187;

// windows-1251 Russian
const BYTE UpperJO       = (BYTE) 168;
const BYTE LowerJO       = (BYTE) 184;
const BYTE RussianUpperA = 192;
const BYTE RussianUpperE = 197;
const BYTE RussianLowerA = 224;
const BYTE RussianLowerE = 229;
const BYTE RussianLowerK = 234;
const BYTE RussianLowerM = 236;
const BYTE RussianLowerN = 237;
const BYTE RussianLowerO = 238;
const BYTE RussianLowerR = 240;
const BYTE RussianLowerS = 241;
const BYTE RussianLowerU = 243;
const BYTE RussianLowerX = 245;

// windows-1252  (ISO/IEC 8859-1)
const BYTE Nu  = 181;
const BYTE Azirkun = 194;
const BYTE Auml  = 196;
const BYTE Ccedille = 199;
const BYTE Egravis = 200;
const BYTE Eakut = 201;
const BYTE Ezirkun = 202;
const BYTE Ntilda = 209;
const BYTE Ozirkun = 212;
const BYTE Ouml  = 214;
const BYTE Uzirkun = 219;
const BYTE Uuml  = 220;
const BYTE szlig  = 223;
const BYTE agrave  = 224;
const BYTE azirkun = 226;
const BYTE auml  = 228;
const BYTE ccedille = 231;
const BYTE egrave  = 232;
const BYTE eacute  = 233;
const BYTE ezirkun = 234;
const BYTE ntilda = 241;
const BYTE ozirkun = 244;
const BYTE ouml  = 246;
const BYTE uzirkun = 251;
const BYTE uuml  = 252;

