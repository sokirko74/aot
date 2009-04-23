// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Graphematical Module (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1996-2001)

// this head is imported by midl
#ifndef  Descriptors_h
#define Descriptors_h

#ifndef __GRAPHANLib_LIBRARY_DEFINED__

enum Descriptors 
{
	 OBeg  = 0, 
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
	 ONam  = 17, 
	 OOpn  = 18, 
	 OCls  = 19,
	 ONil  = 20, 
	 OBullet = 21, 
	 OPar    = 22,
	 ODUMMY_PLEASE_USE_ME = 23, 
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
     NumberOfGraphematicalDescriptors = 49
	 
};
#endif




#endif
