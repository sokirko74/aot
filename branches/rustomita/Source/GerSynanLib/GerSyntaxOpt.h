// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#ifndef ger_syntax_init_h
#define ger_syntax_init_h

#pragma warning(disable:4786)



#include "GerSynan.h"
#include "../AgramtabLib/GerGramTab.h"
#include "GerOborot.h"
#include "../SynCommonLib/SyntaxInit.h"



class CGerSyntaxOpt :  public CSyntaxOpt
{
public:
	StringVector* m_pAdjPrp;

    CGerSyntaxOpt (MorphLanguageEnum langua);
	void DestroyOptions ();


	bool InitOptionsLanguageSpecific();
	CAgramtab* GetNewGramTab () const;


	bool IsSimilarGroup (int type) const;
	bool IsGroupWithoutWeight(int GroupType, const char* cause) const;
	bool is_firm_group(int GroupType) const;

	
};


typedef enum {
 gDET_ADJ_NOMEN, gWEB_ADDR, gKEYB, gPREP_NOUN, gMODIF_ADJ, 
 gSIMILAR_ADJ,gSIMILAR_NOMEN, gSIMILAR_ADV,
 gGEN_NOUN, gDIR_OBJ, gNOUN_GR, gNOUN_ADJ, gVERB_MODIF,
 gADJ_NOMEN, gFAMILIE_GROUP, gBERUF_NP, gGENIT_NP,
 gSIMILAR_NP, gAPPOSITION, gDISKONT_KONJ, gDISRUPT_CONJ_RELATION,
 gOBOROTS, gSUBJ,
 gSPNAME, gTITLE, gPNAME,
 gPERSON, gAPPOS, gADDITION,
 gMODIF_NUMER, gGEOGR, gGENIT_PRE

} gGroupTypeEnum;



#endif
