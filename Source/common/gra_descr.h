#pragma once

#include <string>

enum Descriptors 
{
	 OFirstUnused  = 0, //unused
	 ORLE   = 1, 
	 OLLE  = 2, 
	 ODel  = 3, 
	 OPun  = 4, 
	 ODigits   = 5, 
	 ONumChar = 6, 
	 OElectAddr  = 7, 
	 OUnk  = 8, 
	 OSpc  = 9, 
	 OHyp  = 10, 
	 OEOLN = 11,  
	 OEOP  = 12, 
	 OPlu  = 13, 
	 OUp   = 14, 
	 OLw   = 15, 
	 OUpLw = 16, 
	 ODummy17  = 17, 
	 OOpn  = 18, 
	 OCls  = 19,
	 ONil  = 20, 
	 OBullet = 21, 
	 OPar    = 22,
	 OFixedOborot = 23,
	 OQuoted = 24, 
	 OParagraph = 25,


	 CS_Undef   = 26, 
	 CS_Simple  = 27, 
	 CS_Quasi   = 28, 
	 CS_Parent  = 29, 
	 CS_Heading = 30, 
	 CS_Explan  = 31, 
	 CS_Doc     = 32, 
    
     OSentEnd  = 33,
     ORoman   = 34,

	 OEXPR1    = 35, 
	 OEXPR2    = 36,    
	 OFAM1   = 37, 
	 OFAM2   = 38, 
	 ORef1      = 39, 
	 ORef2      = 40, 
	 OFile1 = 41, 
	 OFile2 = 42,
	 OAbbr1 = 43, 
	 OAbbr2 = 44,
	 OKey1 =  45, 
	 OKey2 =  46,
	 OGerDivComp1 =  47, 
	 OGerDivComp2 =  48,

     NumberOfGraphematicalDescriptors = 49,
	 OTHER_TOKEN_TYPE = 49
	 
};
//const int OTHER_TOKEN_TYPE = NumberOfGraphematicalDescriptors;

extern bool IsFirstMemberOfPairDesciptor(Descriptors d);
extern bool IsSecondMemberOfPairDesciptor(Descriptors d);
extern Descriptors GetSecondMemberByTheFirst(Descriptors d);
extern Descriptors GetFirstMemberByTheSecond(Descriptors d);
extern const std::string& GetDescriptorStr(Descriptors d);
extern uint64_t parse_graphem_descriptors(const std::string& s);

// max length of the token
const size_t CriticalTokenLength = 255;

// length of the token and its graphematical descriptors 
const size_t CriticalGraphemLineLength = CriticalTokenLength*2;


